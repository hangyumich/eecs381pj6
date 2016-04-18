#include "Sailing_view.h"
#include "Utility.h"
#include <iostream>
#include <iomanip>
#include <algorithm>

using std::for_each;
using std::setw;
using std::cout;
using std::endl;
using std::string;

// override output operator for Data
std::ostream& operator<< (std::ostream& os, const Data& data) {
    os << data.fuel << " " << data.course << " " << data.speed;
    return os;
}

// istream ctor for sailing view
Sailing_view::Sailing_view(std::istream& is) {
    int memory_size = read_int(is);
    while (memory_size--) {
        string key;
        is >> key;
        memory[key] = Data(read_double(is), read_double(is), read_double(is));
    }
}

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

void Sailing_view::save(std::ostream &os) const{
    os << "Sailing_view" << endl;
    os << memory.size() << endl;
    std::for_each(memory.begin(), memory.end(), [&os](const std::pair<const std::string, Data>& pair){os << pair.first << " " << pair.second << endl;});
    
}

