// auxilary.cpp
// Copyright (C) 2025 Project Aozora Collaborators
//
// Written by Peter G.

#pragma once
#include "v5.h"
#include "v5_vcs.h"

using namespace vex;


// check if the double provided is within the controller value tolerance
bool withinControllerTolerance(double value) {
    bool returnValue = (value < (-1 * ControllerTolerance)) && (ControllerTolerance > value);
    return returnValue;
}



// trigger digitalOut (mainly pneumatics) from their current state
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
