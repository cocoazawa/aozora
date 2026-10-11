// config.h
// 
// Copyright (C) 2025 cocoazawa
// For `aozora`

#pragma once
#include "vex.h"
#include "inertial.h"

using namespace vex;

// brain setup
brain primaryBrain;

// introduce competition objects
competition Competition;

// introduce controller object (moved to allow for uniform access to one reference)
controller primaryController = controller();



#define aozoraExperimental false

static const char* aozoraVersion = "0.23.0";
static const bool AUTONOMOUS_TEST_FLAG = true;



// Printing Values

#define aozoraTeamColor blue
// #define aozoraTeamColor blue

static const int adjustHorizontal = -10;
static const int adjustVertical = -15;

static bool Ready = false;


// Overseer Calculation Values

// maximum direct rpm output from the motor
static const double MaximumDirectRPM = 600;
static const double TotalMachineDiameter = 292.5;
static const double TotalTravelDistance = 260;
static const double TotalTrackingTravelDistance = 159.5882;
// ratio remapping from the direct output to the actual output (yes, this CAN be simplified; it SHOULD NOT BE SIMPLIFIED)
static const double GearRatio = 36.0 / 60.0;


// Gains Based Filter configurations
static const double relianceHeadingInertial = 0.8;
static const double relianceHeadingEncoders = 0.15;
static const double relianceHeadingDrivetrain = 0.05;

static const double relianceDisplacementEncoders = 0.0;
static const double relianceDisplacementDrivetrain = 1.0;



// Overseer
double LeftSkewMultiplier = 0.95;
double MaximumIntegralError = 2 * pow(10, 100);

/// @details Speed Last, Accuracy First (SLAF-00) Profile
// static const char* profileVersion = "SLAF-00";
// double ProportionalMultiplier = 1.0;
// double IntegralMultiplier = 0.001;
// double DerivativeMultiplier = 0.007;
// double OverallMultiplier = 0.1;

/// @details Speed Priority, Accuracy Non-controlled (SPAN-22) Profile
static const char* profileVersion = "SPAN-22";
double ProportionalMultiplier = 0.78;
double IntegralMultiplier = 0.00015;
double DerivativeMultiplier = 1000.0;
double OverallMultiplier = 1.0;



// tolerances for Overseer
double OverseerToleranceUpper = 0.2;
double OverseerToleranceLower = -0.2;

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

auto IntakeButton = primaryController.ButtonL1;
auto IntakeCounterspinButton = primaryController.ButtonR1;
auto SpinnerButton = primaryController.ButtonL2;
auto SpinnerCounterspinButton = primaryController.ButtonR2;


// rotation sensor
rotation HorizontalRotational = rotation(PORT20, false);
rotation VerticalRotational = rotation(PORT18, false);

static const double HorizontalEncoderCorDistance = 68.5; // mm


// gyroscope object
// introduced from aozora v0.16.1 for testing applications
inertial primaryInertialObject(PORT15);
inertial secondaryInertialObject(PORT14);
inertial tertiaryInertialObject(PORT13);

InertialWrapper primaryInertial = InertialWrapper(primaryInertialObject, 10.0, 10.0, InertialOrientation::FacingRight);
InertialWrapper secondaryInertial = InertialWrapper(secondaryInertialObject, 10.0, 10.0, InertialOrientation::FacingRight);
InertialWrapper tertiaryInertial = InertialWrapper(tertiaryInertialObject, 10.0, 10.0, InertialOrientation::FacingLeft);




