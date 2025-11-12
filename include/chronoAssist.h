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
        ChronoAssist(double minPoint) {
            minPoint = minPoint;
        }

        void start() {
            startPoint = std::chrono::system_clock::now();
        }
        
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




