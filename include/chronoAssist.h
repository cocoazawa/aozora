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
        double setPoint;
        ChronoType chronoType;
    
    public:
        // constructor, sets minPoint
        ChronoAssist(ChronoType chronoType, double minPoint, double setPoint) {
            chronoType = chronoType;
            minPoint = minPoint;
            setPoint = setPoint;
        }

        // for supplying the start time for use in calculations
        ChronoAssist(ChronoType chronoType, std::chrono::_V2::system_clock::time_point startTimeSupplied) {
            chronoType = chronoType;
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
            printf("[  GET   ] ChronosAssist: Elapsed Time %d", elapsedTime.count());

            switch (this->chronoType) {
                case ChronoType::Stopwatch: {
                    return elapsedTime.count();
                    break;
                }

                case ChronoType::MinTime: {
                    double staleDuration = setPoint - elapsedTime.count();
                    if (staleDuration < minPoint) {
                        printf("[  GET   ] ChronosAssist: Returning minPoint of %d", minPoint);
                        printf("[  READ  ] ChronosAssist: staleDuration was %d", staleDuration);
                        return minPoint;
                    }

                    printf("[  GET   ] ChronosAssist: Returning staleDuration of %d and setPoint of %d", staleDuration, setPoint);
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
