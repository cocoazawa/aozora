// robot-config.h
// Copyright (C) 2025 Project Aozora Collaborators
// 
// Written by Peter G.

#pragma once
#include "vex.h"

using namespace vex;

// brain setup
brain Brain;

#define aozoraVersion "v0.16.12\n"


// maximum direct rpm output from the motor
double MaximumDirectRPM = 600;
double TravelDistance = 600;
double AxelWidth = 287.5;
double TotalCircumference = AxelWidth * M_PI * 2;
// ratio remapping from the direct output to the actual output (yes, this CAN be simplified; it SHOULD NOT BE SIMPLIFIED)
double GearRatio = 36.0 / 60.0;
// time in the unit of msec
double ElapseGoalTime = 8;


// Overseer
double ProportionalMultiplier = 0.5;
double IntegralMultiplier = 0.2;
double DerivativeMultiplier = 0.5;
double OverallMultiplier = 0.1;
// the LOWER BOUNDS for the error
double OverseerToleranceUpper = -5.0;
// the UPPER BOUNDS for the error
double OverseerToleranceLower = 5.0;

// the LOWER BOUNDS for the driverControl's Controller
double ControllerTolerance = 0.02;



// pistons
vex::triport::port RubberBandPistonPort = Brain.ThreeWirePort.B;
vex::triport::port IntakePistonPort = Brain.ThreeWirePort.A;
pneumatics RubberBandPiston(RubberBandPistonPort);
pneumatics IntakePiston(IntakePistonPort);


// drivetrain setup
motor MotorLeftFront(PORT5, ratio6_1, false);
motor MotorLeftMid(PORT6, ratio6_1, false);
motor MotorLeftBack(PORT7, ratio6_1, true);

motor MotorRightFront(PORT8, ratio6_1, true);
motor MotorRightMid(PORT9, ratio6_1, true);
motor MotorRightBack(PORT10, ratio6_1, false);

motor_group MotorLeftGroup(MotorLeftFront, MotorLeftMid, MotorLeftBack);
motor_group MotorRightGroup(MotorRightFront, MotorRightMid, MotorRightBack);


// conveyor belts/spinny thing setup
motor MotorSpinner(PORT4, ratio6_1, true);
motor MotorIntake(PORT3, ratio6_1, false);


// introduce controller object (moved to allow for uniform access to one reference)
controller primaryController = controller();


// gyroscope object
// introduced from aozora v0.16.1 for testing applications
inertial primaryInertial(PORT11);



void initializeVEX(void) {
    MotorLeftFront.setBrake(vex::brakeType::brake);
    MotorLeftMid.setBrake(vex::brakeType::brake);
    MotorLeftBack.setBrake(vex::brakeType::brake);
    MotorRightFront.setBrake(vex::brakeType::brake);
    MotorRightMid.setBrake(vex::brakeType::brake);
    MotorRightBack.setBrake(vex::brakeType::brake);
    MotorSpinner.setBrake(vex::brakeType::brake);
    MotorIntake.setBrake(vex::brakeType::brake);

    Brain.Screen.setOrigin(1, 40);
}