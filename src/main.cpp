// main.cpp
//
// Copyright (C) 2025 cocoazawa

#include "robot-config.cpp"
#include "vex.h"

using namespace vex;

competition Competition;
controller control = controller();

digital_out piston = digital_out(Brain.ThreeWirePort.H);

void trigger(digital_out &triggerable)
{
    switch (triggerable.value())
    {
    case 1:
        triggerable.set(false);
        break;
    case 0:
        triggerable.set(true);
        break;
    default:
        break;
    }
}

void preAutonomous(void)
{
    initializeVEX();
}

void autonomous(void)
{
}

void userControl(void)
{
    auto triggerPiston = []()
    { trigger(piston); };

    while (1)
    {
        control.ButtonL1.pressed(triggerPiston);

        wait(10, msec);
    }
}

//
// Main will set up the competition functions and callbacks.
//
int main()
{
    // Set up callbacks for autonomous and driver control periods.
    Competition.autonomous(autonomous);
    Competition.drivercontrol(userControl);

    // Run the pre-autonomous function.
    preAutonomous();

    // Prevent main from exiting with an infinite loop.
    while (true)
    {
        wait(100, msec);
    }
}
