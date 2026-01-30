// report.h
//
// Copyright (C) 2026 cocoazawa
// For `aozora`

#pragma once
#include "vex.h"
#include "config.h"

using namespace vex;

struct Report {
    bool primaryInertialConnected;
    bool primaryInertialCalibrating;
    bool secondaryInertialConnected;
    bool secondaryInertialCalibrating;
    bool tertiaryInertialConnected;
    bool tertiaryInertialCalibrating;
    float batteryPercentage;
    float batteryTemperature;
    float batteryCurrent;
    float batteryVoltage;
    bool allMotorsConnected;
};

class Reporter {
   public:
    Report report;

    void UpdateReport() {

        const float preeminentBatteryCapacity = primaryBrain.Battery.capacity(vex::percent);
        const float preeminentBatteryTemperature = primaryBrain.Battery.temperature(vex::celsius);
        const float preeminentBatteryCurrent = primaryBrain.Battery.current(vex::amp);
        const float preeminentBatteryVoltage = primaryBrain.Battery.voltage(vex::volt);

        const bool preeminentMotorLeftFrontInstalled = MotorLeftFront.installed();
        const bool preeminentMotorLeftMidInstalled = MotorLeftMid.installed();
        const bool preeminentMotorLeftBackInstalled = MotorLeftBack.installed();
        const bool preeminentMotorRightFrontInstalled = MotorRightFront.installed();
        const bool preeminentMotorRightMidInstalled = MotorRightMid.installed();
        const bool preeminentMotorRightBackInstalled = MotorRightBack.installed();
        const bool preeminentMotorSpinnerInstalled = MotorSpinner.installed();
        const bool preeminentMotorIntakeInstalled = MotorIntake.installed();

        const bool preeminentMotorsConnected =  (preeminentMotorLeftFrontInstalled &&
            preeminentMotorLeftMidInstalled &&
            preeminentMotorLeftBackInstalled &&
            preeminentMotorRightFrontInstalled &&
            preeminentMotorRightMidInstalled &&
            preeminentMotorRightBackInstalled &&
            preeminentMotorSpinnerInstalled &&
            preeminentMotorIntakeInstalled);
        

        this->report = {
            primaryInertialConnected: primaryInertialObject.installed(),
            primaryInertialCalibrating: primaryInertial.isCalibrated() ? false : true,
            secondaryInertialConnected: secondaryInertialObject.installed(),
            secondaryInertialCalibrating: secondaryInertial.isCalibrated() ? false : true,
            tertiaryInertialConnected: tertiaryInertialObject.installed(),
            tertiaryInertialCalibrating: tertiaryInertial.isCalibrated() ? false : true,
            batteryPercentage : preeminentBatteryCapacity,
            batteryTemperature : preeminentBatteryTemperature,
            batteryCurrent : preeminentBatteryCurrent,
            batteryVoltage : preeminentBatteryVoltage,
            allMotorsConnected : preeminentMotorsConnected,
        };
    }
};
