#include "Sailing_view.h"
#include <iostream>
#include <iomanip>

using std::setw;
using std::cout;
using std::endl;
using std::string;

// prints out textual information about all ships
void Sailing_view::draw() const {
    cout << "----- Sailing Data -----" << endl;
    cout << setw(10) << "Ship" << setw(10) << "Fuel"
    << setw(10) << "Course" << setw(10) << "Speed" << endl;
    for (const auto& data_pair : memory) {
        auto& data = data_pair.second;
        cout << setw(10) << data_pair.first << setw(10) << data.fuel
        << setw(10) << data.course << setw(10) << data.speed << endl;
    }
}

// Remove the name and its location; no error if the name is not present.
void Sailing_view::update_remove(const std::string& name) {
    memory.erase(name);
}

// Save the supplied name and fuel for future use in a draw() call
void Sailing_view::update_fuel(const std::string& name, double fuel) {
    memory[name].fuel = fuel;
}

// Save the supplied name and course for future use in a draw() call
void Sailing_view::update_course(const std::string& name, double course) {
    memory[name].course = course;
}

// Save the supplied name and speed for future use in a draw() call
void Sailing_view::update_speed(const std::string& name, double speed) {
    memory[name].speed = speed;
}



