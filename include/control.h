// control.h
// 
// Copyright (C) 2025 cocoazawa
// For `aozora`

#include "timekeeper.h"
#include "auxilary.h"

void userControl(void) {
    printf("\n\nRunning Driver Controlled Phase\n");

    Timekeeper timekeeperLocal(TimekeepingType::MinTime, 1.0, 5.0);

    auto triggerDescorePiston = []() { trigger(DescorePiston); };
    auto triggerRubberBandPiston = []() { trigger(RubberBandPiston); };
    auto triggerIntakePiston = []() { trigger(IntakePiston); };
    
    // register 
    DescorePistonButton.pressed(triggerDescorePiston);
    RubberBandPistonButton.pressed(triggerRubberBandPiston);
    IntakePistonButton.pressed(triggerIntakePiston);

    bool motorIntakeStopped = false;
    bool motorSpinnerStopped = false;

    while (true) {
        timekeeperLocal.start();

        // Intake or Spinner buttons pressed respectively
        if (IntakeButton.pressing() || IntakeCounterspinButton.pressing()) {
            motorIntakeStopped = false;
            MotorIntake.spin(IntakeButton.pressing() ? forward : reverse, 600, rpm);
        } else {
            if (motorIntakeStopped != true) {
                MotorIntake.stop();
                motorIntakeStopped = true;
            }
        }

        if (SpinnerButton.pressing() || SpinnerCounterspinButton.pressing()) {
            motorSpinnerStopped = false;
            MotorSpinner.spin(SpinnerButton.pressing() ? forward : reverse, 600, rpm);
        } else {
            if (motorSpinnerStopped != true) {
                MotorSpinner.stop();
                motorSpinnerStopped = true;
            }
        }

        float velocityMultiplierTotal = 127;
        float sidewaysMultiplierTotal = 254;
        float velocityMultiplier = primaryController.Axis3.value() / velocityMultiplierTotal;
        float sidewaysMultiplier = primaryController.Axis1.value() / sidewaysMultiplierTotal;
        
        if (sidewaysMultiplier == 0 && velocityMultiplier == 0) {
            MotorLeftGroup.stop();
            MotorRightGroup.stop();
            
            auto endTime = timekeeperLocal.ending();
            wait(endTime, msec);
            continue;
        }
        
        long leftVelocity = MaximumDirectRPM * (velocityMultiplier + sidewaysMultiplier);
        long rightVelocity = MaximumDirectRPM * (velocityMultiplier - sidewaysMultiplier);

        MotorLeftGroup.spin(forward, leftVelocity, rpm);
        MotorRightGroup.spin(forward, rightVelocity, rpm);
        
        auto endTime = timekeeperLocal.ending();
        wait(endTime, msec);
    }
}


