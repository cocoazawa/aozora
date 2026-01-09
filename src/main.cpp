// main.cpp
// Copyright (C) 2025 cocoazawa
//
// Written by Peter G.

#include "config.h"
#include "vex.h"
#include "timekeeper.h"
#include "auxilary.h"
#include "overseer.h"
#include "render.h"
#include <cmath>

using namespace vex;


void preAutonomous(void) {
    initializeVEX();
    // primaryController.rumble(".");
    // Hold off on rendering an official interface
    vex::thread interfaceThread = thread( refreshCompetitionStatus );
}

void autonomous(void) {
    // OverseerBasis &overseer = OverseerBasis::getInstance();
    // overseer.operation(CommandType::Propel, 600.0);
}

void userControl(void) {
    OverseerBasis &overseer = OverseerBasis::getInstance();
    overseer.operation(CommandType::Propel, 600.0);

    Timekeeper timekeeperLocal(TimekeepingType::MinTime, 1.0, 5.0);
    
    // define buttons and callable functions
    auto RubberBandPistonButton = primaryController.ButtonX;
    auto IntakePistonButton = primaryController.ButtonB;
    
    auto triggerRubberBandPiston = []() { trigger(RubberBandPiston); };
    auto triggerIntakePiston = []() { trigger(IntakePiston); };
    
    // register 
    RubberBandPistonButton.pressed(triggerRubberBandPiston);
    IntakePistonButton.pressed(triggerIntakePiston);
    
    // define buttons and state variables
    auto IntakeButton = primaryController.ButtonL1;
    auto IntakeCounterspinButton = primaryController.ButtonR1;
    auto SpinnerButton = primaryController.ButtonL2;
    auto SpinnerCounterspinButton = primaryController.ButtonR2;

    bool motorIntakeStopped = false;
    bool motorSpinnerStopped = false;

    while (true) {
        timekeeperLocal.start();

        // Intake or Spinner buttons pressed respectively
        if (IntakeButton.pressing() || IntakeCounterspinButton.pressing()) {
            motorIntakeStopped = false;
            MotorIntake.spin(IntakeButton.pressing() ? forward : reverse, 600, rpm);
        } else {
            if (motorIntakeStopped != true) {
                MotorIntake.stop();
                motorIntakeStopped = true;
            }
        }

        if (SpinnerButton.pressing() || SpinnerCounterspinButton.pressing()) {
            motorSpinnerStopped = false;
            MotorSpinner.spin(SpinnerButton.pressing() ? forward : reverse, 600, rpm);
        } else {
            if (motorSpinnerStopped != true) {
                MotorSpinner.stop();
                motorSpinnerStopped = true;
            }
        }

        float velocityMultiplierTotal = 127;
        float sidewaysMultiplierTotal = 254;
        float velocityMultiplier = primaryController.Axis3.value() / velocityMultiplierTotal;
        float sidewaysMultiplier = primaryController.Axis1.value() / sidewaysMultiplierTotal;
        
        if (sidewaysMultiplier == 0 && velocityMultiplier == 0) {
            MotorLeftGroup.stop();
            MotorRightGroup.stop();
            
            auto endTime = timekeeperLocal.ending();
            wait(endTime, msec);
            continue;
        }
        
        long leftVelocity = MaximumDirectRPM * (velocityMultiplier + sidewaysMultiplier);
        long rightVelocity = MaximumDirectRPM * (velocityMultiplier - sidewaysMultiplier);

        MotorLeftGroup.spin(forward, leftVelocity, rpm);
        MotorRightGroup.spin(forward, rightVelocity, rpm);

        primaryBrain.Screen.setCursor(6, 1);
        primaryBrain.Screen.print("                   MotorLeft POSIT: ");
        primaryBrain.Screen.print(MotorLeftGroup.position(rev));
        primaryBrain.Screen.print("                    ");
        primaryBrain.Screen.setCursor(7, 1);
        primaryBrain.Screen.print("                  MotorRight POSIT: ");
        primaryBrain.Screen.print(MotorRightGroup.position(rev));
        primaryBrain.Screen.print("                    ");
        
        auto endTime = timekeeperLocal.ending();
        wait(endTime, msec);
    }
}

//
// Main will set up the competition functions and callbacks.
//
int main() {
    printf("\n\naozora -- version %s -- by cocoazawa\n", aozoraVersion);
    printf("aozora -- profile %s\n\n\n", profileVersion);
    
    // Run the pre-autonomous function.
    preAutonomous();


    // Set up callbacks for autonomous and driver control periods.
    Competition.autonomous(autonomous);
    Competition.drivercontrol(userControl);

    // Prevent main from exiting with an infinite loop.
    while (true) {
        wait(100, msec);
    }
}

