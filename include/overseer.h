// overseer.h
// Copyright (C) 2025 Project Aozora Collaborators
// 
// Written by Peter G.

enum CommandType {
    Pivot,
    Drive
};



class OverseerBasis {
    private:
        // rotationalTravel in mm
        int rotationalTravel = 260;

        bool stopIssue;

    public:
        OverseerBasis() {
            stopIssue = false;
        }


        void threadableUpdate(OverseerBasis &classReference) {
            while (classReference.stopIssue != true) {
                requestCurrent
            }
        }

        void operation(CommandType commandType, double setPoint) {
            switch (commandType) {
                case CommandType::Pivot:
                    
                


                    break;
                case CommandType::Drive:
                    


                    break;
                default:break;
            }
        }
};




