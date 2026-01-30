// overseer.h
// 
// Copyright (C) 2026 cocoazawa
// For `aozora`

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


struct CoordinateOffsetDeltas {
    double horizontalOffsetDelta;
    double verticalOffsetDelta;
};


/// @brief This class, the OverseerDataAggregator has one job: it's to collect data from any possible sensors (except for the motor encoders) to generate an estimate of the current position through physics alone, and from sensor data.
class OverseerDataAggregator {
    private:
        double previousVerticalOffset = 0;
        double previousHeading = 0;
        

        double estimatedVerticalOffset = 0;
        double estimatedHeading = 0;


        Timekeeper timekeeperAggregator = Timekeeper(TimekeepingType::MinTime, 0.1, 10.0);
        Timekeeper timekeeperAggregatorMeasurementTime = Timekeeper(TimekeepingType::Stopwatch);
        bool measurementTimeStarted = false;

        OverseerDataAggregator() {
            printf("[OVERSEER] OverseerDataAggregator instance created.\n");
        }
        
        OverseerDataAggregator(OverseerDataAggregator const&);
        void operator=(OverseerDataAggregator const&);

        // [INERTIAL] Obtain the acceleration for the sensor based on orientation and drift

        CoordinateOffsetDeltas currentInertialAcceleration(vex::inertial &inertialObject, InertialWrapper &inertialWrapper) {
            CoordinateOffsetDeltas deltas;

            // derive raw values in SI units
            double xAcceleration = inertialObject.acceleration(xaxis) * 9.80665;
            double yAcceleration = inertialObject.acceleration(yaxis) * 9.80665;
            
            // factor in drift
            xAcceleration -= inertialWrapper.drift(xaxis);
            yAcceleration -= inertialWrapper.drift(yaxis);

            // change to appropriate relative (horizontal, vertical) and factor in
            switch (inertialWrapper.bearing()) {
                case InertialOrientation::FacingForward:
                    deltas.horizontalOffsetDelta = yAcceleration;
                    deltas.verticalOffsetDelta = xAcceleration;
                    break;
                case InertialOrientation::FacingRight:
                    deltas.horizontalOffsetDelta = xAcceleration;
                    deltas.verticalOffsetDelta = -1 * yAcceleration;
                    break;
                case InertialOrientation::FacingBackwards:
                    deltas.horizontalOffsetDelta = -1 * yAcceleration;
                    deltas.verticalOffsetDelta = -1 * xAcceleration;
                    break;
                case InertialOrientation::FacingLeft:
                    deltas.horizontalOffsetDelta = -1 * xAcceleration;
                    deltas.verticalOffsetDelta = yAcceleration;
                    break;
            }

            return deltas;
        }
        


        // [DRIVETRAIN] angular displacement and eventual displacement derivation

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
        double deriveDisplacement(MotorGroupSide forActiveSide) {
            double totalRotationalDisplacement = 0.0;

            // NOTE: we use revolutions for calculations here, not degrees.
            switch (forActiveSide) {
                case MotorGroupSide::Left: {
                    totalRotationalDisplacement = deriveCubicAverage(MotorLeftFront.position(vex::rotationUnits::rev), MotorLeftMid.position(vex::rotationUnits::rev), MotorLeftBack.position(vex::rotationUnits::rev));
                    break;
                }
                case MotorGroupSide::Right: {
                    totalRotationalDisplacement = deriveCubicAverage(MotorRightFront.position(vex::rotationUnits::rev), MotorRightMid.position(vex::rotationUnits::rev), MotorRightBack.position(vex::rotationUnits::rev));
                    break;
                }
                default: {
                    break;
                }
            }

            // average multiplied by the gear ratio to get actual travel per cent and
            return (totalRotationalDisplacement * GearRatio * TotalTravelDistance);
        }


        // [AGGREGATE]

        /// @brief Aggregates up-to-date information and updates the sensor-based estimate
        void reAggregate() {
            timekeeperAggregator.start();

            previousVerticalOffset = aggregatedVerticalOffset;
            previousHeading = aggregatedHeading;

            // NOTE: Inertial
            
            // i use... heading!
            double inertialBasedHeading = (primaryInertialObject.heading() + secondaryInertialObject.heading() + tertiaryInertialObject.heading()) / 3;


            // NOTE: Encoders
            // 68.5mm off
            CoordinateOffsetDeltas encoderBasedOffset;
            encoderBasedOffset.horizontalOffsetDelta = horizontalRotational.position(vex::rotationUnits::deg) * 159.5882;
            encoderBasedOffset.verticalOffsetDelta = horizontalRotational.position(vex::rotationUnits::deg) * 159.5882;

            double encoderBasedHeading = encoderBasedOffset.horizontalOffsetDelta / (2 * M_PI * HorizontalEncoderCorDistance);


            // NOTE: Drivetrain
            double leftDisplacement = deriveDisplacement(MotorGroupSide::Left);
            double rightDisplacement = deriveDisplacement(MotorGroupSide::Right);

            CoordinateOffsetDeltas drivetrainBasedOffset;
            double drivetrainBasedHeadingOffset = 0;

            drivetrainBasedOffset.verticalOffsetDelta = leftDisplacement;

            if (fabs(leftDisplacement - rightDisplacement) > 0.1) {
                double radiusOfArc = (leftDisplacement * TotalMachineDiameter) / (leftDisplacement - rightDisplacement);
                double arcAngle = (180 * leftDisplacement) / (M_PI * radiusOfArc);
    
                drivetrainBasedHeadingOffset = arcAngle;
    
                double hypotenuseOfArc = radiusOfArc - (TotalMachineDiameter / 2);
                drivetrainBasedOffset.verticalOffsetDelta = sin(arcAngle * M_PI / 180) * hypotenuseOfArc;
                
                double partialExcessHorizontal = (drivetrainBasedOffset.verticalOffsetDelta * sin((90 - arcAngle) * M_PI / 180)) / sin(arcAngle * M_PI / 180);
                drivetrainBasedOffset.horizontalOffsetDelta = hypotenuseOfArc - partialExcessHorizontal;
            }

            double aggregatedDrivetrainBasedVerticalOffset = aggregatedVerticalOffset;
            double aggregatedDrivetrainBasedHeading = aggregatedHeading;

            aggregatedDrivetrainBasedVerticalOffset += drivetrainBasedOffset.verticalOffsetDelta;
            aggregatedDrivetrainBasedHeading += drivetrainBasedHeadingOffset;

            aggregatedVerticalOffset = encoderBasedOffset.verticalOffsetDelta * relianceDisplacementEncoders + aggregatedDrivetrainBasedVerticalOffset * relianceDisplacementDrivetrain;
            aggregatedHeading = inertialBasedHeading * relianceHeadingInertial + encoderBasedHeading * relianceHeadingEncoders + aggregatedDrivetrainBasedHeading * relianceHeadingDrivetrain;
            
            auto waitTime = timekeeperAggregator.ending();
            wait(waitTime, msec);
        }



    public:
        double aggregatedVerticalOffset = 0;
        double aggregatedHeading = 0;


        /// @brief gets the singleton instance of the OverseerDataAggregator class
        static OverseerDataAggregator& getInstance() {
            static OverseerDataAggregator instance;
            return instance;
        }

        /// @brief resets the timekeeper
        void resetTimekeeper() {
            timekeeperAggregator.storeStart(std::chrono::system_clock::now());
            timekeeperAggregatorMeasurementTime.storeStart(std::chrono::system_clock::now());
        }

        static void runAggregation() {
            OverseerDataAggregator &localAggregatorInstance = OverseerDataAggregator::getInstance();

            localAggregatorInstance.resetTimekeeper();

            while(true) {
                localAggregatorInstance.reAggregate();
            }
        }
};


/// @brief Overseer utility data provisioning and processing class
class OverseerDataConnector {
    private:
        double Setpoint = 0;
        CommandType Commanding = Propel;

        double rawCurrentError = 0;
        double rawSummedError = 0;
        double rawRateBasedError = 0;

        double CurrentError = 0;
        double SummedError = 0;
        double RateBasedError = 0;

        double previousErrorValue = 0;
        double previousElapsedTime = 0;

        int Cycles = 0;

        Timekeeper timekeeperData = Timekeeper(TimekeepingType::Stopwatch);

        double positionLeftMaximum = 0;
        double positionRightMaximum = 0;


        double deriveDisplacement() {
            switch (Commanding) {
                case Propel:
                    return OverseerDataAggregator::getInstance().aggregatedVerticalOffset;
                    break;
                case Pivot:
                    return OverseerDataAggregator::getInstance().aggregatedHeading;
                    break;
            }
        }



    public:
        /// @brief constructor for Datacenter with active side
        /// @param side active side
        OverseerDataConnector() {}

        /// @brief gets the current setpoint
        /// @return setpoint (mm)
        double getSetpoint() {
            return this->Setpoint;
        }
        /// @brief gets the current command type
        /// @return command type (CommandType)
        double getCommandType() {
            return this->Commanding;
        }
        /// @brief sets the setpoint
        /// @param newSetpoint new setpoint (mm)
        void setSetpoint(double newSetpoint, CommandType commandType) {
            Setpoint = newSetpoint;
            Commanding = commandType;
        }

        /// @brief gets the relative error based on setpoint and actual covered
        /// @return relative error (mm)
        double getError() {
            return (getSetpoint() - deriveDisplacement());
        }
        double getDisplacement() {
            return (deriveDisplacement());
        }

        /// @brief resets the timekeeper
        void resetTimekeeper() {
            timekeeperData.storeStart(std::chrono::system_clock::now());
        }

        /// @brief resets the motor encoder positions
        void resetDrivetrainDatascope() {
            rawCurrentError = 0.0;
            rawSummedError = 0.0;
            rawRateBasedError = 0.0;
            CurrentError = 0.0;
            SummedError = 0.0;
            RateBasedError = 0.0;
            previousErrorValue = 0.0;
            previousElapsedTime = 0.0;

            MotorLeftFront.resetPosition();
            MotorLeftMid.resetPosition();
            MotorLeftBack.resetPosition();
            MotorRightFront.resetPosition();
            MotorRightMid.resetPosition();
            MotorRightBack.resetPosition();
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

        /// @brief derives the command based on the PID control loop
        /// @return returns the command to be issued to the motors
        double deriveCommand() {
            Cycles++;
            double errorValue = getError();
            if (previousErrorValue == 0) previousErrorValue = errorValue;

            rawCurrentError = errorValue;
            rawSummedError += errorValue;

            auto elapsedTime = timekeeperData.ending();
            timekeeperData.destoreStart();
            timekeeperData.start();

            if (elapsedTime == 0.0 && previousElapsedTime != 0.0) {
                rawRateBasedError = (previousErrorValue - errorValue) / previousElapsedTime;
            } else if (previousElapsedTime == 0) {
                rawRateBasedError = 0;
            } else {
                rawRateBasedError = (previousErrorValue - errorValue) / elapsedTime;
                previousElapsedTime = elapsedTime;
            }

            CurrentError = ProportionalMultiplier * rawCurrentError;
            SummedError = IntegralMultiplier * rawSummedError;
            RateBasedError = DerivativeMultiplier * rawRateBasedError;

            // actual derivation of command
            double command = OverallMultiplier * (CurrentError + SummedError + RateBasedError);

            // clamping command
            if (command > MaximumDirectRPM) command = MaximumDirectRPM;
            if (command < -MaximumDirectRPM) command = -MaximumDirectRPM;

            previousErrorValue = errorValue;

            primaryBrain.Screen.setFont(mono15);
            primaryBrain.Screen.setOrigin(0, 60);

            primaryBrain.Screen.setCursor(1 + 5, 1);
            primaryBrain.Screen.print("                            Cycles: ");
            primaryBrain.Screen.print(Cycles);
            primaryBrain.Screen.print("                    ");
            primaryBrain.Screen.setCursor(2 + 5, 1);
            primaryBrain.Screen.print("                           Command: ");
            primaryBrain.Screen.print(command);
            primaryBrain.Screen.print("                    ");
            primaryBrain.Screen.setCursor(3 + 5, 1);
            primaryBrain.Screen.print("                       Error Value: ");
            primaryBrain.Screen.print(getError());
            primaryBrain.Screen.print("                    ");
            primaryBrain.Screen.setCursor(4 + 5, 1);
            primaryBrain.Screen.print("        DirectDisplacement Reading: ");
            primaryBrain.Screen.print(deriveDisplacement());
            primaryBrain.Screen.print("                    ");
            primaryBrain.Screen.setCursor(5 + 5, 1);
            primaryBrain.Screen.print("                   MotorLeft POSIT: ");
            max(&positionLeftMaximum, MotorLeftGroup.position(rev));
            primaryBrain.Screen.print("%f", positionLeftMaximum);
            primaryBrain.Screen.print("                    ");
            primaryBrain.Screen.setCursor(6 + 5, 1);
            primaryBrain.Screen.print("                  MotorRight POSIT: ");
            max(&positionRightMaximum, MotorRightGroup.position(rev));
            primaryBrain.Screen.print("%f", positionRightMaximum);
            primaryBrain.Screen.print("                    ");
            primaryBrain.Screen.setCursor(7 + 5, 1);
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
        OverseerDataConnector dataConnector = OverseerDataConnector();


        /// @brief evaluates if the stop conditions have been reached for this datacenter
        /// @return a boolean value of true if the autonomous control should halt immediately or not
        bool evaluateStopCondition(OverseerDataConnector* datacenterGeneric) {
            bool stopping = false;
            
            // error is within boundaries
            if ((*datacenterGeneric).getError() < OverseerToleranceUpper && (*datacenterGeneric).getError() > OverseerToleranceLower) stopping = true;
            
            // emergency stop is being issued
            if (primaryController.ButtonDown.pressing() && primaryController.ButtonUp.pressing()) stopping = true;
            
            return stopping;
        }


        /// @brief runs the control loop until stop is issued
        void startControl() {
            // reset the drivetrain's position values
            dataConnector.resetDrivetrainDatascope();


            // remain in control until a stop has been issued
            while (stopIssue != true) {

                stopIssue = true;
                // // issue a stop when the error is below the threshold
                // if (evaluateStopCondition(&datacenterLeft) && evaluateStopCondition(&datacenterRight)) {
                //     primaryBrain.Screen.setFont(mono15);
                //     primaryBrain.Screen.setOrigin(0, 60);
                //     primaryBrain.Screen.setCursor(7 + 5, 1);
                //     primaryBrain.Screen.print("               STOP COMMAND ISSUED: ");
                //     primaryBrain.Screen.print("TRUE");
                //     primaryBrain.Screen.print("                    ");
                //     issueStop();
                //     return;
                // }

                // double leftCommand = 0.0;
                // leftCommand = datacenterLeft.deriveCommand();
                // MotorLeftGroup.spin(forward, (LeftSkewMultiplier * leftCommand), vex::velocityUnits::rpm);
                
                // double rightCommand = 0.0;
                // rightCommand = datacenterRight.deriveCommand();
                // MotorRightGroup.spin(forward, rightCommand, vex::velocityUnits::rpm);

                // // issue a stop when the error is below the threshold
                // if (evaluateStopCondition(&datacenterLeft) && evaluateStopCondition(&datacenterRight)) {
                //     primaryBrain.Screen.setFont(mono15);
                //     primaryBrain.Screen.setOrigin(0, 60);
                //     primaryBrain.Screen.setCursor(7 + 5, 1);
                //     primaryBrain.Screen.print("               STOP COMMAND ISSUED: ");
                //     primaryBrain.Screen.print("TRUE");
                //     primaryBrain.Screen.print("                    ");
                //     issueStop();
                //     return;
                // }

                // if(!evaluateStopCondition(&datacenterLeft)) {
                //     leftCommand = datacenterLeft.deriveCommand();
                //     MotorLeftGroup.spin(forward, leftCommand, vex::velocityUnits::rpm);
                // }
                
                // if(!evaluateStopCondition(&datacenterRight)) {
                //     rightCommand = datacenterRight.deriveCommand();
                //     MotorRightGroup.spin(forward, rightCommand, vex::velocityUnits::rpm);
                // }


                // printf("%f;", leftCommand);
                // printf("%f;", rightCommand);
                // printf("%f;", datacenterLeft.getDisplacement());
                // printf("%f/", datacenterRight.getDisplacement());
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

        /// @brief force stops all motors on the drivetrain (regardless of active side, stops all motors)
        static void stopDrivetrain() {
            MotorLeftFront.stop();
            MotorLeftMid.stop();
            MotorLeftBack.stop();

            MotorRightFront.stop();
            MotorRightMid.stop();
            MotorRightBack.stop();
        }

        /// @brief issue a stop to the overseer control function as well as stopping the drivetrain of any commands.
        void issueStop() {
            stopIssue = true;
            OverseerBasis::stopDrivetrain();
        }
        
        /// @brief Runs the operation until the setpoint has been reached or time outs
        /// @param commandType The type of command to operate
        /// @param setPoint The set point in MILLIMETERS OR in DEGREES depending on the type of the command
        void operation(CommandType commandType, double setpoint) {
            stopIssue = false;
            
            dataConnector.setSetpoint(setpoint, commandType);
            dataConnector.resetTimekeeper();

            startControl();

            return;
        }
};