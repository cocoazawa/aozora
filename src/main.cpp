// main.cpp
// Copyright (C) 2025 Project Aozora Collaborators
//
// Written by Peter G.

#include "config.h"
#include "vex.h"
#include "chronoAssist.h"
#include "auxilary.h"

using namespace vex;

// introduce competition and controller objects
competition Competition;
controller primaryController = controller();


void preAutonomous(void) {
    initializeVEX();
}

void autonomous(void) {
}

void userControl(void) {
    ChronoAssist chronos(1.0);
    
    auto RubberBandPistonButton = primaryController.ButtonX;
    auto IntakePistonButton = primaryController.ButtonB;
    auto IntakeButton = primaryController.ButtonL2;
    auto SpinnerButton = primaryController.ButtonL1;

    auto triggerRubberBandPiston = []() { trigger(RubberBandPiston); };
    auto triggerIntakePiston = []() { trigger(IntakePiston); };

    RubberBandPistonButton.pressed(triggerRubberBandPiston);
    RubberBandPistonButton.released(triggerRubberBandPiston);
    IntakePistonButton.pressed(triggerRubberBandPiston);
    IntakePistonButton.released(triggerRubberBandPiston);

    while (1) {
        chronos.start();

        // Intake or Spinner buttons pressed respectively
        switch(IntakeButton.pressing()){
            case true:
                MotorIntake.spin(forward, 600, rpm);
                break;
            default:
                MotorIntake.stop();
                break;
        }
        switch(SpinnerButton.pressing()){
            case true:
                MotorSpinner.spin(forward, 600, rpm);
                break;
            default:
                MotorSpinner.stop();
                break;
        }

        int velocityMultiplier = primaryController.Axis2.position() / 100;
        int sidewaysMultiplier = primaryController.Axis1.position() / 100;

        int leftVelocity = velocityMultiplier * MaximumDirect_RPM;
        int rightVelocity = velocityMultiplier * MaximumDirect_RPM;

        if (velocityMultiplier == 0) {
            leftVelocity = MaximumDirect_RPM * sidewaysMultiplier;
            rightVelocity = MaximumDirect_RPM * sidewaysMultiplier;
        } else if (sidewaysMultiplier < 0) {
            leftVelocity = velocityMultiplier * MaximumDirect_RPM * abs(sidewaysMultiplier);
        } else if (sidewaysMultiplier > 0) {
            rightVelocity = velocityMultiplier * MaximumDirect_RPM * abs(sidewaysMultiplier);
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
    // Set up callbacks for autonomous and driver control periods.
    Competition.autonomous(autonomous);
    Competition.drivercontrol(userControl);

    // Run the pre-autonomous function.
    preAutonomous();

    // Prevent main from exiting with an infinite loop.
    while (true) {
        wait(100, msec);
    }
}
