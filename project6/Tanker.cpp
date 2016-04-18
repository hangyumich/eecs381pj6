#include "Tanker.h"
#include "Island.h"
#include "Utility.h"
#include "Model.h"

#include <iostream>
#include <memory>

using std::cout;
using std::endl;
using std::shared_ptr;

const char* const load_unload_the_same_c = "Load and unload cargo destinations are the same!";

enum class Tanker_state {no_cargo_destinations, moving_to_loading, unloading,
    loading, moving_to_unloading};

Tanker:: Tanker(const std::string& name_, Point position_) :
Ship(name_, position_, 100., 10., 2., 0.), cargo(0), cargo_capacity(1000.),
tanker_state(Tanker_state::no_cargo_destinations) {}

Tanker::Tanker(std::istream& is): Ship(is), cargo(read_double(is)), cargo_capacity(read_double(is)), tanker_state((Tanker_state) read_int(is)) {
    std::string str;
    is >> str;
    if (str == "load_destination") {
        load_destination = read_island_ptr(is);
        is >> str;
    }
    if (str == "unload_destination") {
        unload_destination = read_island_ptr(is);
    }
}

// If Tanker has assigned cargo destinations, throw Error.
void Tanker::set_destination_position_and_speed(Point destination_point, double speed) {
    throw_if_in_cycle();
    Ship::set_destination_position_and_speed(destination_point, speed);
}

// If Tanker has assigned cargo destinations, throw Error.
void Tanker::set_destination_island_and_speed(std::shared_ptr<Island> destination_island,
                                              double speed) {
    throw_if_in_cycle();
    Ship::set_destination_island_and_speed(destination_island, speed);
}

// If Tanker has assigned cargo destinations, throw Error.
void Tanker::set_course_and_speed(double course, double speed) {
    throw_if_in_cycle();
    Ship::set_course_and_speed(course, speed);
}

/* Set the loading Island destination
 if both destinations are now set, start the cargo cycle */
void Tanker::set_load_destination(shared_ptr<Island> load_island) {
    throw_if_in_cycle();
    load_destination = load_island;
    if (load_destination == unload_destination)
        throw Error(load_unload_the_same_c);
    cout << get_name() << " will load at " << load_island->get_name() << endl;
    start_cycle_if_ready();
}

/* Set the unloading Island destination
 if both destinations are now set, start the cargo cycle */
void Tanker::set_unload_destination(shared_ptr<Island> unload_island) {
    throw_if_in_cycle();
    unload_destination = unload_island;
    if (load_destination == unload_destination)
        throw Error(load_unload_the_same_c);
    cout << get_name() << " will unload at " << unload_island->get_name() << endl;
    start_cycle_if_ready();
}

// Throw error if tanker is in cargo cycle.
void Tanker::throw_if_in_cycle() {
    if (tanker_state != Tanker_state::no_cargo_destinations)
        throw Error("Tanker has cargo destinations!");
}

// If both destination are set, start the cargo cycle
void Tanker::start_cycle_if_ready() {
    if (load_destination == nullptr || unload_destination == nullptr)
        return;
    if (is_docked()) {
        if (get_location() == load_destination->get_location())
            tanker_state = Tanker_state::loading;
        else if (get_location() == unload_destination->get_location())
            tanker_state = Tanker_state::unloading;
        else
            move_based_on_cargo();
    } else if (!is_moving()) {
        if (cargo == 0. && can_dock(load_destination)) {
            dock(load_destination);
            tanker_state = Tanker_state::loading;
        } else if (cargo > 0. && can_dock(unload_destination)) {
            dock(unload_destination);
            tanker_state = Tanker_state::unloading;
        } else {
            move_based_on_cargo();
        }
    } else {
        move_based_on_cargo();
    }
}

// Move to loading/unloading destination based on cargo amount.
void Tanker::move_based_on_cargo() {
    if (cargo == 0.) {
        Ship::set_destination_island_and_speed(load_destination, get_maximum_speed());
        tanker_state = Tanker_state::moving_to_loading;
    } else if (cargo > 0.) {
        Ship::set_destination_island_and_speed(unload_destination, get_maximum_speed());
        tanker_state = Tanker_state::moving_to_unloading;
    }
}

// when told to stop, clear the cargo destinations and stop
void Tanker::stop() {
    Ship::stop();
    load_destination = nullptr;
    unload_destination = nullptr;
    tanker_state = Tanker_state::no_cargo_destinations;
    cout << get_name() <<  " now has no cargo destinations" << endl;
}

// Perform Tanker-specific behavior after ship update.
void Tanker::update() {
    Ship::update();
    if (!can_move()) {
        tanker_state = Tanker_state::no_cargo_destinations;
        load_destination = nullptr;
        unload_destination = nullptr;
        cout << get_name() << " now has no cargo destinations" << endl;
    } else if (tanker_state == Tanker_state::moving_to_loading) {
        if (!is_moving() && can_dock(load_destination)) {
            dock(load_destination);
            tanker_state = Tanker_state::loading;
        }
    } else if (tanker_state == Tanker_state::moving_to_unloading) {
        if (!is_moving() && can_dock(unload_destination)) {
            dock(unload_destination);
            tanker_state = Tanker_state::unloading;
        }
    } else if (tanker_state == Tanker_state::loading) {
        Ship::refuel();
        double cargo_needed = cargo_capacity - cargo;
        if (cargo_needed < 0.005) {
            cargo = cargo_capacity;
            Ship::set_destination_island_and_speed(unload_destination, get_maximum_speed());
            tanker_state = Tanker_state::moving_to_unloading;
        } else {
            cargo += load_destination->provide_fuel(cargo_needed);
            cout << get_name() << " now has " << cargo << " of cargo" << endl;
        }
    } else if (tanker_state == Tanker_state::unloading) {
        if (cargo == 0.) {
            Ship::set_destination_island_and_speed(load_destination, get_maximum_speed());
            tanker_state = Tanker_state::moving_to_loading;
        } else {
            unload_destination->accept_fuel(cargo);
            cargo = 0.;
        }
    }
}

// Perform Tanker-specific behavior in addition to ship describe
void Tanker::describe() const {
    cout << "\nTanker ";
    Ship::describe();
    cout << "Cargo: " << cargo << " tons";
    if (tanker_state == Tanker_state::no_cargo_destinations)
        cout << ", no cargo destinations" << endl;
    else if (tanker_state == Tanker_state::loading)
        cout << ", loading" << endl;
    else if (tanker_state == Tanker_state::unloading)
        cout << ", unloading" << endl;
    else if (tanker_state == Tanker_state::moving_to_loading)
        cout << ", moving to loading destination" << endl;
    else if (tanker_state == Tanker_state::moving_to_unloading)
        cout << ", moving to unloading destination" << endl;
}

void Tanker::save(std::ostream & os) const{
    os << "Tanker" << endl;
    Ship::save(os);
    os << cargo << " " << cargo_capacity << " " << (int) tanker_state << endl;
    if (load_destination.use_count()) {
        os << "load_destination " << load_destination->get_name() << endl;
    } else {
        os << "no_load_destination" << endl;
    }
    if (unload_destination.use_count()) {
        os << "unload_destination " << unload_destination->get_name() << endl;
    } else {
        os << "no_unload_destination"<< endl;
    }
}

Tanker& Tanker::operator= (const Tanker& in_tanker) {
    Ship::operator=(in_tanker);
    cargo = in_tanker.cargo;
    cargo_capacity = in_tanker.cargo_capacity;
    tanker_state = in_tanker.tanker_state;
    load_destination = in_tanker.load_destination;
    unload_destination = in_tanker.unload_destination;
    return *this;
}





