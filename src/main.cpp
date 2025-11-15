// main.cpp
// Copyright (C) 2025 Project Aozora Collaborators
//
// Written by Peter G.

#include "config.h"
#include "vex.h"
#include "chronoAssist.h"
#include "auxilary.h"
#include "overseer.h"
#include <cmath>

using namespace vex;

// introduce competition and controller objects
competition Competition;
controller primaryController = controller();


void preAutonomous(void) {
    initializeVEX();
    primaryController.rumble("....");
}

void autonomous(void) {
    OverseerBasis overseer;
    overseer.operation(CommandType::Propel, 610.0);
}

void userControl(void) {
    ChronoAssist chronos(ChronoType::MinTime, 1.0, 5.0);
    
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

    while (1) {
        chronos.start();

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

        double velocityMultiplier = primaryController.Axis3.value() / 127;
        double sidewaysMultiplier = primaryController.Axis1.value() / 127;

        if (sidewaysMultiplier == 0 && velocityMultiplier == 0) {
            MotorLeftGroup.stop();
            MotorRightGroup.stop();

            auto endTime = chronos.ending();
            wait(endTime, msec);
            continue;
        }
        
        double leftVelocity = MaximumDirectRPM * (velocityMultiplier + sidewaysMultiplier);
        double rightVelocity = MaximumDirectRPM * (velocityMultiplier - sidewaysMultiplier);

        MotorLeftGroup.spin(forward, leftVelocity, rpm);
        MotorRightGroup.spin(forward, rightVelocity, rpm);

        Brain.Screen.print("MotorLeft POSIT:  ");
        Brain.Screen.print(MotorLeftGroup.position(rev));
        Brain.Screen.print("MotorRight POSIT:  ");
        Brain.Screen.print(MotorRightGroup.position(rev));
        Brain.Screen.print("                    ");
        Brain.Screen.setCursor(1, 1);
        
        auto endTime = chronos.ending();
        wait(endTime, msec);
    }
}

//
// Main will set up the competition functions and callbacks.
//
int main() {
    printf("\n[>RUNNING] aozora v0.14.8\n");

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

