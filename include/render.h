// render.h
// Copyright (C) 2026 cocoazawa
//
// Written by Peter G.

#pragma once
#include <vex.h>
#include <config.h>
#include "report.h"

using namespace vex;

void log(int lineNumber, const char* value, ...) {
    vex::brain::lcd screenRef = primaryBrain.Screen;
}


void refreshCompetitionStatus() {
    int count = 0;

    vex::brain::lcd screenRef = primaryBrain.Screen;

    Timekeeper timekeeper(TimekeepingType::MinTime, 0.0, 100.0);
    Reporter reporter;

    // printing title banner
    screenRef.setOrigin(0, 0);

    if (aozoraTeamColor == red) {
        screenRef.setPenColor("#D6162E");
        screenRef.setFillColor("#D6162E");
        screenRef.drawRectangle(0, 0, 500, 40);
    } else if (aozoraTeamColor == blue) {
        screenRef.setPenColor("#0068EE");
        screenRef.setFillColor("#0068EE");
        screenRef.drawRectangle(0, 0, 500, 40);
    }
    
    screenRef.setPenColor("#FFFFFF");
    screenRef.setFont(mono30);
    screenRef.printAt(20, 30, false, "76371B [St. Mary's]");
    
    screenRef.setFont(mono15);
    if (aozoraExperimental) {
        screenRef.printAt(320, 20, false, "AOZORA^ex");
    } else {
        screenRef.printAt(320, 20, false, "AOZORA ");
    }
    screenRef.printAt(320, 30, false, "Peter W.G.");
    
    screenRef.setFillColor(color(transparent));

    // actual logging and other detailed information
    while (true) {
        timekeeper.start();
        
        screenRef.setFont(mono15);
        reporter.UpdateReport();

        screenRef.setOrigin(0, 60);
        
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
        screenRef.print("        Inertial Sensor: ");
        if (reporter.report.inertialConnected && reporter.report.inertialCalibrating) {
            screenRef.print("Calibrating...");
        } else if (reporter.report.inertialConnected && !reporter.report.inertialCalibrating) {
            screenRef.print("Performing");
        } else {
            screenRef.print("Disconnected");
        }
        screenRef.print("                    ");

        screenRef.setCursor(4, 1);
        screenRef.print("       Battery Readings: ");
        screenRef.print("%.0f%%, %.0f°C, %.3f amps, %.2f volts.    ", reporter.report.batteryPercentage, reporter.report.batteryTemperature, reporter.report.batteryCurrent, reporter.report.batteryVoltage);
        screenRef.print("                    ");

        screenRef.setCursor(5, 1);
        screenRef.print("                 Motors: ");
        if (reporter.report.allMotorsConnected) {
            screenRef.print("Performing (8)");
        } else {
            screenRef.print("Connection issues (?)");
        }
        screenRef.print("                    ");

        vex::this_thread::sleep_for(timekeeper.ending());
    }
}

