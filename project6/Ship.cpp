#include "Ship.h"
#include "Geometry.h"
#include "Island.h"
#include "Model.h"
#include "Utility.h"

#include <iostream>
#include <string>
#include <memory>

using std::string;
using std::cout;
using std::endl;
using std::shared_ptr;

/*
Define the destructor function even if it was declared as a pure virtual function.
This seems odd, because pure virtual functions are usually not defined in the class
that declares them. But this is often done as a way to make a class abstract, 
if there is no other virtual function that makes sense to mark as pure. Here we
are defining it just to get the destructor message output.
*/

const char* const ship_cannot_move_c = "Ship cannot move!";

enum class Ship_state {moving_to_position, docked, stopped, moving_on_course,
    dead_in_the_water, moving_to_island, sunk};

// initialize, then output constructor message
Ship::Ship(const string& name_, Point position_, double fuel_capacity_,
           double maximum_speed_, double fuel_consumption_, int resistance_) :
Sim_object(name_), track_base(position_), fuel(fuel_capacity_),
fuel_consumption(fuel_consumption_), fuel_capacity(fuel_capacity_),
maximum_speed(maximum_speed_), resistance(resistance_), ship_state(Ship_state::stopped) {}

Ship::Ship(std::istream& is): Sim_object(is), track_base(read_point(is), Course_speed(read_double(is), read_double(is)), read_double(is)), fuel(read_double(is)),fuel_consumption(read_double(is)), fuel_capacity(read_double(is)), maximum_speed(read_double(is)), resistance(read_double(is)), ship_state((Ship_state)read_int(is)),destination_point(read_point(is)) {
    string line;
    is >> line;
    if (line == "destination_island") {
        destination_Island = read_island_ptr(is);
    }
    is >> line;
    if (line == "docked_island") {
        docked_Island = read_island_ptr(is);
    }
}

/*** Readers ***/

// Return true if ship can move (it is not dead in the water or in the process or sinking);
bool Ship::can_move() const {
    return (is_afloat() && ship_state != Ship_state::dead_in_the_water);
}

// Return true if ship is moving
bool Ship::is_moving() const {
    return (ship_state == Ship_state::moving_on_course ||
            ship_state == Ship_state::moving_to_island ||
            ship_state == Ship_state::moving_to_position);
}

// Return true if ship is docked
bool Ship::is_docked() const {
    return ship_state == Ship_state::docked;
}

// Return true if ship is afloat (not in process of sinking), false if not
bool Ship::is_afloat() const {
    return ship_state != Ship_state::sunk;
}

/* Return true if the ship is Stopped and the distance to the supplied island 
 is less than or equal to 0.1 nm */
bool Ship::can_dock(shared_ptr<Island> island_ptr) const {
    return ship_state == Ship_state::stopped &&
           cartesian_distance(get_location(), island_ptr->get_location()) <= 0.1;
}

// Receives fuel of at most received amount. Returns actual amount used.
double Ship::receive_fuel(double received) {
    double actual = (fuel + received > fuel_capacity) ? (fuel_capacity - fuel) : (received);
    fuel += actual;
    Model::get_instance().notify_fuel(get_name(), fuel);
    ship_state = Ship_state::stopped;
    return actual;
}

/*** Interface to derived classes ***/

// output a description of current state to cout
void Ship::describe() const {
    cout << get_name() << " at " << get_location();
    if (ship_state == Ship_state::sunk) {
        cout << get_name() << " sunk" << endl;
    } else {
        cout << ", fuel: " << fuel << " tons, resistance: " << resistance << endl;
        if (ship_state == Ship_state::moving_to_position)
            cout << "Moving to " << destination_point << " on "
                 << track_base.get_course_speed() << endl;
        else if (ship_state == Ship_state::moving_to_island)
            cout << "Moving to " << destination_Island->get_name() << " on "
                 << track_base.get_course_speed() << endl;
        else if (ship_state == Ship_state::moving_on_course)
            cout << "Moving on " << track_base.get_course_speed() << endl;
        else if (ship_state == Ship_state::docked)
            cout << "Docked at " << destination_Island->get_name() << endl;
        else if (ship_state == Ship_state::stopped)
            cout << "Stopped" << endl;
        else if (ship_state == Ship_state::dead_in_the_water)
            cout << "Dead in the water" << endl;
    }
}

void Ship::broadcast_current_state() const {
    Model::get_instance().notify_location(get_name(), get_location());
    Model::get_instance().notify_fuel(get_name(), fuel);
    Model::get_instance().notify_course(get_name(), track_base.get_course());
    Model::get_instance().notify_speed(get_name(), track_base.get_speed());
}


/* Start moving to a destination position at a speed */
void Ship::set_destination_position_and_speed(Point destination_position, double speed) {
    set_course_speed_and_dest(destination_position, speed);
    docked_Island = nullptr;
    destination_Island = nullptr;
    ship_state = Ship_state::moving_to_position;
    cout << get_name() << " will sail on " << track_base.get_course_speed()
         << " to " << destination_point << endl;
}

/* Start moving to a destination Island at a speed */
void Ship::set_destination_island_and_speed(shared_ptr<Island> destination_island, double speed) {
    set_course_speed_and_dest(destination_island->get_location(), speed);
    docked_Island = nullptr;
    destination_Island = destination_island;
    ship_state = Ship_state::moving_to_island;
    cout << get_name() << " will sail on " << track_base.get_course_speed()
         << " to " << destination_island->get_name() << endl;
}

/* Check if ship can move and if speed is too large. Then set the course, speed,
 and destination point */
void Ship::set_course_speed_and_dest(Point destination_position, double speed) {
    check_state_and_speed(speed);
    track_base.set_course(Compass_vector(get_location(), destination_position).direction);
    Model::get_instance().notify_course(get_name(), track_base.get_course());
    track_base.set_speed(speed);
    Model::get_instance().notify_speed(get_name(), track_base.get_speed());
    destination_point = destination_position;
}

/* Start moving on a course and speed */
void Ship::set_course_and_speed(double course, double speed) {
    check_state_and_speed(speed);
    track_base.set_course_speed(Course_speed(course, speed));
    Model::get_instance().notify_course(get_name(), track_base.get_course());
    Model::get_instance().notify_speed(get_name(), track_base.get_speed());
    docked_Island = nullptr;
    destination_Island = nullptr;
    ship_state = Ship_state::moving_on_course;
    cout << get_name() << " will sail on " << track_base.get_course_speed() << endl;
}

/* Check if ship can move and if speed is too large */
void Ship::check_state_and_speed(double speed) {
    if (!can_move())
        throw Error(ship_cannot_move_c);
    if (speed > maximum_speed)
        throw Error("Ship cannot go that fast!");
}

/* Stop Moving */
void Ship::stop() {
    if (!can_move())
        throw Error(ship_cannot_move_c);
    track_base.set_speed(0.);
    Model::get_instance().notify_speed(get_name(), track_base.get_speed());
    cout << get_name() << " stopping at " << get_location() << endl;
    ship_state = Ship_state::stopped;
}

/* dock at an Island - set our position = Island's position, go into Docked state */
void Ship::dock(shared_ptr<Island> island_ptr) {
    if (ship_state != Ship_state::stopped ||
        cartesian_distance(get_location(), island_ptr->get_location()) > 0.1) {
        throw Error("Can't dock!");
    }
    track_base.set_position(island_ptr->get_location());
    docked_Island = island_ptr;
    Model::get_instance().notify_location(get_name(), get_location());
    ship_state = Ship_state::docked;
    cout << get_name() <<  " docked at " << island_ptr->get_name() << endl;
}

/* Refuel - must already be docked at an island; fill takes as much as possible */
void Ship::refuel() {
    if (!is_docked())
        throw Error("Must be docked!");
    double fuel_needed = fuel_capacity - fuel;
    if (fuel_needed < 0.005) {
        fuel = fuel_capacity;
    } else {
        fuel += docked_Island->provide_fuel(fuel_needed);
        cout << get_name() << " now has " << fuel << " tons of fuel" << endl;
    }
    Model::get_instance().notify_fuel(get_name(), fuel);
}

/*** Fat interface command functions ***/
void Ship::set_load_destination(shared_ptr<Island> island_ptr) {
    throw Not_have_ability("Cannot load at a destination!");
}

void Ship::set_unload_destination(shared_ptr<Island>) {
    throw Not_have_ability("Cannot unload at a destination!");
}

void Ship::attack(shared_ptr<Ship> in_target_ptr) {
    throw Not_have_ability("Cannot attack!");
}

void Ship::stop_attack() {
    throw Not_have_ability("Cannot attack!");
}

void Ship::save(std::ostream& os) const {
    Sim_object::save(os);
    os << track_base.get_position() << endl;
    os << track_base.get_course() << " " << track_base.get_speed() << " " << track_base.get_altitude() << endl;
    os << fuel << " " << fuel_consumption << " " << fuel_capacity << " " << maximum_speed << " " << resistance << " " << (int)ship_state << endl;
    os << destination_point << " " << endl;
    if (destination_Island.use_count()) {
        os << "destination_island " << destination_Island->get_name() << endl;
    } else {
        os << "no_destination_island" << endl;
    }
    if (docked_Island.use_count()) {
        os << "docked_island " << docked_Island->get_name() << endl;
    } else {
        os << "no_docked_island" << endl;
    }
}

// copy assignment
Ship& Ship::operator= (const Ship& in_ship) {
    if(&in_ship == this)
        return *this;
    track_base = in_ship.track_base;
    fuel = in_ship.fuel;
    fuel_consumption = in_ship.fuel_consumption;
    fuel_capacity = in_ship.fuel_capacity;
    maximum_speed = in_ship.maximum_speed;
    resistance = in_ship.resistance;
    ship_state = in_ship.ship_state;
    destination_point = in_ship.destination_point;
    destination_Island = in_ship.destination_Island;
    docked_Island = in_ship.docked_Island;
    return *this;
}


/* interactions with other objects. Receive a hit from an attacker */
void Ship::receive_hit(int hit_force, shared_ptr<Ship> attacker_ptr) {
    resistance -= hit_force;
    cout << get_name() << " hit with " << hit_force
         << ", resistance now " << resistance << endl;
    if (resistance < 0) {
        cout << get_name() << " sunk" << endl;
        ship_state = Ship_state::sunk;
        track_base.set_speed(0.);
        Model::get_instance().notify_speed(get_name(), track_base.get_speed());
        Model::get_instance().notify_gone(get_name());
        Model::get_instance().remove_ship(shared_from_this());
    }
}

// Update the state of the Ship
void Ship::update() {
    // Afloat states
    if (is_afloat()) {
        if (is_moving()) {
            calculate_movement();
            cout << get_name() << " now at " << get_location() << endl;
            Model::get_instance().notify_location(get_name(), get_location());
        } else if (ship_state == Ship_state::stopped) {
            cout << get_name() << " stopped at " << get_location() << endl;
        } else if (is_docked()) {
            cout << get_name() << " docked at " << destination_Island->get_name() << endl;
        } else if (ship_state == Ship_state::dead_in_the_water) {
            cout << get_name() << " dead in the water at " << get_location() << endl;
        }
    } else {
        cout << get_name() << " sunk" << endl;
    }
}



/*
Calculate the new position of a ship based on how it is moving, its speed, and
fuel state. This function should be called only if the state is 
moving_to_position, moving_to_island, or moving_on_course.

Track_base has an update_position(double time) function that computes the new position
of an object after the specified time has elapsed. If the Ship is going to move
for a full time unit (one hour), then it will get go the "full step" distance,
so update_position would be called with time = 1.0. If we can move less than that,
e.g. due to not enough fuel, update position  will be called with the corresponding
time less than 1.0.

For clarity in specifying the computation, this code assumes the specified private variable names, 
but you may change the variable names or enum class names, or state names if you wish (e.g. movement_state).
*/
void Ship:: calculate_movement()
{
	// Compute values for how much we need to move, and how much we can, and how long we can,
	// given the fuel state, then decide what to do.
	double time = 1.0;	// "full step" time
	// get the distance to destination
	double destination_distance = cartesian_distance(get_location(), destination_point);
	// get full step distance we can move on this time step
	double full_distance = track_base.get_speed() * time;
	// get fuel required for full step distance
	double full_fuel_required = full_distance * fuel_consumption;	// tons = nm * tons/nm
	// how far and how long can we sail in this time period based on the fuel state?
	double distance_possible, time_possible;
	if(full_fuel_required <= fuel) {
		distance_possible = full_distance;
		time_possible = time;
		}
	else {
		distance_possible = fuel / fuel_consumption;	// nm = tons / tons/nm
		time_possible = (distance_possible / full_distance) * time;
		}
	
	// are we are moving to a destination, and is the destination within the distance possible?
	if((ship_state == Ship_state::moving_to_position || ship_state== Ship_state::moving_to_island)
       && destination_distance <= distance_possible) {
		// yes, make our new position the destination
		track_base.set_position(destination_point);
        
		// we travel the destination distance, using that much fuel
		double fuel_required = destination_distance * fuel_consumption;
		fuel -= fuel_required;
		track_base.set_speed(0.);
        Model::get_instance().notify_speed(get_name(), track_base.get_speed());
		ship_state = Ship_state::stopped;
    } else {
		// go as far as we can, stay in the same movement state
		// simply move for the amount of time possible
		track_base.update_position(time_possible);
		// have we used up our fuel?
		if(full_fuel_required >= fuel) {
			fuel = 0.0;
            track_base.set_speed(0.);
            Model::get_instance().notify_speed(get_name(), track_base.get_speed());
			ship_state = Ship_state::dead_in_the_water;
			}
		else {
			fuel -= full_fuel_required;
        }
    }
    Model::get_instance().notify_fuel(get_name(), fuel);
}

