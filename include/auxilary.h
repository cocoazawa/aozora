// auxilary.cpp
// Copyright (C) 2025 cocoazawa
//
// Written by Peter G.

#pragma once
#include "v5.h"
#include "v5_vcs.h"


using namespace vex;


/// @brief check if the double provided is within the controller value tolerance.
/// this may be outdated depending on the version of this repository.
bool withinControllerTolerance(double value) {
    bool returnValue = (value < (-1 * ControllerTolerance)) && (ControllerTolerance > value);
    return returnValue;
}



/// @brief trigger digitalOut (mainly pneumatics) from their current state
void trigger(vex::digital_out &peripheral) {
    switch (peripheral.value()) {
        case 1:
            peripheral.set(false);
            break;
        case 0:
            peripheral.set(true);
            break;
        default:
            break;
    }
}



/// @brief emergency stop using down and up buttons pressed together. also allows for thread-detached running.
// void EmergencyStopThread() {
//     OverseerBasis &overseerLocal = OverseerBasis::getInstance();
    
//     while(true) {
//         if (primaryController.ButtonDown.pressing() && primaryController.ButtonUp.pressing()) {
//             overseerLocal.issueStop();
//         }
//         vex::this_thread::sleep_for(10);
//     }
// }