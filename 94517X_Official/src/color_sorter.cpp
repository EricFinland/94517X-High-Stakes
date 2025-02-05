#include <queue> // For queue implementation
#include "color_sorter.hpp" // IWYU pragma: keep
#include "team_color.hpp" // IWYU pragma: keep
#include "pros/misc.h" // IWYU pragma: keep
#include "pros/rtos.hpp" // IWYU pragma: keep
#include "subsystems.hpp" // IWYU pragma: keep

// Hue thresholds for red and blue
#define HUE_BLUE_MIN 200
#define HUE_BLUE_MAX 260
#define HUE_RED_MIN 1
#define HUE_RED_MAX 30
#define HUE_RED_MIN_ALT 0
#define HUE_RED_MAX_ALT 359
#define DISTANCE_THRESHOLD 100

// Ring tracking queue
std::queue<int> RingQueue; // 0 = red, 1 = blue

// State variables


namespace ColorSorter {
int IntakeSpeed = 0;

    
void setIntakeSpeed(int speed) {
    IntakeSpeed = speed; // Update global intake speed variable
    Intake1.move(IntakeSpeed);
    Intake2.move(IntakeSpeed);
}



void colorSortLogic() {
    int teamColor = TeamColor::isRedTeam ? 0 : 1; // 0 = red, 1 = blue

    while (true) {
        // Only update intake speed if the driver is manually pressing a button
        /*
        if (!pros::competition::is_autonomous()) {
        if (master.get_digital(pros::E_CONTROLLER_DIGITAL_R1)) {
            setIntakeSpeed(127);
        } else if (master.get_digital(pros::E_CONTROLLER_DIGITAL_R2)) {
            setIntakeSpeed(-127);
        }else {
            setIntakeSpeed(0);
        }
        }*/

    Intake1.move(IntakeSpeed);
    Intake2.move(IntakeSpeed);

        // Sorting Logic
        if (IntakeSpeed >= 100) { 
            double hueValue = Color.get_hue();
            int detectedColor = -1; 

            if (hueValue >= HUE_BLUE_MIN && hueValue <= HUE_BLUE_MAX) {
                detectedColor = 1; // Blue detected
            } else if ((hueValue >= HUE_RED_MIN && hueValue <= HUE_RED_MAX) || 
                       (hueValue >= HUE_RED_MIN_ALT && hueValue <= HUE_RED_MAX_ALT)) {
                detectedColor = 0; // Red detected
            }

            // If a valid color is detected, add it to the queue
            if (detectedColor != -1) {
                if (RingQueue.size() < 3) {
                    RingQueue.push(detectedColor);
                } else {
                    RingQueue.pop();
                    RingQueue.push(detectedColor);
                }
            }

            // Distance sensor detects a ring
            int distance = DistanceColor.get_distance();
            if (distance < DISTANCE_THRESHOLD && !RingQueue.empty()) {
                int ringToSort = RingQueue.front();
                RingQueue.pop();

                if (ringToSort != teamColor) { // If the ring is the "bad color," sort it
                    pros::delay(70); 
                    setIntakeSpeed(-127); // Reverse intake to eject
                    pros::delay(100); 
                    setIntakeSpeed(127); // Resume normal intake
                }
            }
        }

        pros::delay(25); // Small delay for efficiency
    }
}


void startSortingTask() {
        pros::Task colorSorterTask(colorSortLogic);
}

void stopSortingTask() {
    //sortingTaskActive = false;
}
}