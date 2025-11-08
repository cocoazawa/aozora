// main.cpp
// Copyright (C) 2025 Project Aozora Collaborators
//
// Written by Peter G.

#include "config.h"
#include "vex.h"

using namespace vex;

competition Competition;
controller control = controller();

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
    auto triggerRubberBand = []() { trigger(RubberBandPiston); };
    auto triggerIntake = []() { trigger(IntakePiston); };

    while (1) {
        control.ButtonL1.pressed(triggerRubberBand);
        control.ButtonL2.pressed(triggerIntake);

        wait(10, msec);
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
