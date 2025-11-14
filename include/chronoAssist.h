// overseer.h
// Copyright (C) 2025 Project Aozora Collaborators
// 
// Written by Peter G.

#pragma once
#include <stdio.h>

enum ChronoType {
    MinTime,
    Stopwatch
};

class ChronoAssist {
    private:
        bool storedStart = false;

        std::chrono::_V2::system_clock::time_point startPoint;
        std::chrono::_V2::system_clock::time_point endPoint;

        double minPoint;
        ChronoType chronoType;
    
    public:
        // constructor, sets minPoint
        ChronoAssist(ChronoType chronoType, double minPoint) {
            minPoint = minPoint;
        }

        // for supplying the start time for use in calculations
        ChronoAssist(ChronoType chronoType, double minPoint, std::chrono::_V2::system_clock::time_point startTimeSupplied) {
            minPoint = minPoint;
            startPoint = startTimeSupplied;
        }
        
        // sets start time
        void start() {
            if (storedStart) { return; }
            startPoint = std::chrono::system_clock::now();
        }

        void storeStart(std::chrono::_V2::system_clock::time_point timeToStore) {
            startPoint = std::chrono::system_clock::now();
            storedStart = true;
        }

        void destoreStart() {
            storedStart = false;
        }
        
        std::chrono::system_clock::time_point getStartPoint() {
            return startPoint;
        }

        // sets end time, calculates elapsed, and (if needed) final stop time for the return
        double ending() {
            endPoint = std::chrono::system_clock::now();
            std::chrono::duration<double> elapsedTime = endPoint - startPoint;
            printf("[  GET   ] ChronosAssist: Elapsed Time %d", elapsedTime.count());

            switch (this->chronoType) {
                case ChronoType::Stopwatch: {
                    return elapsedTime.count();
                    break;
                }
                case ChronoType::MinTime: {
                    double staleDuration = minPoint - elapsedTime.count();
                    if (staleDuration < minPoint) {
                        printf("[  GET   ] ChronosAssist: Returning minPoint of %d", minPoint);
                        return minPoint;
                    }

                    printf("[  GET   ] ChronosAssist: Returning staleDuration of %d", staleDuration);

                    return staleDuration;
                    break;
                }
                default: {
                    return 0.0;
                    break;
                }
            }
        }
};
