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


static const char* aozoraVersion = "0.18.1";
#define aozoraExperimental false

// Printing Values

#define aozoraTeamColor red
// #define aozoraTeamColor blue

static const int adjustHorizontal = -10;
static const int adjustVertical = -15;

static bool Ready = false;


// Overseer Calculation Values

// maximum direct rpm output from the motor
static const double MaximumDirectRPM = 600;
static const double TotalMachineDiameter = 315.0;
static const double TotalTravelDistance = 260;
// ratio remapping from the direct output to the actual output (yes, this CAN be simplified; it SHOULD NOT BE SIMPLIFIED)
static const double GearRatio = 36.0 / 60.0;





// Overseer
double LeftSkewMultiplier = 0.86;

/// @details Speed Last, Accuracy First (SLAF-00) Profile
// static const char* profileVersion = "SLAF-00";
// double ProportionalMultiplier = 1.0;
// double IntegralMultiplier = 0.001;
// double DerivativeMultiplier = 0.007;
// double OverallMultiplier = 0.1;

/// @details Speed Priority, Accuracy Non-controlled (SPAN-14) Profile
static const char* profileVersion = "SPAN-21";
double ProportionalMultiplier = 0.78;
double IntegralMultiplier = 0.00015;
double DerivativeMultiplier = 10.0;
double OverallMultiplier = 1.0;



// tolerances for Overseer
double OverseerToleranceUpper = 2.0;
double OverseerToleranceLower = -2.0;

// the LOWER BOUNDS for the driverControl's Controller
double ControllerTolerance = 0.02;


// pistons
vex::triport::port DescorePistonPort = primaryBrain.ThreeWirePort.C;
vex::triport::port RubberBandPistonPort = primaryBrain.ThreeWirePort.B;
vex::triport::port IntakePistonPort = primaryBrain.ThreeWirePort.A;
pneumatics DescorePiston(DescorePistonPort);
pneumatics RubberBandPiston(RubberBandPistonPort);
pneumatics IntakePiston(IntakePistonPort);

// define buttons and callable functions for pistons
auto DescorePistonButton = primaryController.ButtonA;
auto RubberBandPistonButton = primaryController.ButtonX;
auto IntakePistonButton = primaryController.ButtonB;

auto triggerDescorePiston = []() { trigger(DescorePiston); };
auto triggerRubberBandPiston = []() { trigger(RubberBandPiston); };
auto triggerIntakePiston = []() { trigger(IntakePiston); };


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
