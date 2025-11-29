# aozora

Aozora or 青空, the word for "blue sky" in Japanese. Project Aozora aims to be a fully sufficient program for the control of the robot in the '26 V5RC Push Back Game Season ("Game Season").

Copyright (C) 2025 Project Aozora Collaborators. All rights reserved.  
The contents of this repository is hereby referred to as the "Code" and is specially licensed to VEX V5 Team 76371M/B/C (the "Team") for the Game Season. The Team is only allowed to distribute the code internally, within members of the Team. Members of the Team may also store this code locally on their machine with no intent to distribute outside of the Team, learn from the Code, or use the Code in competitions.

## About

> [!IMPORTANT]
> All distance measurements are in MILLIMETERS. NOT the SI unit of meters.

This project is the coding portion of the team for the main robot code, and implements developmental stages for the control system (in this case, Aozora) of the robot.

### Stages

1. Driver Control (DC) is fully sufficient. It supports all features being controlled using the controller.
2. On top of Stage 1, the Semi-Autonomous Control (SAC) is fully sufficient. This indicates that, during the Autonomous Period (AP), the robot is able to derive simple instructions for pivots and propelling the robot from the `config.h` or similar file.
3. On top of Stage 1, but in stead of Stage 2, Stage 3 aims to have the SAC running fully on complex instructions, including instructions derived from waypoints and other forms of non-direct instructions for the robot.
4. On top of Stage 1 and 3 only, there is an external platform for planning and simulating the robot's 15 second journey using a simulator.

### Current Development

Project Aozora is currently on Stage 2.

## Overseer Control Class

The Overseer control class includes methods to control the robot using rudimentary commands such as propelling and pivoting the robot on-spot. It takes commands in the form of a type and setpoints, performs arithmetics on the setpoint, and utilizes a PID control method to initiate commands to the robot.

```plain/txt

Command -> Derive Setpoint for Actuator -> Loop A START -> End

Loop A START -> Collect Data/Time and Derive Error -> Produce Command -> Give Command to Actuator -> Collect Data/Time -> RESTART (Unless STOP Command Issued)

```

## Project Aozora Collaborators

- cocoazawa (Peter W.G.)


