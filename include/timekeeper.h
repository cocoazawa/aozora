// timekeeper.h
// 
// Copyright (C) 2025 cocoazawa
// For `aozora`

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
        /// @brief constructor with minimum time and set time (the time we want to reach) for differential calculation
        /// @param timekeepingType timekeeping type
        /// @param minPoint minimum time we should rest for
        /// @param setPoint the time we are trying to rest for. calculations will be based off of this
        Timekeeper(TimekeepingType givenTimekeepingType, double givenMinPoint, double givenSetPoint) {
            timekeepingType = givenTimekeepingType;
            minPoint = givenMinPoint;
            setPoint = givenSetPoint;
        }

        /// @brief constructor for supplying the start time for use in calculations
        /// @param timekeepingType timekeeping type
        /// @param startTimeSupplied start point (usually for stopwatches)
        Timekeeper(TimekeepingType givenTimekeepingType, std::chrono::_V2::system_clock::time_point startTimeSupplied) {
            timekeepingType = givenTimekeepingType;
            startPoint = startTimeSupplied;
        }

        /// @brief WARN: constructor for creating a timekeeper without time values
        Timekeeper(TimekeepingType givenTimekeepingType) {
            timekeepingType = givenTimekeepingType;
        }

        /// @brief gets the start time stored locally
        /// @return start time
        std::chrono::system_clock::time_point getStartPoint() {
            return startPoint;
        }
        
        /// @brief start the timer or stopwatch without an arbitrary time (using current time)
        void start() {
            if (storedStart == true) { return; }
            startPoint = std::chrono::system_clock::now();
        }

        /// @brief store an arbitrary time to use as the starting point
        /// @param timeToStore arbitrary time
        void storeStart(std::chrono::_V2::system_clock::time_point timeToStore) {
            storedStart = true;
            startPoint = std::chrono::system_clock::now();
        }

        /// @brief unlock the stored time variable
        void destoreStart() {
            storedStart = false;
        }
        
        /// @brief tells the timekeeper to lapse or stop the timing here
        /// @return final time to stop for (MinTime) OR the elapsed time since given start time (Stopwatch)
        double ending() {
            endPoint = std::chrono::system_clock::now();
            std::chrono::duration<double> elapsedTime = endPoint - startPoint;
            // printf("[  GET   ] ChronosAssist: Elapsed Time %f\n", elapsedTime.count());

            switch (this->timekeepingType) {
                case TimekeepingType::Stopwatch: {
                    return elapsedTime.count();
                    break;
                }

                case TimekeepingType::MinTime: {
                    double staleDuration = setPoint - elapsedTime.count();
                    if (staleDuration < minPoint) {
                        // printf("[  GET   ] ChronosAssist: Returning minPoint of %f\n", minPoint);
                        // printf("[  READ  ] ChronosAssist: staleDuration was %f\n", staleDuration);
                        return minPoint;
                    }

                    // printf("[  GET   ] ChronosAssist: Returning staleDuration of %f and setPoint of %f\n", staleDuration, setPoint);
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
