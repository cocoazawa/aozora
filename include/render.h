// render.h
//
// Copyright (C) 2026 cocoazawa
// For `aozora`

#pragma once
#include <vex.h>
#include <config.h>
#include <report.h>
#include <string>
#include "overseer.h"

using namespace vex;


void logTitleBanner() {
    vex::brain::lcd screenRef = primaryBrain.Screen;

    if (aozoraTeamColor == red) {
        screenRef.setPenColor("#D6162E");
        screenRef.setFillColor("#D6162E");
    } else if (aozoraTeamColor == blue) {
        screenRef.setPenColor("#0068EE");
        screenRef.setFillColor("#0068EE");
    }

    screenRef.drawRectangle(0, 0, 500, 100);
    
    screenRef.setFont(prop60);
    screenRef.setPenColor("#FFFFFF");
    screenRef.printAt(10, 50, true, "76371M");
    
    screenRef.setFont(fontType::mono20);
    if (aozoraExperimental) {
        screenRef.printAt(10, 75, false, "aozora (Experimental Build)");
    } else {
        screenRef.printAt(10, 75, false, "aozora");
    }

    screenRef.setFont(mono15);
    screenRef.printAt(10, 90, false, "Built by @cocoazawa / St. Mary's International HS Robotics");

    screenRef.setOrigin(0, 0);
    screenRef.setPenColor("#FFFFFF");
    screenRef.setFillColor(color(transparent));
}


void refreshCompetitionStatus() {
    int count = 0;

    Timekeeper timekeeper(TimekeepingType::MinTime, 0.0, 100.0);
    Reporter reporter;

    vex::brain::lcd screenRef = primaryBrain.Screen;
    screenRef.clearScreen(black);
    screenRef.setOrigin(0, 0);

    logTitleBanner();

    screenRef.setOrigin(0, 120);
    screenRef.setPenColor("#FFFFFF");
    screenRef.setFillColor(color(transparent));
    screenRef.setFont(prop30);
    
    screenRef.setCursor(1,2);
    screenRef.print("Initializing...");

    screenRef.setFont(propM);
    screenRef.setCursor(1, 20);
    screenRef.print("Do NOT move or place the robot.");
    screenRef.setCursor(2, 20);
    screenRef.print("Rerun this program if moved.");

    while (!Ready) {
        wait(10, msec);
    }

    // printing title banner
    screenRef.clearScreen(black);
    screenRef.setOrigin(0, 0);

    logTitleBanner();

    // actual logging and other detailed information
    while (true) {
        timekeeper.start();
        reporter.UpdateReport();

        screenRef.setOrigin(0, 120);
        screenRef.setPenColor("#FFFFFF");
        screenRef.setFillColor(color(transparent));
        
        screenRef.setFont(mono15);
        screenRef.setCursor(1, 1);
        screenRef.print("            Environment: ");
        if (Competition.isAutonomous()) {
            screenRef.print("Autonomous");
        } else if (Competition.isDriverControl()) {
            screenRef.print("Driver Control");
        } else if (Competition.isFieldControl()) {
            screenRef.print("Standing By");
        } else {
            screenRef.print("Non-competition");
        }
        screenRef.print("                    ");


        screenRef.setCursor(2, 1);
        screenRef.print("      Installed Version: ");
        if (aozoraExperimental) {
            screenRef.print("^ex Version ");
        } else {
            screenRef.print("Stable Version ");
        }
        screenRef.print(aozoraVersion);
        screenRef.print("                    ");

        screenRef.setCursor(3, 1);
        screenRef.print("       Inertial Sensors: ");
        if ((reporter.report.primaryInertialConnected && reporter.report.primaryInertialCalibrating) || 
            (reporter.report.secondaryInertialConnected && reporter.report.secondaryInertialCalibrating) || 
            (reporter.report.tertiaryInertialConnected && reporter.report.tertiaryInertialCalibrating)) {
            screenRef.print("Calibrating...  ");
        } else if ((reporter.report.primaryInertialConnected && !reporter.report.primaryInertialCalibrating) && 
            (reporter.report.secondaryInertialConnected && !reporter.report.secondaryInertialCalibrating) && 
            (reporter.report.tertiaryInertialConnected && !reporter.report.tertiaryInertialCalibrating)) {
            screenRef.print("Performing  ");
        } else {
            screenRef.print("Disconnected  ");
        }
        screenRef.print("                    ");

        screenRef.setCursor(4, 1);
        screenRef.print("       Battery Readings: ");
        screenRef.print("%.0f%%, %.0f°C, %.3f A, %.2f V    ", reporter.report.batteryPercentage, reporter.report.batteryTemperature, reporter.report.batteryCurrent, reporter.report.batteryVoltage);
        screenRef.print("                    ");

        screenRef.setCursor(5, 1);
        screenRef.print("                 Motors: ");
        if (reporter.report.allMotorsConnected) {
            screenRef.print("Performing (8)");
        } else {
            screenRef.print("Connection issues (?)");
        }
        screenRef.print("                    ");
        
        
        
        
        
        // screenRef.setCursor(6, 1);
        // screenRef.print("EST POS: ");
        // // screenRef.print(OverseerDataAggregator::getInstance().PositionEstimate_InertialHorizontal);
        // // screenRef.print(", ");
        // // screenRef.print(OverseerDataAggregator::getInstance().PositionEstimate_InertialVertical);
        // // printf("%.2f, %.2f; ", OverseerDataAggregator::getInstance().PositionEstimate_InertialHorizontal, OverseerDataAggregator::getInstance().PositionEstimate_InertialVertical);
        // // // screenRef.print(" // ");
        // screenRef.print(OverseerDataAggregator::getInstance().PositionEstimate_DrivetrainHorizontal);
        // screenRef.print(", ");
        // screenRef.print(OverseerDataAggregator::getInstance().PositionEstimate_DrivetrainVertical);
        // screenRef.print("                                                                                                                        ");

        // screenRef.setCursor(7, 1);
        // screenRef.print("EST HEAD: ");
        // screenRef.print(OverseerDataAggregator::getInstance().PositionEstimate_InertialHeading);
        // screenRef.print(" // ");
        // screenRef.print(OverseerDataAggregator::getInstance().PositionEstimate_DrivetrainHeading);
        // screenRef.print("                                                                                                                                            ");

        screenRef.setOrigin(0, 0);
        vex::this_thread::sleep_for(timekeeper.ending());
    }
    
    screenRef.setOrigin(0, 0);
}

