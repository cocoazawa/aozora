// main.cpp
//
// Copyright (C) 2025 cocoazawa
// For `aozora`

#include "config.h"
#include "vex.h"
#include "timekeeper.h"
#include "auxilary.h"
#include "overseer.h"
#include "render.h"
#include "control.h"

using namespace vex;

void preAutonomous(void) {
    printf("\n\nPre-Running Autonomous (1)...\n");
    
    // initialize interface
    // vex::thread interfaceThread = thread( refreshCompetitionStatus );
    
    // initialize vex components
    initializeVEX();
    
    primaryController.rumble("...");
}

void autonomous(void) {
    // start running aggregation
    printf("\n\nRunning Autonomous Phase\n");

    setDrivetrainBraking(vex::brakeType::hold);
    
    vex::thread aggregationThread = thread( OverseerDataAggregator::runAggregation );
    
    OverseerBasis &overseer = OverseerBasis::getInstance();
    
    // vex::thread interfaceThread = thread( spinIntake );
    overseer.operation(CommandType::Propel, 600.0);
    // overseer.operation(CommandType::Pivot, 148.2);
    // overseer.operation(CommandType::Propel, 1235.4756169184);
    // overseer.operation(CommandType::Pivot, -148.2);
    // overseer.operation(CommandType::Propel, 750.0);
    // aggregationThread.interrupt();
}



//
// Main will set up the competition functions and callbacks.
//
int main() {
    printf("\n\naozora -- version %s -- by cocoazawa for 76371M\n", aozoraVersion);
    printf("aozora -- profile %s\n\n\n", profileVersion);
    // Run the pre-autonomous function.
    preAutonomous();

    if (!Ready) wait(10, sec);


    // Run autonomous in testing mode when the flag is set to true for that build.
    if (AUTONOMOUS_TEST_FLAG) {
        printf("\n[WARN] aozora -- THIS BUILD RUNS AUTONOMOUS TESTING. PLEASE RUN WITH CAUTION.\n");
        autonomous();
        while (true) wait(100, msec);
    }


    // Set up callbacks for autonomous and driver control periods.
    // NOTE: `userControl` is from `control.h`
    Competition.autonomous(autonomous);
    Competition.drivercontrol(userControl);


    // Prevent main from exiting with an infinite loop.
    while (true) {
        wait(100, msec);
    }
}


