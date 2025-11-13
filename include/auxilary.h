// auxilary.cpp
// Copyright (C) 2025 Project Aozora Collaborators
//
// Written by Peter G.

#include <vex.h>

using namespace vex;

void trigger(pneumatics& piston) {
    switch (piston.value()) {
        case 1:
            piston.set(false);
            break;
        case 0:
            piston.set(true);
            break;
        default:
            break;
    }
}
