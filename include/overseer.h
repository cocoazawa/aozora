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

        double Setpoint = 0;

        double rawCurrentError = 0;
        double rawSummedError = 0;
        double rawRateBasedError = 0;

        double CurrentError = 0;
        double SummedError = 0;
        double RateBasedError = 0;

        int Cycles = 0;

        Timekeeper timekeeperData = Timekeeper(TimekeepingType::Stopwatch);




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

        /// @brief derives total displacement in millimeters for the active side
        /// @return displacement in millimeters
        double deriveDisplacement() {
            double totalRotationalDisplacement = 0.0;
    
            // NOTE: we use revolutions for calculations, not degrees.
            switch(ActiveSide) {
                case MotorGroupSide::Left: {
                    totalRotationalDisplacement = deriveCubicAverage(MotorLeftFront.position(vex::rotationUnits::rev), MotorLeftMid.position(vex::rotationUnits::rev), MotorLeftBack.position(vex::rotationUnits::rev));
                    break;
                }
                case MotorGroupSide::Right: {
                    totalRotationalDisplacement = deriveCubicAverage(MotorRightFront.position(vex::rotationUnits::rev), MotorRightMid.position(vex::rotationUnits::rev), MotorRightBack.position(vex::rotationUnits::rev));
                    break;
                }
                default: { break; }
            }
            
            // average multiplied by the gear ratio to get actual travel per cent and
            return(totalRotationalDisplacement * GearRatio * TravelDistance);
        }




    public:
        /// @brief constructor for Datacenter with active side
        /// @param side active side
        OverseerDatacenter(MotorGroupSide side) {
            this->ActiveSide = side;
        }
        
        /// @brief gets the current setpoint
        /// @return setpoint (mm)
        double getSetpoint() {
            return this->Setpoint;
        }
        /// @brief sets the setpoint
        void setSetpoint(double newSetpoint) {
            Setpoint = newSetpoint;
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




        /// @brief resets the timekeeper
        void resetTimekeeper() {
            timekeeperData.storeStart(std::chrono::system_clock::now());
        }

        /// @brief resets the motor encoder positions
        void resetDrivetrainDatascope() {
            switch (ActiveSide) {
                case Left:
                    MotorLeftFront.resetPosition();
                    MotorLeftMid.resetPosition();
                    MotorLeftBack.resetPosition();
                    break;
                case Right:
                    MotorRightFront.resetPosition();
                    MotorRightMid.resetPosition();
                    MotorRightBack.resetPosition();
                    break;
            }
        }

        /// @brief force stops all motors on the drivetrain (regardless of active side, stops all motors)
        static void stopDrivetrain() {
            MotorLeftFront.stop();
            MotorLeftMid.stop();
            MotorLeftBack.stop();

            MotorRightFront.stop();
            MotorRightMid.stop();
            MotorRightBack.stop();
        }


        double positionLeftMaximum = 0;
        double positionRightMaximum = 0;

        void max(double* point, double data) {
            if(*point < data) {
                *point = data;
            }
        }




        /// @brief derives the command based on the PID control loop
        /// @return returns the command to be issued to the motors
        double deriveCommand() {
            Cycles++;
            double errorValue = getError();
    
            rawCurrentError = errorValue;
            rawSummedError += errorValue;
            
            auto elapsedTime = timekeeperData.ending();
            timekeeperData.destoreStart();
            timekeeperData.start();

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
            

            printf("%f;", command);
            printf("%f/", getError());

            primaryBrain.Screen.setOrigin(0, 0);
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
            primaryBrain.Screen.setCursor(6, 1);
            primaryBrain.Screen.print("                   MotorLeft POSIT: ");
            max(&positionLeftMaximum, MotorLeftGroup.position(rev));
            primaryBrain.Screen.print("%f", positionLeftMaximum);
            primaryBrain.Screen.print("                    ");
            primaryBrain.Screen.setCursor(7, 1);
            primaryBrain.Screen.print("                  MotorRight POSIT: ");
            max(&positionRightMaximum, MotorRightGroup.position(rev));
            primaryBrain.Screen.print("%f", positionRightMaximum);
            primaryBrain.Screen.print("                    ");
            primaryBrain.Screen.setCursor(8, 1);
            primaryBrain.Screen.print("               STOP COMMAND ISSUED: ");
            primaryBrain.Screen.print("FALSE");
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
        OverseerDatacenter datacenterLeft = OverseerDatacenter(Left);
        OverseerDatacenter datacenterRight = OverseerDatacenter(Right);

        // central timekeeper
        Timekeeper timekeeperCentral = Timekeeper(TimekeepingType::Stopwatch);


        /// @brief evaluates if the stop conditions have been reached for this datacenter
        /// @return a boolean value of true if the autonomous control should halt immediately or not
        bool evaluateStopCondition(OverseerDatacenter* datacenterGeneric) {
            bool stopping = false;
            
            // error is within boundaries
            if ((*datacenterGeneric).getError() < OverseerToleranceUpper && (*datacenterGeneric).getError() > OverseerToleranceLower) { stopping = true; }
            
            // emergency stop is being issued
            if (primaryController.ButtonDown.pressing() && primaryController.ButtonUp.pressing()) { stopping = true; }

            return stopping;
        }


        /// @brief runs the control loop until stop is issued
        void startControl() {
            printf("RESET\n");
            // reset the drivetrain's position values
            datacenterLeft.resetDrivetrainDatascope();
            datacenterRight.resetDrivetrainDatascope();


            // remain in control until a stop has been issued
            while (stopIssue != true) {

                double leftCommand = datacenterLeft.deriveCommand();
                MotorLeftGroup.spin(forward, leftCommand, vex::velocityUnits::rpm);
                
                double rightCommand = datacenterRight.deriveCommand();
                MotorRightGroup.spin(forward, rightCommand, vex::velocityUnits::rpm);


                // issue a stop when the error is below the threshold
                if (evaluateStopCondition(&datacenterLeft) || evaluateStopCondition(&datacenterRight)) {
                    primaryBrain.Screen.setCursor(8, 1);
                    primaryBrain.Screen.print("               STOP COMMAND ISSUED: ");
                    primaryBrain.Screen.print("TRUE");
                    primaryBrain.Screen.print("                    ");
                    issueStop();
                }
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
            OverseerDatacenter::stopDrivetrain();
        }
        
        /// @brief Runs the operation until the setpoint has been reached or time outs
        /// @param commandType The type of command to operate
        /// @param setPoint The set point in MILLIMETERS OR in DEGREES depending on the type of the command
        void operation(CommandType commandType, double setpoint) {
            switch (commandType) {
                case Propel:
                    datacenterLeft.setSetpoint(setpoint);
                    datacenterRight.setSetpoint(setpoint);
                    break;
                default:
                    break;
            }

            datacenterLeft.resetTimekeeper();
            datacenterRight.resetTimekeeper();

            startControl();

            return;
        }
};