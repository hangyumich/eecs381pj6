#include "Refuel_ship.h"
#include "Island.h"
#include "Model.h"
#include "Utility.h"
#include "Ship_factory.h"

#include <memory>
#include <iostream>
#include <set>

using std::string;
using std::shared_ptr;
using std::cout;
using std::endl;
using std::set;

enum class Refuel_state { not_refueling, moving_to_start, load_refuel, waiting, moving_to_ship, refuel_target, read_to_back };

// initialize
Refuel_ship::Refuel_ship(const std::string& name_, Point position_): Ship(name_, position_, 100, 5, 1., 0), refuel_state(Refuel_state::not_refueling), cargo(0), cargo_capacity(500) {}

Refuel_ship::Refuel_ship(std::istream& is): Ship(is), refuel_state((Refuel_state)read_int(is)), cargo(read_double(is)), cargo_capacity(read_double(is)){
    std::string str;
    is >> str;
    if (str == "base_island") {
        base_island = read_island_ptr(is);
    }
    is >> str;
    if (str == "target_ship") {
        std::string ship_type, ship_name;
        is >> ship_type >> ship_name;
        try {
            target_ship = Model::get_instance().get_ship_ptr(ship_name);
        } catch (Error& e) {
            shared_ptr<Ship> ship_ptr = create_ship(ship_name, ship_type.substr(1), Point(0, 0));
            target_ship = ship_ptr;
            Model::get_instance().add_ship(ship_ptr);
        }
    }
}
// Perform Refuel_ship-specific behavior after ship update.
void Refuel_ship::update() {
    Ship::update();
    if ( refuel_state == Refuel_state::moving_to_start && !is_moving() && can_dock(base_island)) {
        dock(base_island);
        refuel_state = Refuel_state::load_refuel;
    } else if ( refuel_state == Refuel_state::moving_to_start && !is_moving() && can_move()) {
        // dead and rescued before reached base island
        Ship::set_destination_island_and_speed(base_island, get_maximum_speed());
    } else if ( refuel_state == Refuel_state::load_refuel ) {
        Ship::refuel(); // refuel self
        double cargo_needed = cargo_capacity - cargo; // refuel cargo
        if (cargo_needed < 0.005) {
            cargo = cargo_capacity;
            refuel_state = Refuel_state::waiting;
        } else {
            cargo += base_island->provide_fuel(cargo_needed);
            cout << get_name() << " now has " << cargo << " of cargo" << endl;
        }
    } else if (refuel_state == Refuel_state::waiting) {
        find_next_ship();
    } else if ( refuel_state == Refuel_state::moving_to_ship ) {
        if ( target_ship.expired() )
            refuel_state = Refuel_state::read_to_back;
        else if (cartesian_distance(target_ship.lock()->get_location(), get_location()) < 0.005) { // close enough to refuel
                refuel_state = Refuel_state::refuel_target;
            }
        else if (!is_moving()) { // too far
            // this can happen when some refuel_ship refueled the ship which moved afterwards
            // thus this refuel_ship will not meet the target ship and has to go back
            // if it realizes this is the situation by checking if has stopped but not close to target
            refuel_state = Refuel_state::read_to_back;
        }
    } else if ( refuel_state == Refuel_state::refuel_target ) {
        if (!target_ship.expired()) {
            double used = target_ship.lock()->receive_fuel(cargo); // can provide at most cargo amount
            cout << target_ship.lock()->get_name() << " has received " << used << " of fuel" << endl;
            cargo -= used;
        }
        refuel_state = Refuel_state::read_to_back;
    } else if ( refuel_state == Refuel_state::read_to_back ) {
        target_ship.reset();
        refuel_state = Refuel_state::moving_to_start;
        Ship::set_destination_island_and_speed(base_island, get_maximum_speed());
    }
}

// Perform Refuel_ship-specific behavior in addition to ship describe
void Refuel_ship::describe() const {
    cout << "\nRefuel_ship ";
    Ship::describe();
    if ( refuel_state == Refuel_state::moving_to_start) {
        cout << "Moving to base island " << base_island->get_name() << endl;
    } else if ( refuel_state == Refuel_state::load_refuel ) {
        cout << "loading cargo and refueling at " << base_island->get_name() << endl;
    } else if (refuel_state == Refuel_state::waiting) {
        cout << "waiting at " << base_island->get_name() << endl;
    } else if ( refuel_state == Refuel_state::moving_to_ship ) {
        if (!target_ship.expired()) {
            cout << "Moving to ship " << target_ship.lock()->get_name() << endl;
        } else {
            cout << "Moving to absent ship" << endl;
        }
    } else if ( refuel_state == Refuel_state::refuel_target ) {
        if (!target_ship.expired()) {
            cout << "Refueling ship " << target_ship.lock()->get_name() << endl;
        } else {
            cout << "Refueling absent ship" << endl;
        }
    } else if ( refuel_state == Refuel_state::read_to_back ) {
        cout << "Ready to go back to " << base_island->get_name() << endl;
    }
}

// Throw error if tanker in refueling cycle.
void Refuel_ship::set_destination_position_and_speed(Point destination_point, double speed) {
    throw_if_in_cycle();
    Ship::set_destination_position_and_speed(destination_point, speed);
}

// Sets new base_island and starts entering refuel states
void Refuel_ship::set_destination_island_and_speed(std::shared_ptr<Island> destination_island,
                                                   double speed) {
    target_ship.reset();
    base_island = destination_island;
    refuel_state = Refuel_state::moving_to_start;
    Ship::set_destination_island_and_speed(destination_island, speed);
}

// Throw error if tanker in refueling cycle.
void Refuel_ship::set_course_and_speed(double course, double speed) {
    throw_if_in_cycle();
    Ship::set_course_and_speed(course, speed);
}

// Cancels current refuel cycle and perform the ship specific behaviors
void Refuel_ship::stop() {
    Ship::stop();
    target_ship.reset();
    base_island.reset();
    refuel_state = Refuel_state::not_refueling;
    cout << get_name() <<  " now not in refueling state" << endl;
}

// save ship status to os
void Refuel_ship::save(std::ostream & os) const {
    os << "Refuel_ship" << endl;
    Ship::save(os);
    os << (int)refuel_state << " " << cargo << " " << cargo_capacity << " " << endl;
    if (base_island) {
        os << "base_island " << base_island->get_name() << endl;
    } else {
        os << "no_base_island" << endl;
    }
    if (target_ship.expired()) {
        os << "no_taget" << endl;
    } else {
        os << "target_ship" << typeid(*target_ship.lock()).name()
        <<" "<<target_ship.lock()->get_name() << endl;
    }
}

Refuel_ship& Refuel_ship::operator= (const Refuel_ship& rhs) {
    Ship::operator=(rhs);
    refuel_state = rhs.refuel_state;
    cargo = rhs.cargo;
    cargo_capacity = rhs.cargo_capacity;
    base_island = rhs.base_island;
    target_ship = rhs.target_ship;
    return *this;
}

// Throw error if Refuel_ship not not_refueling.
void Refuel_ship::throw_if_in_cycle() {
    if ( refuel_state != Refuel_state::not_refueling )
        throw Error("Refuel_ship in cycle!");
}

// Find the nearest dead_in_water ship within 20 of base island and set as target
void Refuel_ship::find_next_ship() {
    set<shared_ptr<Ship>, Comp> ships = Model::get_instance().get_ships();
    double min_distance = -1;
    shared_ptr<Ship> nearest;
    for (auto& i : ships) {
        if ( i->can_move() ) // only dead_in_water ships
            continue;
        double distance = cartesian_distance(i->get_location(), base_island->get_location());
        if ( distance < 20 ) {
            if ( min_distance < 0 || distance < min_distance ) {
                min_distance = distance;
                nearest = i;
            }
        }
    }
    if ( nearest ) { // is there is such a ship to refuel
        refuel_state = Refuel_state::moving_to_ship;
        target_ship = nearest;
        Ship::set_destination_position_and_speed(nearest->get_location(), get_maximum_speed());
    }
}









