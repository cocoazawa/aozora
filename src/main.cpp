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

void autonomousAuxilary() {
    MotorIntake.spin(forward, 600, rpm);
}

void preAutonomous(void) {
    vex::thread interfaceThread = thread( refreshCompetitionStatus );
    
    // initialize vex components
    initializeVEX();
    
    // initialize interface
    primaryController.rumble("...");
}

void autonomous(void) {
    OverseerBasis &overseer = OverseerBasis::getInstance();
    // vex::thread interfaceThread = thread( spinIntake );
    overseer.operation(CommandType::Propel, 600.0);
    // overseer.operation(CommandType::Pivot, 148.2);
    // overseer.operation(CommandType::Propel, 1235.4756169184);
    // overseer.operation(CommandType::Pivot, -148.2);
    // overseer.operation(CommandType::Propel, 750.0);
}

void userControl(void) {
    // autonomous();

    // while (true) {
    //     wait(100, msec);
    // }

    Timekeeper timekeeperLocal(TimekeepingType::MinTime, 1.0, 5.0);
    
    // register 
    DescorePistonButton.pressed(triggerDescorePiston);
    RubberBandPistonButton.pressed(triggerRubberBandPiston);
    IntakePistonButton.pressed(triggerIntakePiston);

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
        
        auto endTime = timekeeperLocal.ending();
        wait(endTime, msec);
    }
}

//
// Main will set up the competition functions and callbacks.
//
int main() {
    printf("\n\naozora -- version %s -- by cocoazawa for 76371M\n", aozoraVersion);
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


