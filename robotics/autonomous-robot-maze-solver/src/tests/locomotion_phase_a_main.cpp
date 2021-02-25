#include <Arduino.h>
#include <hardware.h>
#include <hardware_definition.h>

static const double LEFT_WHEEL_TARGET = -90 * PI / 180.0; // rads
static const double RIGHT_WHEEL_TARGET = 90 * PI / 180.0; // rads
static const int MIN_MOTOR_DRIVE = 20;
static const int MAX_MOTOR_DRIVE = 80;


using led = hardware::led;

void toggle_led() {
    led::high();
    delay(2000);
    led::low();
}

template <typename Wheel>
static void print_wheel_info() {
    units::millimeters wheel_pos = Wheel::position();
    Serial.print("Encoder count = ");
    Serial.print((long int)Wheel::count());
    Serial.print(" Wheel position (mm) = ");
    Serial.println(wheel_pos.count());
}

template<typename Motor, typename Wheel>
void move_to_angle(double angle) {
    Serial.println("Moving to angle...");

    //int target = Wheel::count() + angle_rads / static_cast<double>(hardware::locomotion::RADS_PER_COUNT);
    auto start_counts = Wheel::count();
    int additional_counts = angle / (double)RADS_PER_COUNT;

    //int target = start_counts + additional_counts;

    if (angle > 0) {
        Motor::forward(units::percentage{MIN_MOTOR_DRIVE});
    } else {
        Motor::backward(units::percentage{MIN_MOTOR_DRIVE});
    }

    Serial.print("current wheel count ");
    Serial.print((long int)Wheel::count());
    Serial.print(" angle ");
    Serial.print(angle);
    Serial.print(" rads per count ");
    Serial.print(RADS_PER_COUNT);
    Serial.print(" additonal counts ");
    Serial.println(additional_counts);

    while (abs(start_counts - Wheel::count()) < abs(additional_counts)) {
        print_wheel_info<Wheel>();
    }

    Motor::stop();
    print_wheel_info<Wheel>();

    Serial.println("...done");
}


template<typename Motor, typename Wheel>
void run_motor(hardware::drive_direction dir) {
    Serial.println("Starting run_motor test");

    if (dir == hardware::drive_direction::forward) {
        Motor::forward(units::percentage{MAX_MOTOR_DRIVE});
    } else {
        Motor::backward(units::percentage{MAX_MOTOR_DRIVE});
    }
    auto start = millis();

    while (millis() - start < 5000) {
        print_wheel_info<Wheel>();
    }

    Motor::stop();
    print_wheel_info<Wheel>();

    Serial.println("...done");
}

void imu_demo() {
  while (1) {
      while(!hardware::imu::update());
      Serial.print("yaw ");
      Serial.print(hardware::imu::yaw());
      Serial.print("\n");
  }
}

void setup() {
    Serial.begin(115200);
    led::config_io_mode(hardware::io_mode::output);

    Serial.println("Enabling devices");
    hardware::left_wheel::enable();
    hardware::left_motor::enable();
    hardware::right_wheel::enable();
    hardware::right_motor::enable();
    hardware::i2c::enable();
    hardware::imu::enable();

    Serial.println("Stabilising IMU");
    while(!hardware::imu::update());
    hardware::imu::stabilize();
    Serial.println("GTG");
}

void loop() {
    int input = Serial.parseInt();
    if (input == 0) return;

    switch(input) {
        case 1: toggle_led(); break;
        case 2: run_motor<hardware::left_motor, hardware::left_wheel>(hardware::drive_direction::forward); break;
        case 3: run_motor<hardware::right_motor, hardware::right_wheel>(hardware::drive_direction::backward); break;
        case 4: move_to_angle<hardware::left_motor, hardware::left_wheel>(LEFT_WHEEL_TARGET); break;
        case 5: move_to_angle<hardware::right_motor, hardware::right_wheel>(RIGHT_WHEEL_TARGET); break;
        case 6: imu_demo(); break;
        default: Serial.print("Unknown option "); Serial.println(input); break;
    }
}
