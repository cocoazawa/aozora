// overseer.h
// Copyright (C) 2025 Project Aozora Collaborators
// 
// Written by Peter G.

#include <vex.h>

enum ChronoType {
    MinTime,
    Stopwatch
};

class ChronoAssist {
    private:
        std::chrono::_V2::system_clock::time_point startPoint;
        std::chrono::_V2::system_clock::time_point endPoint;
        double minPoint;
        ChronoType chronoType;
    
    public:
        // constructor, sets minPoint
        ChronoAssist(ChronoType type, double minPoint) {
            minPoint = minPoint;
        }
        
        // sets start time
        void start() {
            startPoint = std::chrono::system_clock::now();
        }
        
        // sets end time, calculates elapsed, and (if needed) final stop time for the return
        double ending() {
            endPoint = std::chrono::system_clock::now();
            std::chrono::duration<double> elapsedTime = endPoint - startPoint;

            switch (this->chronoType) {
                case ChronoType::Stopwatch:
                    return elapsedTime.count();

                case ChronoType::MinTime:
                    double staleDuration = minPoint - elapsedTime.count();
                    if (staleDuration < 0) {
                        return minPoint;
                    }

                    return staleDuration;

                default:
                    return 0.0;
            }
        }

};




