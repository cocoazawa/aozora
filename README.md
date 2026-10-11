# aozora

Copyright (C) 2025 cocoazawa. All rights reserved.  

"おあぞら" means "blue sky" in Japanese. As a blue sky usually represents a vast amount of positive things, the codebase is named after this word.  

## Post-competition Note

This code does not work for the most part due to the highly faulty and unreliable nature of the VEX V5 Inertial Sensor. You can find details of this behaviour below. It was used partially for the Japan Nationals competition, as the driver control mechanisms work perfectly fine.

[What is the normal drift of an inertial sensor? - VEX V5 Technical Support / UNOFFICIAL Tech Support - VEX Forum](https://www.vexforum.com/t/what-is-the-normal-drift-of-an-inertial-sensor/139591/2)

[Is My Inertial Sensor Broken? - VEX V5 General Discussion - VEX Forum](https://www.vexforum.com/t/is-my-inertial-sensor-broken/87120)

[V5 Inertial Sensor Drifting - Programming Support - VEX Forum](https://www.vexforum.com/t/v5-inertial-sensor-drifting/101195/2)

[Intertial Sensor is not Accurate - Programming Support - VEX Forum](https://www.vexforum.com/t/intertial-sensor-is-not-accurate/120191)

[Inertial Sensor odd behavior - VEX V5 Technical Support - VEX Forum](https://www.vexforum.com/t/inertial-sensor-odd-behavior/131675)

The final integration of this codebase was executed within a 12-hour overnight window. without a CAD model, and under rapid-iteration. Because the actual robot was still being adjusted 20 minutes before the first match and without a "design freeze", the physical kinematics of the robot were non-deterministic during development. I designed this codebase to push the limits of standard high school robotics software for myself and for the team. Here it lies for reference.
