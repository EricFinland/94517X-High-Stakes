#ifndef CLAMP_HPP
#define CLAMP_HPP

#include "pros/misc.hpp" // IWYU pragma: keep

// Clamp namespace for organization
namespace ClampSystem {

    void initialize(); // Initializes the clamp system
    void update();//update logic
    void toggleManualClamp(); // Manually Clamp for debugging
}

#endif // CLAMP_HPP
