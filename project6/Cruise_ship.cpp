#include "Cruise_ship.h"
#include "Island.h"
#include "Model.h"
#include "Utility.h"

#include <string>
#include <memory>
#include <limits>
#include <map>
#include <iostream>
#include <algorithm>

using std::string;
using std::shared_ptr;
using std::numeric_limits;
using std::cout;
using std::endl;
using std::map;
using std::copy;

enum class Cruise_state {not_cruising, moving_to_destination, refueling,
    waiting_passengers, ready_to_go};

Cruise_ship::Cruise_ship(const string& name_, Point position_) :
Ship(name_, position_, 500., 15., 2., 0), cruise_state(Cruise_state::not_cruising) {}

Cruise_ship::Cruise_ship(std::istream& is): Ship(is), cruise_state((Cruise_state)read_int(is)), cruise_speed(read_double(is)) {
    string line;
    is >> line;
    if (line == "init_island") {
        init_island = read_island_ptr(is);
    }
    int list_size = read_int(is);
    while (list_size--) {
        unvisited_island.push_back(read_island_ptr(is));
    }
    
}

// perform Cruise_ship specific behavior
void Cruise_ship::update() {
    Ship::update();
    if (!can_move() && cruise_state != Cruise_state::not_cruising) {
        cancel_cruise();
    } else if (cruise_state == Cruise_state::moving_to_destination
        && !is_moving() && can_dock(get_destination_Island())) {
        dock(get_destination_Island());
        // the cruise is over
        if (get_location() == init_island->get_location() && unvisited_island.empty()) {
            cout << get_name() << " cruise is over at " << init_island->get_name() << endl;
            cruise_state = Cruise_state::not_cruising;
            init_island = nullptr;
            return;
        }
        cruise_state = Cruise_state::refueling;
    } else if (cruise_state == Cruise_state::refueling) {
        Ship::refuel();
        cruise_state = Cruise_state::waiting_passengers;
    } else if (cruise_state == Cruise_state::waiting_passengers) {
        cruise_state = Cruise_state::ready_to_go;
    } else if (cruise_state == Cruise_state::ready_to_go) {
        set_destination_island_and_speed(get_next_Island(), cruise_speed);
    }
}

// perform Cruise_ship specific behavior
void Cruise_ship::describe() const {
    cout << "\nCruise_ship ";
    Ship::describe();
    if (cruise_state == Cruise_state::not_cruising)
        return;
    else if (cruise_state == Cruise_state::moving_to_destination)
        cout << "On cruise to " << get_destination_Island()->get_name() << endl;
    else
        cout << "Waiting during cruise at " << get_destination_Island()->get_name() << endl;
}

// Cancel the current cruise and start a new cruise when arrives at island
void Cruise_ship::set_destination_island_and_speed(shared_ptr<Island> destination_island,
                                                   double speed) {
    cancel_cruise();
    Ship::set_destination_island_and_speed(destination_island, speed);
    cout << get_name() << " will visit " << get_destination_Island()->get_name() << endl;
    if (cruise_state == Cruise_state::not_cruising) {
        init_island = destination_island;
        for (auto& island : Model::get_instance().get_all_islands())
            if (island != destination_island)
                unvisited_island.push_back(island);
        cout << get_name() << " cruise will start and end at ";
        cout << destination_island->get_name() << endl;
    }
    cruise_state = Cruise_state::moving_to_destination;
    cruise_speed = speed;
}

// Cancel the current cruise and perform the ship specific behaviors
void Cruise_ship::set_destination_position_and_speed(Point destination_point,
                                                     double speed) {
    cancel_cruise();
    Ship::set_destination_position_and_speed(destination_point, speed);
}

// Cancel the current cruise and perform the ship specific behaviors
void Cruise_ship::set_course_and_speed(double course, double speed) {
    cancel_cruise();
    Ship::set_course_and_speed(course, speed);
}

// Cancel the current cruise and perform the ship specific behaviors
void Cruise_ship::stop() {
    cancel_cruise();
    Ship::stop();
}

// Save the current state to os
void Cruise_ship::save(std::ostream &os) const {
    os << "Cruise_ship" << endl;
    Ship::save(os);
    os << (int)cruise_state << " " << cruise_speed << " " << endl;
    if (init_island.use_count()) {
        os << "init_island" << endl << init_island->get_name() << endl;
    } else {
        os << "no island ptr" << endl;
    }
    os << unvisited_island.size() << endl;
    std::for_each(unvisited_island.begin(), unvisited_island.end(), [&os](const std::shared_ptr<Island>& island_ptr){os << island_ptr->get_name() << " ";});
    os << endl;
}

Cruise_ship& Cruise_ship::operator= (Cruise_ship& in_cruise) {
    Ship::operator=(in_cruise);
    cruise_state = in_cruise.cruise_state;
    cruise_speed = in_cruise.cruise_speed;
    init_island = in_cruise.init_island;
    unvisited_island = in_cruise.unvisited_island;
    return *this;
    
}


// Discard te cruise information and output a message
void Cruise_ship::cancel_cruise() {
    if (cruise_state == Cruise_state::not_cruising
        || cruise_state == Cruise_state::ready_to_go)
        return;
    cout << get_name() << " canceling current cruise" << endl;
    cruise_state = Cruise_state::not_cruising;
    init_island = nullptr;
    unvisited_island.clear();
}

/* Find the nearest unvisited island and remove it from the set.
 If all islands has been visited, return initial island. */
shared_ptr<Island> Cruise_ship::get_next_Island() {
    if (unvisited_island.empty())
        return init_island;
    auto next_island_iter = unvisited_island.begin();
    double shortest_distance = numeric_limits<double>::max();
    for (auto iter = unvisited_island.begin(); iter != unvisited_island.end(); ++iter) {
        double distance = cartesian_distance(get_location(), (*iter)->get_location());
        if (distance > shortest_distance)
            continue;
        if (distance < shortest_distance ||
            (*iter)->get_name() < (*next_island_iter)->get_name()) {
            shortest_distance = distance;
            next_island_iter = iter;
        }
    }
    shared_ptr<Island> next_island(*next_island_iter);
    unvisited_island.erase(next_island_iter);
    return next_island;
}
