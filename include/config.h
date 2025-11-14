// robot-config.h
// Copyright (C) 2025 Project Aozora Collaborators
// 
// Written by Peter G.

#pragma once
#include "vex.h"

using namespace vex;

// brain setup
brain Brain;

// maximum direct rpm output from the motor
double MaximumDirectRPM = 600;
double TravelDistance = 600;
double AxelWidth = 287.5;
double TotalCircumference = AxelWidth * M_PI * 2;
// ratio remapping from the direct output to the actual output (yes, this CAN be simplified; it SHOULD NOT BE SIMPLIFIED)
double GearRatio = 36/60;
// time in the unit of msec
double ElapseGoalTime = 8;


// Overseer
double ProportionalMultiplier = 0.0;
double IntegralMultiplier = 0.0;
double DerivativeMultiplier = 0.0;
double OverallMultiplier = 1.0;
// DO NOT CHANGE THE NAME: IT IS SUPPOSED TO MISMATCH
// the LOWER BOUNDS for the error DESPITE the name
double OverseerToleranceUpper = 5.0;
// the UPPER BOUNDS for the error DESPITE the name
double OverseerToleranceLower = -5.0;

// the LOWER BOUNDS for the driverControl's Controller
double ControllerTolerance = 0.02;



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
motor MotorIntake = motor(PORT3, ratio6_1, false);



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