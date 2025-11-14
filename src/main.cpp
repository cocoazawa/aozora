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
}

void autonomous(void) {
    OverseerBasis overseer;

    overseer.operation(CommandType::Propel, 610);
}

void userControl(void) {
    ChronoAssist chronos(ChronoType::MinTime, 1.0, 10.0);
    
    // define buttons and callable functions
    auto RubberBandPistonButton = primaryController.ButtonX;
    auto IntakePistonButton = primaryController.ButtonB;
    
    auto triggerRubberBandPiston = []() { trigger(RubberBandPiston); };
    auto triggerIntakePiston = []() { trigger(IntakePiston); };
    
    // register 
    RubberBandPistonButton.pressed(triggerRubberBandPiston);
    RubberBandPistonButton.released(triggerRubberBandPiston);
    IntakePistonButton.pressed(triggerIntakePiston);
    IntakePistonButton.released(triggerIntakePiston);
    
    // define buttons and state variables
    auto IntakeButton = primaryController.ButtonL2;
    auto SpinnerButton = primaryController.ButtonL1;

    bool motorIntakeStopped = false;
    bool motorSpinnerStopped = false;

    while (1) {
        chronos.start();

        // Intake or Spinner buttons pressed respectively
        if (IntakeButton.pressing()) {
            motorIntakeStopped = false;
            MotorIntake.spin(forward, 600, rpm);
        } else {
            if (motorIntakeStopped != true) {
                MotorIntake.stop();
                motorIntakeStopped = true;
            }
        }

        if (SpinnerButton.pressing()) {
            motorSpinnerStopped = false;
            MotorSpinner.spin(forward, 600, rpm);
        } else {
            if (motorSpinnerStopped != true) {
                MotorSpinner.stop();
                motorSpinnerStopped = true;
            }
        }

        double velocityMultiplier = primaryController.Axis2.position() / 100;
        double sidewaysMultiplier = primaryController.Axis1.position() / 100;

        double leftVelocity = velocityMultiplier * MaximumDirectRPM;
        double rightVelocity = velocityMultiplier * MaximumDirectRPM;

        if (sidewaysMultiplier == 0 && velocityMultiplier == 0) {
            MotorLeftGroup.stop();
            MotorRightGroup.stop();

            auto endTime = chronos.ending();
            wait(endTime, msec);
            continue;
        }
        
        if (sidewaysMultiplier < 0) {
            leftVelocity = velocityMultiplier * MaximumDirectRPM * std::abs(1.0 - sidewaysMultiplier);
        } else if (sidewaysMultiplier > 0) {
            rightVelocity = velocityMultiplier * MaximumDirectRPM * std::abs(1.0 - sidewaysMultiplier);
        }

        MotorLeftGroup.spin(forward, leftVelocity, rpm);
        MotorRightGroup.spin(forward, rightVelocity, rpm);

        auto endTime = chronos.ending();
        wait(endTime, msec);
    }
}

//
// Main will set up the competition functions and callbacks.
//
int main() {
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
