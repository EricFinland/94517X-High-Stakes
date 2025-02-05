#include "main.h" // IWYU pragma: keep
#include "clamp.hpp" // IWYU pragma: keep
#include "pros/abstract_motor.hpp"// IWYU pragma: keep
#include "pros/llemu.hpp" // IWYU pragma: keep
#include "pros/misc.h" // IWYU pragma: keep
#include "pros/motors.h" // IWYU pragma: keep
#include "subsystems.hpp" // IWYU pragma: keep
#include "team_color.hpp"  // IWYU pragma: keep
#include "color_sorter.hpp" // IWYU pragma: keep





// Chassis constructor
ez::Drive chassis(
    // These are your drive motors, the first motor is used for sensing!
    {-20, -19, 18},     // Left Chassis Ports (negative port will reverse it!)
    {11, 12, -13},  // Right Chassis Ports (negative port will reverse it!)

    10,      // IMU Port
    3.25,  // Wheel Diameter (Remember, 4" wheels without screw holes are actually 4.125!)
    480);   // Wheel RPM = cartridge * (motor gear / wheel gear)

// Are you using tracking wheels?  Comment out which ones you're using here!
//  `2.75` is the wheel diameter
//  `4.0` is the distance from the center of the wheel to the center of the robot
// ez::tracking_wheel right_tracker({-'A', -'B'}, 2.75, 4.0);  // ADI Encoders
// ez::tracking_wheel left_tracker(1, {'C', 'D'}, 2.75, 4.0);  // ADI Encoders plugged into a Smart port
ez::tracking_wheel horiz_tracker(8, 2, 2.0); // Rotation sensors

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
  // Print our branding over your terminal :D
  ez::ez_template_print();
  ClampSystem::initialize(); //Mogo Clamp

  ColorSorter::startSortingTask();
  
  Color.set_led_pwm(100);
  //Rotation.reset();
  //Rotation.set_position(1);
  //liftPID.exit_condition_set(80, 50, 300, 150, 500, 500);
  pros::delay(1000);  // Stop the user from doing anything while legacy ports configure

  
  // Are you using tracking wheels?  Comment out which ones you're using here!
  // chassis.odom_tracker_right_set(&right_tracker);
  // chassis.odom_tracker_left_set(&left_tracker);
  // chassis.odom_tracker_back_set(&horiz_tracker);  // Replace `back` to `front` if your tracker is in the front!




  // Configure your chassis controls
  chassis.opcontrol_curve_buttons_toggle(false);   // Enables modifying the controller curve with buttons on the joysticks
  chassis.opcontrol_drive_activebrake_set(0.0);   // Sets the active brake kP. We recommend ~2.  0 will disable.
  //chassis.opcontrol_curve_default_set(0.0, 0.0);  // Defaults for curve. If using tank, only the first parameter is used. (Comment this line out if you have an SD card!)

  // Set the drive to your own constants from autons.cpp!
  default_constants();

  // These are already defaulted to these buttons, but you can change the left/right curve buttons here!
  // chassis.opcontrol_curve_buttons_left_set(pros::E_CONTROLLER_DIGITAL_LEFT, pros::E_CONTROLLER_DIGITAL_RIGHT);  // If using tank, only the left side is used.
  // chassis.opcontrol_curve_buttons_right_set(pros::E_CONTROLLER_DIGITAL_Y, pros::E_CONTROLLER_DIGITAL_A);

  // Autonomous Selector using LLEMU
  ez::as::auton_selector.autons_add({
      {"Negative Side Red", Negative_side},
      {"Negative Side Blue", Negative_side_2},
      {"Red Positive Simple", Positive_side},
      {"Blue Poisitve simple", Positive_side_2},
      {"Skills Autonomous", Skills},
      {"Rush Auto BLUE\n\nRushing The middle mogo Blue Side :D", motion_chaining},
      {"Rush Reds", combining_movements},
      {"Interference\n\nAfter driving forward, robot performs differently if interfered or not", interfered_example},
      {"Simple Odom\n\nThis is the same as the drive example, but it uses odom instead!", odom_drive_example},
      {"Pure Pursuit\n\nGo to (0, 30) and pass through (6, 10) on the way.  Come back to (0, 0)", odom_pure_pursuit_example},
      {"Pure Pursuit Wait Until\n\nGo to (24, 24) but start running an intake once the robot passes (12, 24)", odom_pure_pursuit_wait_until_example},
      {"Boomerang\n\nGo to (0, 24, 45) then come back to (0, 0, 0)", odom_boomerang_example},
      {"Boomerang Pure Pursuit\n\nGo to (0, 24, 45) on the way to (24, 24) then come back to (0, 0, 0)", odom_boomerang_injected_pure_pursuit_example},
      {"Measure Offsets\n\nThis will turn the robot a bunch of times and calculate your offsets for your tracking wheels.", odom_boomerang_injected_pure_pursuit_example},
  });

  // Initialize chassis and auton selector
  chassis.initialize();
  ez::as::initialize();
  master.rumble(chassis.drive_imu_calibrated() ? "." : ".");
}


void lift_task() {
  pros::delay(2000);  // Set EZ-Template calibrate before this function starts running
  while (true) {
    int armp = Rotation.get_position()/100;
    set_lift(liftPID.compute(armp));

    pros::delay(ez::util::DELAY_TIME);
  }
}

pros::Task Lift_Task(lift_task); 


int DriverHappened = 0;

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {
  Lift_Task.suspend();




  // . . .
}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {
  // . . .
}

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */
void autonomous() {
  chassis.pid_targets_reset();                // Resets PID targets to 0
  chassis.drive_imu_reset();                  // Reset gyro position to 0
  chassis.drive_sensor_reset();               // Reset drive sensors to 0
  chassis.odom_xyt_set(0_in, 0_in, 0_deg);    // Set the current position, you can start at a specific position with this
  chassis.drive_brake_set(MOTOR_BRAKE_HOLD);  // Set motors to hold.  This helps autonomous consistency
  Lift_Task.resume();


  /*
  Odometry and Pure Pursuit are not magic

  It is possible to get perfectly consistent results without tracking wheels,
  but it is also possible to have extremely inconsistent results without tracking wheels.
  When you don't use tracking wheels, you need to:
   - avoid wheel slip
   - avoid wheelies
   - avoid throwing momentum around (super harsh turns, like in the example below)
  You can do cool curved motions, but you have to give your robot the best chance
  to be consistent
  */
  //selector.run_auton();
ez::as::auton_selector.selected_auton_call();  // Calls selected auton from autonomous selector
}

/**
 * Simplifies printing tracker values to the brain screen
 */
void screen_print_tracker(ez::tracking_wheel *tracker, std::string name, int line) {
  std::string tracker_value = "", tracker_width = "";
  // Check if the tracker exists
  if (tracker != nullptr) {
    tracker_value = name + " tracker: " + util::to_string_with_precision(tracker->get());             // Make text for the tracker value
    tracker_width = "  width: " + util::to_string_with_precision(tracker->distance_to_center_get());  // Make text for the distance to center
  }
  ez::screen_print(tracker_value + tracker_width, line);  // Print final tracker text
}

/**
 * Ez screen task
 * Adding new pages here will let you view them during user control or autonomous
 * and will help you debug problems you're having
 */
void ez_screen_task() {
  while (true) {
    // Only run this when not connected to a competition switch
    if (!pros::competition::is_connected()) {
      // Blank page for odom debugging
      if (chassis.odom_enabled() && !chassis.pid_tuner_enabled()) {
        // If we're on the first blank page...
        if (ez::as::page_blank_is_on(0)) {
          // Display X, Y, and Theta
          ez::screen_print("x: " + util::to_string_with_precision(chassis.odom_x_get()) +
                               "\ny: " + util::to_string_with_precision(chassis.odom_y_get()) +
                               "\nAngle: " + util::to_string_with_precision(chassis.odom_theta_get()),
                           1);  // Don't override the top Page line

          // Display all trackers that are being used
          screen_print_tracker(chassis.odom_tracker_left, "l", 4);
          screen_print_tracker(chassis.odom_tracker_right, "r", 5);
          screen_print_tracker(chassis.odom_tracker_back, "b", 6);
          screen_print_tracker(chassis.odom_tracker_front, "f", 7);
        }
      }
    }

    // Remove all blank pages when connected to a comp switch
    else {
      if (ez::as::page_blank_amount() > 0)
        ez::as::page_blank_remove_all();
    }

    pros::delay(ez::util::DELAY_TIME);
  }
}
pros::Task ezScreenTask(ez_screen_task);

/**
 * Gives you some extras to run in your opcontrol:
 * - run your autonomous routine in opcontrol by pressing DOWN and B
 *   - to prevent this from accidentally happening at a competition, this
 *     is only enabled when you're not connected to competition control.
 * - gives you a GUI to change your PID values live by pressing X
 */
void ez_template_extras() {
  // Only run this when not connected to a competition switch
  if (!pros::competition::is_connected()) {
    // PID Tuner
    // - after you find values that you're happy with, you'll have to set them in auton.cpp
    // Enable / Disable PID Tuner
    //  When enabled:
    //  * use A and Y to increment / decrement the constants
    //  * use the arrow keys to navigate the constants
    if (master.get_digital_new_press(DIGITAL_X))
      chassis.pid_tuner_toggle();

    // Trigger the selected autonomous routine
    if (master.get_digital(DIGITAL_B) ) {
      pros::motor_brake_mode_e_t preference = chassis.drive_brake_get();
      autonomous();
      chassis.drive_brake_set(preference);
    }

    // Allow PID Tuner to iterate
    chassis.pid_tuner_iterate();
  }

  // Disable PID Tuner when connected to a comp switch
  else {
    if (chassis.pid_tuner_enabled())
      chassis.pid_tuner_disable();
  }
}


void opcontrol() {
  // This is preference to what you like to drive on
  chassis.drive_brake_set(MOTOR_BRAKE_COAST);
  LadyBrown.set_brake_mode(MOTOR_BRAKE_HOLD);

  Intake1.set_brake_mode(MOTOR_BRAKE_COAST);
  Intake2.set_brake_mode(MOTOR_BRAKE_COAST);
  if (TeamColor::isRedTeam) {
    // Robot is on the red team

    pros::lcd::set_text(5, "RED team!");
    master.print(1, 1, "RED team!");
  } else {
    // Robot is on the blue team
    pros::lcd::set_text(5, "BLUE team!");
    master.print(1, 1, "BLUE team!");
  }
  //ColorSorter::startSortingTask();
  Lift_Task.suspend();   

    int directSpeed = 0;
    int pidOutput = 0;



  while (true) {
    // Gives you some extras to make EZ-Template ezier
    ez_template_extras();

    //chassis.opcontrol_tank();  // Tank control
    // chassis.opcontrol_arcade_standard(ez::SPLIT);   // Standard split arcade
    // chassis.opcontrol_arcade_standard(ez::SINGLE);  // Standard single arcade
    chassis.opcontrol_arcade_flipped(ez::SPLIT);    // Flipped split arcade
    // chassis.opcontrol_arcade_flipped(ez::SINGLE);   // Flipped single arcade

    ClampSystem::update();


    if (master.get_digital(DIGITAL_UP)) {
      liftPID.target_set(10);
    }
    if (master.get_digital(DIGITAL_DOWN)) {
      liftPID.target_set(-10);
    }
    if (master.get_digital(DIGITAL_L2)) {
      liftPID.target_set(160);
    }
  if (master.get_digital(DIGITAL_LEFT)) {
      liftPID.target_set(275);
    }
    int armpos = Rotation.get_position() / 100;

    set_lift(liftPID.compute(armpos));

    pros::lcd::print(7, "Arm position: %i!", armpos);

    //console.printf("Arm Position: %i\n", armpos);

    if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_Y)){
      TeamColor::isRedTeam = !TeamColor::isRedTeam;
    }

        if (TeamColor::isRedTeam) {
          // Robot is on the red team
          pros::lcd::set_text(3, "RED team!");
          master.set_text(1, 1, "RED team!");

        } else {
          // Robot is on the blue team
          pros::lcd::set_text(3, "BLUE team!");
          master.set_text(1, 1, "BLUE team!");
        }


    if (master.get_digital_new_press(DIGITAL_RIGHT)){
      Doinker.set(!Doinker.get());
    }
    if (master.get_digital_new_press(DIGITAL_A)) {
      IntakeLift.set(!IntakeLift.get());
    }
    pros::delay(ez::util::DELAY_TIME);  // This is used for timer calculations!  Keep this ez::util::DELAY_TIME
  }
}
