# Overseer Navigation Class

Copyright (C) 2025 Project Aozora Collaborators. All rights reserved.  

## About

Previously (in Project Sprint) called the Navigator class, the Overseer class overlooks all autonomous and programmatic movements for the robot. Any movements that the robot makes following a certain path or logic is made with the assistance of Overseer.

## Logic

The logic for the Overseer class is divided up into two actual classes to manage data calculation and handling in a more readable and easier to understand manner. The two classes are **OverseerBasis** and **OverseerDataProvisioning**. The two classes have two very distinct areas of control.

For **OverseerBasis**, this class handles any input from the autonomous routine or the skills routine. OverseerBasis is also where all the commands to the motors to spin at a given command is sent. OverseerBasis only has two exposed, public methods that can either command or emergency stop the bot. When OverseerBasis receives a command to either pivot or propel the bot, the class does two things. It stores (or provisions) the data in a shared OverseerDataProvisioning class, and then starts the control cycle.
