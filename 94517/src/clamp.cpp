#include "clamp.hpp" // IWYU pragma: keep
#include "pros/misc.h" // IWYU pragma: keep
#include "subsystems.hpp" // IWYU pragma: keep

// State Variables (only used in this file)
bool clampEngaged = false;          // True if the clamp is engaged
bool mogoDetected = false;          // True if a mogo is currently detected
bool previousMogoDetected = false;  // Tracks if a mogo was detected in previous loop
bool manualToggleState = false;     // Tracks if clamp is manually clamped or unclamped
bool buttonPressedLastCycle = false; // Used for edge detection of button press
bool manualControlActive = false;   // True if manual control is being used (disables auto clamp)

namespace ClampSystem {
    void initialize() {
        // This function runs once at the start
        clampEngaged = false;
        previousMogoDetected = false;
        manualToggleState = false;
        buttonPressedLastCycle = false;
        manualControlActive = false;
        Mogo.set(false); // Ensure the clamp starts in the open position
    }

    void update() {
        // Handle manual toggle logic
        toggleManualClamp();

        // Get the distance sensor value and see if a mogo is detected
        double distance = DistanceSensor.get(); // Get distance in mm
        mogoDetected = (distance < 100); // Adjust 100 mm as threshold for detection

        // Auto Clamp Logic - Only clamp if a *new* mogo is detected, but only if manual control is NOT active
        if (mogoDetected && !previousMogoDetected) { 
            // A new mogo is detected (transition from no mogo to detected mogo)
            if (!manualControlActive) { 
                clampEngaged = true;
                master.rumble(".");
                Mogo.set(true); // Clamp the piston
            }
        }

        // If the mogo is no longer detected, reset manual control state
        if (!mogoDetected) { 
            manualControlActive = false; // Reset manual control if no mogo is near
        }

        // Update the "previousMogoDetected" state for the next loop
        previousMogoDetected = mogoDetected;
    }

    void toggleManualClamp() {
        // Edge detection logic to detect button press (not hold)
        bool buttonPressed = pros::c::controller_get_digital(pros::E_CONTROLLER_MASTER, pros::E_CONTROLLER_DIGITAL_L1);

        // Detect the rising edge of the button press (only trigger on press, not hold)
        if (buttonPressed && !buttonPressedLastCycle) {
            // Toggle the manual clamp state
            manualToggleState = !manualToggleState;
            manualControlActive = true; // Mark manual control as active
            Mogo.set(manualToggleState); // Engage or disengage the piston
        }

        // Update the button state for edge detection
        buttonPressedLastCycle = buttonPressed;
    }
}
