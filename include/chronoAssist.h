// overseer.h
// Copyright (C) 2025 Project Aozora Collaborators
// 
// Written by Peter G.

#include <vex.h>

class ChronoAssist {
    private:
        std::chrono::_V2::system_clock::time_point startPoint;
        std::chrono::_V2::system_clock::time_point endPoint;
        double minPoint;
    
    public:
        // constructor, sets minPoint
        ChronoAssist(double minPoint) {
            minPoint = minPoint;
        }

        // sets start time
        void start() {
            startPoint = std::chrono::system_clock::now();
        }
        
        // sets end time, calculates final stop time for the return
        int64_t ending() {
            endPoint = std::chrono::system_clock::now();
            std::chrono::duration<double> elapsedTime = endPoint - startPoint;
            
            auto staleDuration = minPoint - elapsedTime.count();
            if (staleDuration < 0) {
                return minPoint;
            }

            return staleDuration;
        }

};




