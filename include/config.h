// config.h
// Copyright (C) 2025 cocoazawa
// 
// Written by Peter G.

#pragma once
#include "vex.h"

using namespace vex;

// brain setup
brain primaryBrain;

// introduce competition objects
competition Competition;


static const char* aozoraVersion = "0.17.0";
static const char* profileVersion = "SPAN-13";
#define aozoraExperimental true

// #define aozoraColor red
#define aozoraTeamColor blue

static const int adjustHorizontal = -10;
static const int adjustVertical = -15;

static const double TotalMachineDiameter = 128;




// maximum direct rpm output from the motor
double MaximumDirectRPM = 600;
double TravelDistance = 260;
double AxelWidth = 287.5;
double TotalCircumference = AxelWidth * M_PI * 2;
// ratio remapping from the direct output to the actual output (yes, this CAN be simplified; it SHOULD NOT BE SIMPLIFIED)
double GearRatio = 36.0 / 60.0;
// time in the unit of msec
double ElapseGoalTime = 8;


// Overseer

/// @details Speed Last, Accuracy First (SLAF) Profile
// double ProportionalMultiplier = 1.0;
// double IntegralMultiplier = 0.001;
// double DerivativeMultiplier = 0.007;
// double OverallMultiplier = 0.1;

/// @details Speed Priority, Accuracy Non-controlled (SPAN) Profile
double ProportionalMultiplier = 15.0;
double IntegralMultiplier = 0.3;
double DerivativeMultiplier = 1.00;
double OverallMultiplier = 0.1;





// tolerances for Overseer
double OverseerToleranceUpper = 1.0;
double OverseerToleranceLower = -1.0;

// the LOWER BOUNDS for the driverControl's Controller
double ControllerTolerance = 0.02;


// pistons
vex::triport::port RubberBandPistonPort = primaryBrain.ThreeWirePort.B;
vex::triport::port IntakePistonPort = primaryBrain.ThreeWirePort.A;
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
    MotorLeftFront.setBrake(vex::brakeType::coast);
    MotorLeftMid.setBrake(vex::brakeType::coast);
    MotorLeftBack.setBrake(vex::brakeType::coast);
    MotorRightFront.setBrake(vex::brakeType::coast);
    MotorRightMid.setBrake(vex::brakeType::coast);
    MotorRightBack.setBrake(vex::brakeType::coast);
    MotorSpinner.setBrake(vex::brakeType::coast);
    MotorIntake.setBrake(vex::brakeType::coast);

    primaryBrain.Screen.setOrigin(1, 40);
}