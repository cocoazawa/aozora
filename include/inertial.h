// inertial.h
// 
// Copyright (C) 2025 cocoazawa
// For `aozora`

#pragma once
#include "vex.h"

static const int InertialDriftCalibrationCycles = 100;


enum class InertialOrientation {
    FacingForward = 0,
    FacingRight = 1,
    FacingBackwards = 2,
    FacingLeft = 3
};

class InertialWrapper {
    private:
        vex::inertial &linkedInertial;
        double corHorizontalOffset;
        double corVerticalOffset;
        InertialOrientation corBearing;

        bool driftCalibrated = false;
        
        double driftXAxis = 0.0;
        double driftYAxis = 0.0;
        double driftZAxis = 0.0;

    public:
        bool isCalibrated() const {
            return driftCalibrated;
        }

        bool drift(vex::axisType requestAxis) const {
            switch (requestAxis) {
                case vex::axisType::xaxis:
                    return driftXAxis;
                    break;
                case vex::axisType::yaxis:
                    return driftYAxis;
                    break;
                case vex::axisType::zaxis:
                    return driftZAxis;
                    break;
            }
        }


        /// @brief Constructor for the InertialWrapper.
        /// @param linkingInertial The inertial sensor to link to.
        /// @param linkingCorHorizontalOffset The offset from the center of rotation (CoR or "Cor") that the sensor is at horizontally.
        /// @param linkingCorVerticalOffset The offset from the center of rotation (CoR or "Cor") that the sensor is at vertically.
        /// @param linkingCorBearing The offset from the center of rotation (CoR or "Cor") that the sensor is at in terms of it's bearing clockwise in standard bearing format.
        InertialWrapper(vex::inertial &linkingInertial, double linkingCorHorizontalOffset, double linkingCorVerticalOffset, InertialOrientation linkingCorBearing): linkedInertial(linkingInertial) {
            corHorizontalOffset = linkingCorHorizontalOffset;
            corVerticalOffset = linkingCorVerticalOffset;
            corBearing = linkingCorBearing;
        };

        /// @brief A blocking calibration function for the linked inertial sensor.

        void calibrate() {
            if (!linkedInertial.installed()) return;
            
            printf("\nInertial Sensor is connected. Calibrating...\n");
            linkedInertial.calibrate();
            
            while (linkedInertial.isCalibrating()) vex::wait(5, vex::msec);
            printf("Calibrated (1)!\n");
            
            // X-Axis
            double totalSummationXDrift = 0;
            for (int iterations = 0; iterations < InertialDriftCalibrationCycles; iterations++) {
                double xAxisAcceleration = linkedInertial.acceleration(vex::xaxis) * 9.80665;
                totalSummationXDrift += xAxisAcceleration;
                vex::wait(5, vex::msec);
            }
            driftXAxis = totalSummationXDrift / InertialDriftCalibrationCycles;
            printf("Calibrated (2)!\n");
            
            // Y-Axis
            double totalSummationYDrift = 0;
            for (int iterations = 0; iterations < InertialDriftCalibrationCycles; iterations++) {
                double yAxisAcceleration = linkedInertial.acceleration(vex::yaxis) * 9.80665;
                totalSummationYDrift += yAxisAcceleration;
                vex::wait(5, vex::msec);
            }
            driftYAxis = totalSummationYDrift / InertialDriftCalibrationCycles;
            printf("Calibrated (3)!\n");
            
            // Z-Axis
            double totalSummationZDrift = 0;
            for (int iterations = 0; iterations < InertialDriftCalibrationCycles; iterations++) {
                double zAxisAcceleration = linkedInertial.acceleration(vex::zaxis) * 9.80665;
                totalSummationZDrift += zAxisAcceleration;
                vex::wait(5, vex::msec);
            }
            driftZAxis = totalSummationZDrift / InertialDriftCalibrationCycles;
            printf("Calibrated (4)!\n");

            // Calibrate heading
            linkedInertial.resetHeading();


            driftCalibrated = true;
        }

        InertialOrientation bearing() const {
            return corBearing;
        }
};


