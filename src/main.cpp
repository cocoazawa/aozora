// main.cpp
// Copyright (C) 2025 Project Aozora Collaborators
//
// Written by Peter G.

#include "config.h"
#include "vex.h"
#include "chronoAssist.h"

using namespace vex;

competition Competition;
controller primaryController = controller();

void trigger(pneumatics& piston) {
    switch (piston.value()) {
        case 1:
            piston.set(false);
            break;
        case 0:
            piston.set(true);
            break;
        default:
            break;
    }
}

void preAutonomous(void) {
    initializeVEX();
}

void autonomous(void) {
}

void userControl(void) {
    ChronoAssist chronos(1.0);
    
    auto RubberBandPistonButton = primaryController.ButtonX;
    auto IntakePistonButton = primaryController.ButtonB;
    auto ConveyorAButton = primaryController.ButtonX;
    auto ConveyorBButton = primaryController.ButtonX;

    auto triggerRubberBandPiston = []() { trigger(RubberBandPiston); };
    auto triggerIntakePiston = []() { trigger(IntakePiston); };
    
    auto triggerConveyorA = []() { trigger(IntakePiston); };
    auto triggerConveyorB = []() { trigger(IntakePiston); };

    RubberBandPistonButton.pressed(triggerRubberBandPiston);
    IntakePistonButton.pressed(triggerIntakePiston);
    ConveyorAButton.pressed(triggerConveyorA);
    ConveyorBButton.pressed(triggerConveyorB);

    while (1) {
        chronos.start();

        int velocityMultiplier = primaryController.Axis2.position() / 100;
        int sidewaysMultiplier = primaryController.Axis1.position() / 100;

        int leftVelocity = velocityMultiplier * bestRPM;
        int rightVelocity = velocityMultiplier * bestRPM;

        if (velocityMultiplier == 0) {
            leftVelocity = bestRPM * sidewaysMultiplier;
            rightVelocity = bestRPM * sidewaysMultiplier;
        } else if (sidewaysMultiplier < 0) {
            leftVelocity = velocityMultiplier * bestRPM * abs(sidewaysMultiplier);
        } else if (sidewaysMultiplier > 0) {
            rightVelocity = velocityMultiplier * bestRPM * abs(sidewaysMultiplier);
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
