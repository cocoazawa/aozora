// overseer.h
// Copyright (C) 2025 Project Aozora Collaborators
// 
// Written by Peter G.

#pragma once
#include "math.h"
#include "config.h"
#include "chronoAssist.h"


enum CommandType {
    Pivot,
    Propel,
    Unissued
};

enum MotorGroup {
    Left,
    Right
};

class OverseerBasis {
    private:
        // rotationalTravel in mm
        bool stopIssue;

        double setPoint = 0;
        CommandType commandType = Unissued; 
        
        double PRE_CurrentError = 0;
        double PRE_SummedError = 0;
        double PRE_RateBasedError = 0;

        double CurrentError = 0;
        double SummedError = 0;
        double RateBasedError = 0;

        ChronoAssist chronosLocal = ChronoAssist(Stopwatch, 0.0);

        double getAverageRotation(double doubleA, double doubleB, double doubleC) {
            double averageDoubles = doubleA + doubleB + doubleC;
            averageDoubles = averageDoubles / 3;
            return averageDoubles;
        }


        // returns total revolutions as average for a certain side
        double getRotationalDisplacement(MotorGroup group) {
            double average;

            switch(group) {
                case MotorGroup::Left: {
                    average = getAverageRotation(
                        MotorLeftFront.position(vex::rotationUnits::rev),
                        MotorLeftMid.position(vex::rotationUnits::rev),
                        MotorLeftBack.position(vex::rotationUnits::rev)
                    );
                    break;
                }
                case MotorGroup::Right: {
                    average = getAverageRotation(
                        MotorRightFront.position(vex::rotationUnits::rev),
                        MotorRightMid.position(vex::rotationUnits::rev),
                        MotorRightBack.position(vex::rotationUnits::rev)
                    );
                    break;
                }
                default: { break; }
            }

            return average;
        }

        /// @brief converts the average from each side to the total distance using gear ratios and the wheel's travel
        /// @return the MILLIMETERS of the distance covered for Propel OR the ANGLE the robot is at for Pivot
        double getTotalCovered() {
            // get average
            double totalRotationalDisplacement = getRotationalDisplacement(MotorGroup::Left) + getRotationalDisplacement(MotorGroup::Right);
            totalRotationalDisplacement = totalRotationalDisplacement / 2;

            // average multiplied by the gear ratio to get actual travel per cent and
            double totalDistance = totalRotationalDisplacement * GearRatio * TravelDistance;

            switch (commandType) {
                case CommandType::Pivot: {
                    return totalDistance;
                    break;
                }

                case CommandType::Propel: {
                    double percentCircumferenceCovered = totalDistance / TotalCircumference;
                    return percentCircumferenceCovered * 360;
                    break;
                }

                default: {
                    return 0.0;
                    break;
                }
            }
        }

        double getRelativeError() {
            return(setPoint - getTotalCovered());
        }

    public:
        OverseerBasis() {
            stopIssue = false;
            printf("[  STOP  ] STOP command issued via set variable");
        }

        double deriveCommand() {
            double coveredValue = getTotalCovered();

            if (commandType == CommandType::Pivot) {
                coveredValue = coveredValue / 360 * TotalCircumference;
            }
            
            double errorValue = setPoint - coveredValue;

            PRE_CurrentError = errorValue;
            PRE_SummedError += errorValue;
            PRE_RateBasedError = errorValue / chronosLocal.ending();

            CurrentError = ProportionalMultiplier * PRE_CurrentError;
            SummedError = IntegralMultiplier * PRE_SummedError;
            RateBasedError = DerivativeMultiplier * PRE_RateBasedError;

            double command = (OverallMultiplier * (CurrentError + SummedError + RateBasedError));
            return command;
        }

        void runControl() {
            printf("[  RUN   ] Running control in a thread using the class reference until STOP command is issued.");
            while (stopIssue != true) {
                switch (commandType) {
                    case CommandType::Pivot: {
                        // FIXME: Add pivot.
                        break;
                    }
                    case CommandType::Propel: {
                        double command = deriveCommand();
                        MotorLeftGroup.spin(forward, command, vex::velocityUnits::rpm);
                        MotorRightGroup.spin(forward, command, vex::velocityUnits::rpm);

                        break;
                    }
                    default: {
                        break;
                    }
                }

                if (ToleranceLower <= getRelativeError() || getRelativeError() <= ToleranceUpper) {
                    stopIssue = true;
                }
            }
        }

        /// @brief Runs the operation until the setpoint has been reached or time outs
        /// @param commandType The type of command to operate
        /// @param setPoint The set point in MILLIMETERS OR in DEGREES depending on the type of the command
        void operation(CommandType commandType, double setPoint) {
            printf("[  CMD   ] Command of command type %s issued with setpoint of %f.", commandType, setPoint);
            this->setPoint = setPoint;
            this->commandType = commandType;

            switch (commandType) {
                case CommandType::Pivot: {
                    // FIXME: Add pivot.
                    break;
                }
                case CommandType::Propel: {
                    runControl();
                    break;
                }
                default: { break; }
            }

            return;
        }
};




