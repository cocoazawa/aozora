// auxilary.cpp
// Copyright (C) 2025 cocoazawa
//
// Written by Peter G.

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



void initializeVEX(void) {
    primaryInertial.calibrate();

    while (primaryInertial.isCalibrating()) {
        wait(5, msec);
    }
    
    MotorLeftFront.setBrake(vex::brakeType::coast);
    MotorLeftMid.setBrake(vex::brakeType::coast);
    MotorLeftBack.setBrake(vex::brakeType::coast);
    MotorRightFront.setBrake(vex::brakeType::coast);
    MotorRightMid.setBrake(vex::brakeType::coast);
    MotorRightBack.setBrake(vex::brakeType::coast);
    MotorSpinner.setBrake(vex::brakeType::coast);
    MotorIntake.setBrake(vex::brakeType::coast);
}

