#include "clamp.hpp" // IWYU pragma: keep
#include "pros/misc.h" // IWYU pragma: keep
#include "subsystems.hpp" // IWYU pragma: keep

// State Variables (only used in this file)
bool clampEngaged = false;          // True if the clamp is engaged
bool mogoDetected = false;          // True if a mogo is currently detected
bool previousMogoDetected = false;  // Tracks if a mogo was detected in previous loop
bool manualControlActive = false;   // True if manual control is being used
bool clampReset = true;             // True if the clamp is ready to clamp again
bool mogoFullyLeft = true;           // Ensures a mogo must fully leave before a new one is detected

namespace ClampSystem {
    void handleManualClamp(); // Forward declaration

    void initialize() {
        // Initialize clamp states
        clampEngaged = false;
        mogoDetected = false;
        previousMogoDetected = false;
        manualControlActive = false;
        clampReset = true;
        mogoFullyLeft = true;
        Mogo.set(false); // Ensure the clamp starts in the open position
    }

    void update() {
        // Handle manual toggle logic
        handleManualClamp();

        // Get the distance sensor value
        double distance = DistanceSensor.get(); // Get distance in mm
        mogoDetected = (distance < 20); // Mogo detected within 20mm
        bool mogoOutOfRange = (distance > 40); // Mogo fully left beyond 40mm

        // Track if a mogo has fully left before allowing a new one to be clamped
        if (mogoOutOfRange) {
            mogoFullyLeft = true; // Mark that the previous mogo is gone
        }

        // Auto Clamp Logic
        if (!manualControlActive && mogoDetected && mogoFullyLeft && clampReset) {
            // Clamp only if a new mogo is detected after the last one fully left
            clampEngaged = true;
            Mogo.set(true);
            clampReset = false; // Prevent immediate reclamping
            mogoFullyLeft = false; // Mark that we are holding a mogo now
        }

        // Reset clamp system when no mogo is detected
        if (!mogoDetected) {
            clampReset = true; // Allow reclamping when a new mogo is detected
            manualControlActive = false; // Reset manual control if no mogo is near
        }

        // Update previous detection state
        previousMogoDetected = mogoDetected;
    }

    void handleManualClamp() {
        // Check if the button was pressed (edge detection handled by PROS)
        if (pros::c::controller_get_digital_new_press(pros::E_CONTROLLER_MASTER, pros::E_CONTROLLER_DIGITAL_L1)) {
            // Toggle clamp state
            clampEngaged = !clampEngaged;
            manualControlActive = true; // Manual override activated
            Mogo.set(clampEngaged); // Update piston state

            // If manually unclamping, allow the next mogo to be recognized
            if (!clampEngaged) {
                mogoFullyLeft = false; // Prevent immediate reclamping on the same mogo
            }
        }
    }
}
