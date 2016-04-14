#include "Island.h"
#include "Model.h"

#include <iostream>

using std::cout;
using std::endl;

Island::Island (const std::string& name_, Point position_,
                double fuel_, double production_rate_) :
Sim_object(name_), position(position_), fuel(fuel_), production_rate(production_rate_) {}

/* if production_rate > 0, compute production_rate * unit time, and add to amount, 
 and print an update message */
void Island::update() {
    if (production_rate > 0) {
        fuel += production_rate * 1.0;
        cout << "Island " << get_name() << " now has " << fuel << " tons" << endl;
    }
}

// output information about the current state
void Island::describe() const {
    cout << "\nIsland " << get_name() << " at position " << position << endl;
    cout << "Fuel available: " << fuel << " tons" << endl;
}

// ask model to notify views of current state
void Island::broadcast_current_state() const {
    Model::get_instance().notify_location(get_name(), position);
}


/* Return whichever is less, the request or the amount left, 
 update the amount on hand accordingly, and output the amount supplied. */
double Island::provide_fuel(double request) {
    double provide = request < fuel ? request : fuel;
    fuel -= provide;
    cout << "Island " << get_name() << " supplied "
         << provide << " tons of fuel" << endl;
    return provide;
}

/* Add the amount to the amount on hand, 
 and output the total as the amount the Island now has. */
void Island::accept_fuel(double amount) {
    fuel += amount;
    cout << "Island " << get_name() << " now has " << fuel << " tons" << endl;
}