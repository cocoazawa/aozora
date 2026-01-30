// auxilary.cpp
//
// Copyright (C) 2025 cocoazawa
// For `aozora`

#pragma once
#include "v5.h"
#include "v5_vcs.h"
#include "config.h"


using namespace vex;


/// @brief check if the double provided is within the controller value tolerance.
/// this may be outdated depending on the version of this repository.
bool withinControllerTolerance(double value) {
    bool returnValue = (value < (-1 * ControllerTolerance)) && (ControllerTolerance > value);
    return returnValue;
}



/// @brief trigger digitalOut (mainly pneumatics) from their current state
void trigger(vex::digital_out &peripheral) {
    switch (peripheral.value()) {
        case 1:
            peripheral.set(false);
            break;
        case 0:
            peripheral.set(true);
            break;
        default:
            break;
    }
}

void max(double* point, double data) {
    if (*point < data) {
        *point = data;
    }
}

void setDrivetrainBraking(vex::brakeType newBrakeType) {
    MotorLeftFront.setBrake(newBrakeType);
    MotorLeftMid.setBrake(newBrakeType);
    MotorLeftBack.setBrake(newBrakeType);
    MotorRightFront.setBrake(newBrakeType);
    MotorRightMid.setBrake(newBrakeType);
    MotorRightBack.setBrake(newBrakeType);
}


void initializeVEX(void) {
    primaryInertial.calibrate();
    secondaryInertial.calibrate();
    tertiaryInertial.calibrate();

    setDrivetrainBraking(vex::brakeType::coast);
    MotorSpinner.setBrake(vex::brakeType::coast);
    MotorIntake.setBrake(vex::brakeType::coast);
    
    MotorLeftFront.resetPosition();
    MotorLeftMid.resetPosition();
    MotorLeftBack.resetPosition();
    MotorRightFront.resetPosition();
    MotorRightMid.resetPosition();
    MotorRightBack.resetPosition();
    MotorSpinner.resetPosition();
    MotorIntake.resetPosition();

    Ready = true;
}

