#pragma once

#include "EZ-Template/api.hpp" // IWYU pragma: keep
#include "EZ-Template/piston.hpp" // IWYU pragma: keep
#include "api.h" // IWYU pragma: keep
#include "pros/optical.hpp" // IWYU pragma: keep

#include "clamp.hpp" // IWYU pragma: keep

extern Drive chassis;

// Your motors, sensors, etc. should go here.  Below are examples

inline pros::Motor Intake(1);
inline pros::Motor Arm1(2);
inline pros::Motor Arm2(3);
inline pros::MotorGroup LadyBrown({2,3});

inline ez::Piston Mogo('A');
inline ez::Piston Doinker('B');

inline pros::Rotation Rotation(4);
inline pros::Distance Distance(5);
inline pros::Optical Color(6);
inline pros::Distance DistanceSensor(1);

inline pros::adi::DigitalIn limit_switch('C');