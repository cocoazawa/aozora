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
    while(true) {
        primaryBrain.Screen.setFont(mono15);
        if (Competition.isAutonomous()) {
            primaryBrain.Screen.printAt(20 + adjustHorizontal, 205 + adjustVertical, true, "STAT_AUTONOMOUS      ");
        } else if (Competition.isDriverControl()) {
            primaryBrain.Screen.printAt(20 + adjustHorizontal, 205 + adjustVertical, true, "STAT_DRIVER_CONTROL  ");
        } else if (Competition.isFieldControl()) {
            primaryBrain.Screen.printAt(20 + adjustHorizontal, 205 + adjustVertical, true, "STAT_STANDBY         ");
        } else {
            primaryBrain.Screen.printAt(20 + adjustHorizontal, 205 + adjustVertical, true, "STAT_NOT_ENTERED     ");
        }
        
        // Brain.Screen.printAt(20 + adjustHorizontal, 225 + adjustVertical, true, "X %f, Y %f, Z %f", Tracker::positionAxis(xaxis), Tracker::positionAxis(yaxis), Tracker::positionAxis(zaxis));
        vex::this_thread::sleep_for(100);
    }
}


void refreshCompetitionStatus() {
    int count = 0;

    while (true) {
        // Evaluate the need to rerender and consume more resources
        

        // Proceed with rerender
        vex::brain::lcd screenRef = primaryBrain.Screen;

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
        
        screenRef.setFont(mono15);
        if (aozoraExperimental) {
            screenRef.printAt(20 + adjustHorizontal, 125 + adjustVertical, true, "Experimental version installed - Version %s.", aozoraVersion);
            screenRef.printAt(450, 230, true, "%d", count);
        } else {
            screenRef.printAt(20 + adjustHorizontal, 125 + adjustVertical, true, "Stable version installed - Version %s.", aozoraVersion);
        }

        if (shared.lastEvaluation.inertialConnected && shared.lastEvaluation.inertialCalibrating) {
            screenRef.printAt(20 + adjustHorizontal, 145 + adjustVertical, true, "Inertial sensor connected and calibrating...");
        } else if (shared.lastEvaluation.inertialConnected && !shared.lastEvaluation.inertialCalibrating) {
            screenRef.printAt(20 + adjustHorizontal, 145 + adjustVertical, true, "Inertial sensor connected and calibrated.   ");
        } else {
            screenRef.printAt(20 + adjustHorizontal, 145 + adjustVertical, true, "Inertial sensor disconnected.                ");
        }

        screenRef.printAt(20 + adjustHorizontal, 165 + adjustVertical, true, "Battery at %.0f%%, %.0f°C, %.3f amps, %.2f volts.    ", shared.lastEvaluation.batteryPercentage, shared.lastEvaluation.batteryTemperature, shared.lastEvaluation.batteryCurrent, shared.lastEvaluation.batteryVoltage);

        if (shared.lastEvaluation.allMotorsConnected) {
            screenRef.printAt(20 + adjustHorizontal, 185 + adjustVertical, true, "All specified motors (8) successfully connected.       ");
        } else {
            screenRef.printAt(20 + adjustHorizontal, 185 + adjustVertical, true, "Some or none of the specified motors (8) are connected.");
        }

        vex::this_thread::sleep_for(100);
    }

    return 0;
}