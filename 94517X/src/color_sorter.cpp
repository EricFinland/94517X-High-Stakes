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
#define DISTANCE_THRESHOLD 150

// Ring tracking queue
std::queue<int> ringQueue; // 0 = red, 1 = blue

// State variables
bool sortingTaskActive = false;

void colorSortLogic() {
    int teamColor = TeamColor::isRedTeam ? 0 : 1; // 0 = red, 1 = blue

    while (sortingTaskActive) {
        // Optical sensor detects a ring's color
        double hueValue = Color.get_hue();
        int detectedColor = -1; // -1 = no valid color detected

        if (hueValue >= HUE_BLUE_MIN && hueValue <= HUE_BLUE_MAX) {
            detectedColor = 1; // Blue detected
        } else if ((hueValue >= HUE_RED_MIN && hueValue <= HUE_RED_MAX) || 
                   (hueValue >= HUE_RED_MIN_ALT && hueValue <= HUE_RED_MAX_ALT)) {
            detectedColor = 0; // Red detected
        }

        // If a valid color is detected, add it to the queue
        if (detectedColor != -1) {
            if (ringQueue.size() < 3) {
                ringQueue.push(detectedColor);
            } else {
                // If the queue is full, remove the oldest and add the new color
                ringQueue.pop();
                ringQueue.push(detectedColor);
            }
        }

        // Distance sensor detects a ring
        int distance = Distance.get_distance();
        if (distance < DISTANCE_THRESHOLD && !ringQueue.empty()) {
            int ringToSort = ringQueue.front(); // Get the oldest detected ring color
            ringQueue.pop(); // Remove the processed ring from the queue

            if (ringToSort != teamColor) { // If the ring is the "bad color," sort it
                pros::delay(70); // Short delay for stability
                Intake.move(-127); // Reverse the intake to eject
                pros::delay(100); // Wait for the ejection
                Intake.move(127); // Resume normal intake
            }
        }

        pros::delay(25); // Small delay for efficiency
    }
}

void startSortingTask() {
    if (!sortingTaskActive) {
        sortingTaskActive = true;
        pros::Task colorSorterTask(colorSortLogic);
    }
}

void stopSortingTask() {
    sortingTaskActive = false;
}
