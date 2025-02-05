#pragma once

#include "EZ-Template/api.hpp" // IWYU pragma: keep
#include "EZ-Template/auton.hpp" // IWYU pragma: keep
#include "EZ-Template/piston.hpp" // IWYU pragma: keep
#include "api.h" // IWYU pragma: keep
#include "pros/motor_group.hpp" // IWYU pragma: keep
#include "pros/optical.hpp" // IWYU pragma: keep

#include "clamp.hpp" // IWYU pragma: keep

extern Drive chassis;

// Your motors, sensors, etc. should go here.  Below are examples



inline pros::Motor Intake(17);

inline pros::Motor Intake1(1);
inline pros::Motor Intake2(-2);

inline pros::Motor LadyBrown(-17);
//inline pros::MotorGroup LadyBrown({2,-3});



inline ez::Piston Mogo('B');
inline ez::Piston Doinker('C');
inline ez::Piston IntakeLift('A');

inline pros::Rotation Rotation(14);
inline pros::Optical Color(16);
inline pros::Distance DistanceColor(6);//Distance Sensor Sorter
inline pros::Distance DistanceSensor(15); //Clamp Distance Sensors

//inline pros::adi::DigitalIn limit_switch('C');


inline void set_lift(int input){
  LadyBrown.move(input);
}


inline ez::PID liftPID{5, 0, 25, 0, "LadyBrown"};

inline void lift_wait() {
  while (liftPID.exit_condition(LadyBrown, true) == ez::RUNNING) {
    pros::delay(ez::util::DELAY_TIME);
  }
}

