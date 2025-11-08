// robot-config.h
// Copyright (C) 2025 Project Aozora Collaborators
// 
// Written by Peter G.

#include "vex.h"

using namespace vex;

brain Brain;

// pistons
vex::triport::port RubberBandPistonPort = Brain.ThreeWirePort.B;
vex::triport::port IntakePistonPort = Brain.ThreeWirePort.A;
pneumatics RubberBandPiston(RubberBandPistonPort);
pneumatics IntakePiston(IntakePistonPort);


// drivetrain setup
motor MotorLeftFront = motor(PORT10);
motor MotorLeftMid = motor(PORT10);
motor MotorLeftBack = motor(PORT10);
motor MotorRightFront = motor(PORT10);
motor MotorRightMid = motor(PORT10);
motor MotorRightBack = motor(PORT10);

motor_group MotorLeftGroup = motor_group(MotorLeftFront, MotorLeftMid, MotorLeftBack);
motor_group MotorRightGroup = motor_group(MotorRightFront, MotorRightMid, MotorRightBack);

drivetrain MainDrivetrain = drivetrain(MotorLeftGroup, MotorRightGroup);

// conveyor belts/spinny thing setup
motor MotorConveyorA = motor(PORT10);
motor MotorConveyorB = motor(PORT10);

// pneumatics

void initializeVEX(void) {
    // idk why vex has this function
    // maybe for VEXCode PRO? that's deprecated tho;
}