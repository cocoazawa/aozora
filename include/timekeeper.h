// timekeeper.h
// Copyright (C) 2025 Project Aozora Collaborators
// 
// Written by Peter G.

#pragma once
#include <stdio.h>
#include <chrono>

enum TimekeepingType {
    MinTime,
    Stopwatch
};

class Timekeeper {
    private:
        bool storedStart = false;

        std::chrono::_V2::system_clock::time_point startPoint;
        std::chrono::_V2::system_clock::time_point endPoint;

        double minPoint;
        double setPoint;
        TimekeepingType timekeepingType;
    
    public:
        // constructor, sets minPoint
        Timekeeper(TimekeepingType timekeepingType, double minPoint, double setPoint) {
            timekeepingType = timekeepingType;
            minPoint = minPoint;
            setPoint = setPoint;
        }

        // for supplying the start time for use in calculations
        Timekeeper(TimekeepingType timekeepingType, std::chrono::_V2::system_clock::time_point startTimeSupplied) {
            timekeepingType = timekeepingType;
            startPoint = startTimeSupplied;
        }

        std::chrono::system_clock::time_point getStartPoint() {
            return startPoint;
        }
        
        // sets start time
        void start() {
            if (storedStart == true) { return; }
            startPoint = std::chrono::system_clock::now();
        }

        void storeStart(std::chrono::_V2::system_clock::time_point timeToStore) {
            storedStart = true;
            startPoint = std::chrono::system_clock::now();
        }

        void destoreStart() {
            storedStart = false;
        }
        
        // sets end time, calculates elapsed, and (if needed) final stop time for the return
        double ending() {
            endPoint = std::chrono::system_clock::now();
            std::chrono::duration<double> elapsedTime = endPoint - startPoint;
            // printf("[  GET   ] ChronosAssist: Elapsed Time %d\n", elapsedTime.count());

            switch (this->timekeepingType) {
                case TimekeepingType::Stopwatch: {
                    return elapsedTime.count();
                    break;
                }

                case TimekeepingType::MinTime: {
                    double staleDuration = setPoint - elapsedTime.count();
                    if (staleDuration < minPoint) {
                        // printf("[  GET   ] ChronosAssist: Returning minPoint of %d\n", minPoint);
                        // printf("[  READ  ] ChronosAssist: staleDuration was %d\n", staleDuration);
                        return minPoint;
                    }

                    // printf("[  GET   ] ChronosAssist: Returning staleDuration of %d and setPoint of %d\n", staleDuration, setPoint);
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
