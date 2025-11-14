// auxilary.cpp
// Copyright (C) 2025 Project Aozora Collaborators
//
// Written by Peter G.

#pragma once
#include "v5.h"
#include "v5_vcs.h"

using namespace vex;

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
