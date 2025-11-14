// robot-config.h
// Copyright (C) 2025 Project Aozora Collaborators
// 
// Written by Peter G.

#include "vex.h"

using namespace vex;

// brain setup
brain Brain;

// maximum direct rpm output from the motor
int MaximumDirect_RPM = 600;
// ratio remapping from the direct output to the actual output (yes, this CAN be simplified; it SHOULD NOT BE SIMPLIFIED)
int Ratio = 60/36;
// time in the unit of msec
int ElapseGoalTime = 8;


// pistons
vex::triport::port RubberBandPistonPort = Brain.ThreeWirePort.B;
vex::triport::port IntakePistonPort = Brain.ThreeWirePort.A;
pneumatics RubberBandPiston(RubberBandPistonPort);
pneumatics IntakePiston(IntakePistonPort);


// drivetrain setup
motor MotorLeftFront = motor(PORT5, ratio6_1, false);
motor MotorLeftMid = motor(PORT6, ratio6_1, false);
motor MotorLeftBack = motor(PORT7, ratio6_1, true);

motor MotorRightFront = motor(PORT8, ratio6_1, true);
motor MotorRightMid = motor(PORT9, ratio6_1, true);
motor MotorRightBack = motor(PORT10, ratio6_1, false);

motor_group MotorLeftGroup = motor_group(MotorLeftFront, MotorLeftMid, MotorLeftBack);
motor_group MotorRightGroup = motor_group(MotorRightFront, MotorRightMid, MotorRightBack);


// conveyor belts/spinny thing setup
motor MotorSpinner = motor(PORT4, ratio6_1, true);
motor MotorIntake = motor(PORT3, ratio6_1, true);



void initializeVEX(void) {
    MotorLeftFront.setBrake(vex::brakeType::hold);
    MotorLeftMid.setBrake(vex::brakeType::hold);
    MotorLeftBack.setBrake(vex::brakeType::hold);
    MotorRightFront.setBrake(vex::brakeType::hold);
    MotorRightMid.setBrake(vex::brakeType::hold);
    MotorRightBack.setBrake(vex::brakeType::hold);
    MotorSpinner.setBrake(vex::brakeType::hold);
    MotorIntake.setBrake(vex::brakeType::hold);
}