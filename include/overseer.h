// overseer.h
// Copyright (C) 2025 Project Aozora Collaborators
// 
// Written by Peter G.

#pragma once
#include "math.h"
#include "config.h"
#include "chronoAssist.h"


enum CommandType {
    Pivot = 0,
    Propel = 1,
    Unissued = 2
};

enum MotorGroupSide {
    Left,
    Right
};





class OverseerDataProvisioning {
    private:
        CommandType CommandTypeActual = Unissued; 
        double SetPointActual = 0;

        double PRE_CurrentError = 0;
        double PRE_SummedError = 0;
        double PRE_RateBasedError = 0;

        double CurrentError = 0;
        double SummedError = 0;
        double RateBasedError = 0;

        double Cycles = 0.0;

        controller controllerReference = controller();

        ChronoAssist chronosLocal = ChronoAssist(ChronoType::Stopwatch, std::chrono::_V2::system_clock::now());

        // error calculation

        /// @brief gets the relative error based on setpoint and actual covered
        double getRelativeError() {
            return(getSetPointActual() - deriveDisplacement());
        }



        // angular displacement derivation
        double deriveCubicAverage(double doubleA, double doubleB, double doubleC) {
            double averageDoubles = doubleA + doubleB + doubleC;
            averageDoubles = averageDoubles / 3;
            return averageDoubles;
        }

        /// @brief gets the rotational displacement of a specified motor group.
        /// @param group specified motor group based on the `MotorGroupSide` enum
        /// @return returns the total revolutions of the motor group's side (averaged);
        double getGroupAngularDisplacement(MotorGroupSide group) {
            double average = 0.0;

            switch(group) {
                case MotorGroupSide::Left: {
                    average = deriveCubicAverage(MotorLeftFront.position(vex::rotationUnits::rev), MotorLeftMid.position(vex::rotationUnits::rev), MotorLeftBack.position(vex::rotationUnits::rev));
                    break;
                }
                case MotorGroupSide::Right: {
                    average = deriveCubicAverage(MotorRightFront.position(vex::rotationUnits::rev), MotorRightMid.position(vex::rotationUnits::rev), MotorRightBack.position(vex::rotationUnits::rev));
                    break;
                }
                default: { break; }
            }

            return average;
        }




        /// @brief converts the average from each side to the total distance using gear ratios and the wheel's travel
        /// @return the MILLIMETERS of the distance covered for Propel OR the ANGLE the robot is at for Pivot
        double deriveDisplacement() {
            double displacement = 0.0;

            // get average
            double totalRotationalDisplacement = getGroupAngularDisplacement(MotorGroupSide::Left) + getGroupAngularDisplacement(MotorGroupSide::Right);
            totalRotationalDisplacement = totalRotationalDisplacement / 2;

            // average multiplied by the gear ratio to get actual travel per cent and
            double totalLinearDisplacement = totalRotationalDisplacement * GearRatio * TravelDistance;

            
            switch (CommandTypeActual) {
                case CommandType::Pivot: {
                    double percentCircumferenceCovered = totalLinearDisplacement / TotalCircumference * 360.0;
                    displacement = percentCircumferenceCovered;
                    break;
                }
                
                case CommandType::Propel: {
                    displacement = totalLinearDisplacement;
                    break;
                }

                default: { break; }
            }

            return displacement;
        }


        
    public:
        OverseerDataProvisioning() {}

        void provision(CommandType basedOnCommandType, double basedOnSetPoint) {
            this->CommandTypeActual = basedOnCommandType;
            this->SetPointActual = basedOnSetPoint;
        }

        void startChronosStorage() {
            chronosLocal.storeStart(std::chrono::_V2::system_clock::now());
        }

        // a bunch of protected setters and getters
        void setSetPointActual(double setPoint) {
            // printf("[  SETP  ] Changing SetPoint from %d to %d\n", this->SetPointActual, setPoint);
            this->SetPointActual = setPoint;
        }
        double getSetPointActual() {
            // printf("[  SETP  ] SetPoint is %d\n", this->SetPointActual);
            return this->SetPointActual;
        }
        CommandType getCommandType() {
            return this->CommandTypeActual;
        }
        double getReadableError() {
            return getRelativeError();
        }

        /// @brief derives the command based on the PID control loop
        /// @return returns the command to be issued to the motors
        double deriveCommand() {
            double coveredValue = deriveDisplacement();
    
            if (CommandTypeActual == CommandType::Pivot) {
                coveredValue = coveredValue / 360 * TotalCircumference;
            }
            
            double errorValue = getSetPointActual() - coveredValue;
    
            PRE_CurrentError = errorValue;
            PRE_SummedError += errorValue;
            if (chronosLocal.ending() == 0) {
                PRE_RateBasedError = 0;
            } else {
                PRE_RateBasedError = errorValue / chronosLocal.ending();
            }
            
            CurrentError = ProportionalMultiplier * PRE_CurrentError;
            SummedError = IntegralMultiplier * PRE_SummedError;
            RateBasedError = DerivativeMultiplier * PRE_RateBasedError;

            // actual derivation of command
            double command = OverallMultiplier * (CurrentError + SummedError + RateBasedError);

            controllerReference.Screen.print(command);
            controllerReference.Screen.print(" AND ");
            controllerReference.Screen.print(errorValue);


            // clamping command
            if (command > MaximumDirectRPM) command = MaximumDirectRPM;
            if (command < -MaximumDirectRPM) command = -MaximumDirectRPM;

            return command;
        }
};






class OverseerBasis {
    private:
        bool stopIssue = false;

        OverseerDataProvisioning DataProvisioner;

        /// @brief runs the control loop until stop is issued
        void startControl() {
            DataProvisioner.startChronosStorage();
            ChronoAssist chronosBalancer(ChronoType::MinTime, 1.0, 5.0);

            while (stopIssue != true) {
                chronosBalancer.start();

                double command = DataProvisioner.deriveCommand();
    
                switch (DataProvisioner.getCommandType()) {
                    case CommandType::Pivot: {
                        if (DataProvisioner.getSetPointActual() > 0) {
                            MotorLeftGroup.spin(forward, command, vex::velocityUnits::rpm);
                            MotorRightGroup.spin(reverse, command, vex::velocityUnits::rpm);
                        } else {
                            MotorLeftGroup.spin(reverse, command, vex::velocityUnits::rpm);
                            MotorRightGroup.spin(forward, command, vex::velocityUnits::rpm);
                        }
                        break;
                    }
                    case CommandType::Propel: {
                        MotorLeftGroup.spin(forward, command, vex::velocityUnits::rpm);
                        MotorRightGroup.spin(forward, command, vex::velocityUnits::rpm);
                        break;
                    }
                    default: {
                        break;
                    }
                }

                auto endTime = chronosBalancer.ending();
                wait(endTime, msec);
                

                if (DataProvisioner.getReadableError() < OverseerToleranceUpper && DataProvisioner.getReadableError() > OverseerToleranceLower) {
                    stopIssue = true;
                }
            }
        }

    public:
        OverseerBasis() {
            // printf("[   GO   ] OverseerBasis instance has been created and is ready to accept commands.\n");
        }


        /// @brief Runs the operation until the setpoint has been reached or time outs
        /// @param commandType The type of command to operate
        /// @param setPoint The set point in MILLIMETERS OR in DEGREES depending on the type of the command
        void operation(CommandType commandType, double setPoint) {
            DataProvisioner.provision(commandType, setPoint);
            
            // printf("[  CMD   ] Command of command type %d (0 Pivot, 1 Propel, 2 N/A) issued with setpoint of the following: %d\n", (int) DataProvisioner.getCommandType(), DataProvisioner.getSetPointActual());
            
            startControl();
            return;
        }
};




