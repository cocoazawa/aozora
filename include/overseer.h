// overseer.h
// Copyright (C) 2025 Project Aozora Collaborators
// 
// Written by Peter G.

#pragma once
#include "math.h"
#include "config.h"
#include "timekeeper.h"

/// @brief type of Command for Class 2 control
enum CommandType {
    Pivot = 0,
    Propel = 1,
    Unissued = 2
};
/// @brief side of the motor group
enum MotorGroupSide {
    Left = 0,
    Right = 1
};



/// @brief Overseer utility data provisioning and processing class
class OverseerDatacenter {
    private:
        MotorGroupSide ActiveSide;
        Timekeeper* centralTimekeeper;

        double Setpoint = 0;

        double rawCurrentError = 0;
        double rawSummedError = 0;
        double rawRateBasedError = 0;

        double CurrentError = 0;
        double SummedError = 0;
        double RateBasedError = 0;

        int Cycles = 0;

        Timekeeper timekeeperData = Timekeeper(TimekeepingType::Stopwatch, std::chrono::_V2::system_clock::now());




        // angular displacement and eventual displacement derivation
        
        /// @brief gets the average of three doubles
        /// @param doubleA first double
        /// @param doubleB second double
        /// @param doubleC third double
        /// @return averaged double
        double deriveCubicAverage(double doubleA, double doubleB, double doubleC) {
            double averageDoubles = doubleA + doubleB + doubleC;
            averageDoubles = averageDoubles / 3;
            return averageDoubles;
        }

        /// @brief gets the rotational displacement of a specified motor group.
        /// @param group specified motor group based on the `MotorGroupSide` enum
        /// @return returns the total revolutions of the motor group's side (averaged);
        double getGroupAngularDisplacement() {
            double average = 0.0;

            // NOTE: we use revolutions for calculations, not degrees.
            switch(ActiveSide) {
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

        /// @brief derives total displacement in millimeters for the active side
        /// @return displacement in millimeters
        double deriveDisplacement() {
            // get average
            double totalRotationalDisplacement = getGroupAngularDisplacement();
            
            // average multiplied by the gear ratio to get actual travel per cent and
            double totalLinearDisplacement = totalRotationalDisplacement * GearRatio * TravelDistance;
            return totalLinearDisplacement;
        }




    public:
        /// @brief constructor for Datacenter with active side
        /// @param side active side
        OverseerDatacenter(MotorGroupSide side, Timekeeper* timekeeper) {
            this->ActiveSide = side;

            this->centralTimekeeper = timekeeper;
            (*centralTimekeeper).storeStart(std::chrono::system_clock::now());
        }
        
        /// @brief gets the current setpoint
        /// @return setpoint (mm)
        double getSetpoint() {
            return this->Setpoint;
        }

        /// @brief gets the active side
        /// @return active side (MotorGroupSide)
        MotorGroupSide getActiveSide() {
            return this->ActiveSide;
        }

        /// @brief gets the relative error based on setpoint and actual covered
        /// @return relative error (mm)
        double getError() {
            return(getSetpoint() - deriveDisplacement());
        }



        /// @brief resets the motor encoder positions
        void resetDrivetrainDatascope() {
            MotorLeftFront.resetPosition();
            MotorLeftMid.resetPosition();
            MotorLeftBack.resetPosition();

            MotorRightFront.resetPosition();
            MotorRightMid.resetPosition();
            MotorRightBack.resetPosition();
        }

        /// @brief force stops all motors on the drivetrain
        static void stopDrivetrain() {
            MotorLeftFront.stop();
            MotorLeftMid.stop();
            MotorLeftBack.stop();

            MotorRightFront.stop();
            MotorRightMid.stop();
            MotorRightBack.stop();
        }




        /// @brief derives the command based on the PID control loop
        /// @param elapsedTime the total elapsed time between the last command and this command
        /// @return returns the command to be issued to the motors
        double deriveCommand() {
            double errorValue = getError();
    
            rawCurrentError = errorValue;
            rawSummedError += errorValue;
            auto elapsedTime = (*centralTimekeeper).ending();
            if (elapsedTime == 0.0) {
                rawRateBasedError = 0;
            } else {
                rawRateBasedError = errorValue / elapsedTime;
            }
            
            CurrentError = ProportionalMultiplier * rawCurrentError;
            SummedError = IntegralMultiplier * rawSummedError;
            RateBasedError = DerivativeMultiplier * rawRateBasedError;

            // actual derivation of command
            double command = OverallMultiplier * (CurrentError + SummedError + RateBasedError);

            // clamping command
            if (command > MaximumDirectRPM) command = MaximumDirectRPM;
            if (command < -MaximumDirectRPM) command = -MaximumDirectRPM;
            (*centralTimekeeper).destoreStart();
            (*centralTimekeeper).start();
            

            printf("[OVERSEER] COMMAND VALUE %d", command);
            printf("[OVERSEER] ERROR VALUE %d", getError());

            primaryBrain.Screen.setCursor(1, 1);
            primaryBrain.Screen.print("                            Cycles: ");
            primaryBrain.Screen.print(Cycles);
            primaryBrain.Screen.print("                    ");
            primaryBrain.Screen.setCursor(2, 1);
            primaryBrain.Screen.print("                           Command: ");
            primaryBrain.Screen.print(command);
            primaryBrain.Screen.print("                    ");
            primaryBrain.Screen.setCursor(3, 1);
            primaryBrain.Screen.print("                       Error Value: ");
            primaryBrain.Screen.print(getError());
            primaryBrain.Screen.print("                    ");
            primaryBrain.Screen.setCursor(4, 1);
            primaryBrain.Screen.print("        DirectDisplacement Reading: ");
            primaryBrain.Screen.print(deriveDisplacement());
            primaryBrain.Screen.print("                    ");
            primaryBrain.Screen.setCursor(5, 1);
            primaryBrain.Screen.print("       AngularDisplacement Reading: ");
            primaryBrain.Screen.print(getGroupAngularDisplacement());
            primaryBrain.Screen.print("                    ");
            primaryBrain.Screen.setCursor(6, 1);
            primaryBrain.Screen.print("     Active Side (0 Left, 1 Right): ");
            primaryBrain.Screen.print("%i", ActiveSide);
            primaryBrain.Screen.print("                    ");

            return command;
        }
};




/// @brief Overseer main access class
class OverseerBasis {
    private:
        bool stopIssue = false;

        // central datacenter
        // TODO: Provision a datacenter with a pointer to the central timekeeper. You can make the timekeeper public.
        OverseerDatacenter datacenter;

        // central timekeeper
        Timekeeper timekeeperCentral = Timekeeper(TimekeepingType::Stopwatch);


        /// @brief determines if the stop conditions have been reached
        /// @return a boolean value of true if the autonomous control should halt immediately or not
        bool stopConditionReached() {
            bool stopping = false;
            
            // error is within boundaries
            if (datacenter.getError() < OverseerToleranceUpper && datacenter.getError() > OverseerToleranceLower) { stopping = true; }
            
            // emergency stop is being issued
            if (primaryController.ButtonDown.pressing() && primaryController.ButtonUp.pressing()) { stopping = true; }

            return stopping;
        }


        /// @brief runs the control loop until stop is issued
        void startControl() {
            // reset the drivetrain's position values
            datacenter.resetDrivetrainDatascope();

            // remain in control until a stop has been issued
            while (stopIssue != true) {

                // derive command value
                // double command = datacenter.deriveCommand();
    
                // access command type from provisioning and command accordingly
                // switch (datacenter.getCommandType()) {
                //     case CommandType::Pivot: {
                //         if (command > 0) {
                //             MotorLeftGroup.spin(forward, abs(command), vex::velocityUnits::rpm);
                //             MotorRightGroup.spin(reverse, abs(command), vex::velocityUnits::rpm);
                //         } else {
                //             MotorLeftGroup.spin(reverse, abs(command), vex::velocityUnits::rpm);
                //             MotorRightGroup.spin(forward, abs(command), vex::velocityUnits::rpm);
                //         }
                //         break;
                //     }

                //     case CommandType::Propel: {
                //         MotorLeftGroup.spin(forward, command, vex::velocityUnits::rpm);
                //         MotorRightGroup.spin(forward, command, vex::velocityUnits::rpm);
                //         break;
                //     }

                //     default: { break; }
                // }


                // // issue a stop when the error is below the threshold
                // if (stopConditionReached()) {
                //     issueStop();
                // }

                
                // // calculates endtime and waits for that amount of time
                // auto endTime = timekeeperControl.ending();
                // wait(endTime, msec);
            }
        }

        
        OverseerBasis() {
            datacenter = OverseerDatacenter(MotorGroupSide::Left, &timekeeperCentral);
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
            OverseerDatacenter::stopDrivetrain();
        }
        
        /// @brief Runs the operation until the setpoint has been reached or time outs
        /// @param commandType The type of command to operate
        /// @param setPoint The set point in MILLIMETERS OR in DEGREES depending on the type of the command
        void operation(CommandType commandType, double setPoint) {
            datacenter.provision(commandType, setPoint);
            
            startControl();

            return;
        }
};