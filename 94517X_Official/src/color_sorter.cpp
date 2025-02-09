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
#define DISTANCE_THRESHOLD 80 // Ensure this is correctly set

// Ring tracking queue
std::queue<int> RingQueue; // 0 = red, 1 = blue

// State variables
namespace ColorSorter {
int IntakeSpeed = 0;
bool sortingTaskActive = false;

void setIntakeSpeed(int speed) {
    IntakeSpeed = speed; // Update global intake speed variable
    Intake1.move(IntakeSpeed);
    Intake2.move(IntakeSpeed);
    IntakeS1.move(IntakeSpeed);
    IntakeS2.move(IntakeSpeed);

}

void setIntakeFront(int speeds) {
    IntakeSpeed = speeds;
    Intake1.move(IntakeSpeed);
}

void colorSortLogic() {
    int teamColor = TeamColor::isRedTeam ? 0 : 1; // 0 = red, 1 = blue

    while (true) {


        // Detect color
        double hueValue = Color.get_hue();
        int detectedColor = -1; 

        if (hueValue >= HUE_BLUE_MIN && hueValue <= HUE_BLUE_MAX) {
            detectedColor = 1; // Blue detected

        } else if ((hueValue >= HUE_RED_MIN && hueValue <= HUE_RED_MAX) || 
                   (hueValue >= HUE_RED_MIN_ALT && hueValue <= HUE_RED_MAX_ALT)) {
            detectedColor = 0; // Red detected

        } 

        // Add detected color to queue
        if (detectedColor != -1) {
            if (RingQueue.size() < 3) {
                RingQueue.push(detectedColor);
            } else {
                RingQueue.pop();
                RingQueue.push(detectedColor);
            }
        }

        // Get distance
        int distance = DistanceColor.get_distance();


        // Sorting Logic
        if (distance < DISTANCE_THRESHOLD && !RingQueue.empty()) {
            int ringToSort = RingQueue.front();
            RingQueue.pop();

            if (ringToSort != teamColor) { // If ring is wrong color, eject it


                pros::delay(80);   // Allow time for motor to sto0
                setIntakeSpeed(-127); // Reverse intake to eject
                pros::delay(100);    // Hold reverse longer

                setIntakeSpeed(127); // Resume normal intake

            }
        }

        pros::delay(25); // Small delay for efficiency
    }
}


// Start Sorting Task
void startSortingTask() {
    if (!sortingTaskActive) {
        sortingTaskActive = true;
        pros::Task colorSorterTask(colorSortLogic);
    }
}

// Stop Sorting Task
void stopSortingTask() {
    sortingTaskActive = false;
}
}
