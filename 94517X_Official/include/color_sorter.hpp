#ifndef COLOR_SORTER_HPP
#define COLOR_SORTER_HPP

#include "pros/adi.hpp" // IWYU pragma: keep
#include "pros/motors.hpp" // IWYU pragma: keep

namespace ColorSorter {
    extern int IntakeSpeed;


    // Declare functions
    void setIntakeSpeed(int speed);
    void colorSortLogic();
    void startSortingTask();
    void stopSortingTask();
}

#endif // COLOR_SORTER_HPP
