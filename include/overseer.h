// overseer.h
// Copyright (C) 2025 Project Aozora Collaborators
// 
// Written by Peter G.

#pragma once
#include "math.h"
#include "config.h"
#include "chronoAssist.h"

/// @brief type of Command for Class 2 control
enum CommandType {
    Pivot = 0,
    Propel = 1,
    Unissued = 2
};
/// @brief side of the motor group
enum MotorGroupSide {
    Left,
    Right
};




/// @brief Overseer utility data provisioning and processing class
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
                    return percentCircumferenceCovered;
                    break;
                }
                
                case CommandType::Propel: {
                    return totalLinearDisplacement;
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

        // chronos store time
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

        void resetDrivetrainDatascope() {
            MotorLeftFront.resetPosition();
            MotorLeftMid.resetPosition();
            MotorLeftBack.resetPosition();

            MotorRightFront.resetPosition();
            MotorRightMid.resetPosition();
            MotorRightBack.resetPosition();
        }
        static void stopDrivetrain() {
            MotorLeftFront.stop();
            MotorLeftMid.stop();
            MotorLeftBack.stop();

            MotorRightFront.stop();
            MotorRightMid.stop();
            MotorRightBack.stop();
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

            Brain.Screen.setCursor(1, 1);
            Brain.Screen.print("                            Cycles: ");
            Brain.Screen.print(Cycles);
            Brain.Screen.print("                    ");
            Brain.Screen.setCursor(2, 1);
            Brain.Screen.print("                           Command: ");
            Brain.Screen.print(command);
            Brain.Screen.print("                    ");
            Brain.Screen.setCursor(3, 1);
            Brain.Screen.print("                       Error Value: ");
            Brain.Screen.print(getReadableError());
            Brain.Screen.print("                    ");
            Brain.Screen.setCursor(4, 1);
            Brain.Screen.print("        DirectDisplacement Reading: ");
            Brain.Screen.print(deriveDisplacement());
            Brain.Screen.print("                    ");
            Brain.Screen.setCursor(5, 1);
            Brain.Screen.print("AngularDisplacement Reading (Left): ");
            Brain.Screen.print(getGroupAngularDisplacement(MotorGroupSide::Left));
            Brain.Screen.print("                    ");
            Brain.Screen.setCursor(6, 1);
            Brain.Screen.print("AngularDisplacement Reading (Rig.): ");
            Brain.Screen.print(getGroupAngularDisplacement(MotorGroupSide::Right));
            Brain.Screen.print("                    ");
            

            // clamping command
            if (command > MaximumDirectRPM) command = MaximumDirectRPM;
            if (command < -MaximumDirectRPM) command = -MaximumDirectRPM;

            return command;
        }
};




/// @brief Overseer main access class
class OverseerBasis {
    private:
        bool stopIssue = false;

        OverseerDataProvisioning DataProvisioner;


        /// @brief determines if the stop conditions have been reached
        /// @return if the autonomous control should halt immediately or not
        bool stopConditionReached() {
            bool stopping = false;
            
            // error is within boundaries
            if (DataProvisioner.getReadableError() < OverseerToleranceUpper && DataProvisioner.getReadableError() > OverseerToleranceLower) { stopping = true; }
            
            // emergency stop is being issued
            if (primaryController.ButtonDown.pressing() && primaryController.ButtonUp.pressing()) { stopping = true; }

            return stopping;
        }


        /// @brief runs the control loop until stop is issued
        void startControl() {
            // reset the drivetrain's position values
            DataProvisioner.resetDrivetrainDatascope();
            // only for keeping a minimum time of the loop
            ChronoAssist chronosBalancer(ChronoType::MinTime, 1.0, 5.0);

            // start the time before the loop starts
            DataProvisioner.startChronosStorage();

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

                    default: { break; }
                }


                // issue a stop when the error is below the threshold
                if (stopConditionReached()) {
                    issueStop();
                }
                
                // stop the timekeeper when the program is over
                auto endTime = chronosBalancer.ending();
                wait(endTime, msec);
            }
        }

        
        OverseerBasis() {
            printf("[OVERSEER] OverseerBasis instance created.\n");
        }
        
        OverseerBasis(OverseerBasis const&);
        void operator=(OverseerBasis const&);
        
    public:
        /// @brief gets the singleton instance of the OverseerBasis class
        static OverseerBasis& getInstance() {
            static OverseerBasis instance;
            return instance;
        }

        /// @brief issue a stop to the overseer control function as well as stopping the drivetrain of any commands.
        void issueStop() {
            stopIssue = true;
            OverseerDataProvisioning::stopDrivetrain();
        }
        
        /// @brief Runs the operation until the setpoint has been reached or time outs
        /// @param commandType The type of command to operate
        /// @param setPoint The set point in MILLIMETERS OR in DEGREES depending on the type of the command
        void operation(CommandType commandType, double setPoint) {
            DataProvisioner.provision(commandType, setPoint);
            
            startControl();

            return;
        }
};