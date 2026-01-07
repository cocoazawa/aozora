// render.h
// Copyright (C) 2026 Project Aozora Collaborators
//
// Written by Peter G.

#pragma once
#include <vex.h>
#include <config.h>
#include "report.h"

using namespace vex;


void refreshCompetitionStatus() {
    int count = 0;

    vex::brain::lcd screenRef = primaryBrain.Screen;

    Timekeeper timekeeper(TimekeepingType::MinTime, 0.0, 100.0);

    while (true) {
        timekeeper.start();

        // printing title banner
        screenRef.setOrigin(0, 0);
        screenRef.setFillColor(color(transparent));
        screenRef.setPenColor("#FFFFFF");
        screenRef.setFont(mono40);

        if (aozoraExperimental) {
            screenRef.printAt(20 + adjustHorizontal, 53 + adjustVertical, true, "Project Aozora^ex");
        } else {
            screenRef.printAt(20 + adjustHorizontal, 53 + adjustVertical, true, "Project Aozora ");
        }

        if (aozoraTeamColor == red) {
            screenRef.setPenColor("#D6162E");
            screenRef.setFillColor("#D6162E");
            screenRef.drawRectangle(0, 68 + adjustVertical, 500, 40);
        } else if (aozoraTeamColor == blue) {
            screenRef.setPenColor("#0068EE");
            screenRef.setFillColor("#0068EE");
            screenRef.drawRectangle(0, 68, 500, 40);
        }
        
        screenRef.setPenColor("#FFFFFF");
        screenRef.setFont(mono40);
        screenRef.printAt(20 + adjustHorizontal, 100 + adjustVertical, true, "76371B [St. Mary's]");
        screenRef.setFillColor(color(transparent));
        

        // actual logging and other detailed information
        screenRef.setFont(mono15);
        Reporter::UpdateReport();

        screenRef.setOrigin(20 + adjustHorizontal, 125 + adjustVertical);
        screenRef.setCursor(1, 1);
        screenRef.print(" Running Environment (auto detect): ");
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
        screenRef.print("                 Installed Version: ");
        if (aozoraExperimental) {
            screenRef.print("^ex Version ");
        } else {
            screenRef.print("Stable Version ");
        }
        screenRef.print(aozoraVersion);
        screenRef.print("                    ");

        screenRef.setCursor(3, 1);
        screenRef.print("                   Inertial Sensor: ");
        if (Reporter::report.inertialConnected && Reporter::report.inertialCalibrating) {
            screenRef.print("Calibrating...");
        } else if (Reporter::report.inertialConnected && !Reporter::report.inertialCalibrating) {
            screenRef.print("Performing");
        } else {
            screenRef.print("Disconnected");
        }
        screenRef.print("                    ");

        screenRef.setCursor(4, 1);
        screenRef.print("                  Battery Readings: ");
        screenRef.print("%.0f%%, %.0f°C, %.3f amps, %.2f volts.    ", Reporter::report.batteryPercentage, Reporter::report.batteryTemperature, Reporter::report.batteryCurrent, Reporter::report.batteryVoltage);
        screenRef.print("                    ");

        screenRef.setCursor(5, 1);
        screenRef.print("                            Motors: ");
        if (Reporter::report.allMotorsConnected) {
            screenRef.print("Performing (8)");
        } else {
            screenRef.print("Connection issues (?)");
        }
        screenRef.print("                    ");

        vex::this_thread::sleep_for(timekeeper.ending());
    }
}