#include "Warships.h"
#include "Utility.h"
#include "Model.h"
#include "Island.h"
#include "Ship_factory.h"

#include <iostream>
#include <memory>
#include <cassert>
#include <limits>
#include <typeinfo>

using std::cout;
using std::endl;
using std::shared_ptr;
using std::numeric_limits;

/* ** Warships Implementation **
 ********************************** */


Warships::Warships(const std::string& name_, Point position_, double fuel_capacity_,
                 double maximum_speed_, double fuel_consumption_, int resistance_,
                 double firepower_, double max_attack_range_) :
Ship(name_, position_, fuel_capacity_, maximum_speed_, fuel_consumption_, resistance_),
firepower(firepower_), max_attack_range(max_attack_range_) {}

Warships::Warships(std::istream& is): Ship(is), firepower(read_double(is)), max_attack_range(read_double(is)), attacking(read_int(is)) {
    std::string str;
    is >> str;
    if (str == "target") {
        std::string ship_type, ship_name;
        is >> ship_type >> ship_name;
        try {
            target = Model::get_instance().get_ship_ptr(ship_name);
        } catch (Error& e) {
            shared_ptr<Ship> ship_ptr = create_ship(ship_name, ship_type.substr(1), Point(0, 0));
            target = ship_ptr;
            Model::get_instance().add_ship(ship_ptr);
        }
    }
}

void Warships::describe() const {
    Ship::describe();
    if (!attacking)
        return;
    if (target.expired()) {
        cout << "Attacking absent ship" << endl;
    } else {
        assert(target.lock()->is_afloat());
        cout << "Attacking " << target.lock()->get_name() << endl;
    }
}

// start an attack on a target ship
void Warships::attack(shared_ptr<Ship> target_ptr_) {
    if (!is_afloat())
        throw Error("Cannot attack!");
    if (target_ptr_.get() == this)
        throw Error("Cannot attack itself!");
    if (target_ptr_ == target.lock())
        throw Error("Already attacking this target!");
    target = target_ptr_;
    attacking = true;
    cout << get_name() << " will attack " << target.lock()->get_name() << endl;
}

// stop attacking and discard the target pointer
void Warships::stop_attack() {
    if (!attacking)
        throw Error("Was not attacking!");
    attacking = false;
    target.reset();
    cout << get_name() << " stopping attack" << endl;
}

void Warships::update() {
    Ship::update();
    if (!attacking)
        return;
    
    shared_ptr<Ship> target_ptr = target.lock();
    if (!target_ptr) {
        stop_attack();
    } else {
        cout << get_name() << " is attacking" << endl;
        if (cartesian_distance(get_location(), target_ptr->get_location()) <= max_attack_range) {
            cout << get_name() << " fires" << endl;
            target_ptr->receive_hit(firepower, shared_from_this());
        } else {
            target_out_of_range(target_ptr);
        }
    }
    
}

void Warships::save(std::ostream& os) const {
    Ship::save(os);
    os << firepower << " " << max_attack_range << " " << attacking << endl;
    if (target.expired()) {
        os << "no_taget" << endl;
    }else{
        os << "target " << typeid(*target.lock()).name()
        <<" "<<target.lock()->get_name() << endl;
    }
}

Warships& Warships::operator= (const Warships& in_warship) {
    Ship::operator=(in_warship);
    firepower = in_warship.firepower;
    max_attack_range = in_warship.max_attack_range;
    target = in_warship.target;
    attacking = in_warship.attacking;
    return *this;
}

/* ** Cruiser Ship Implementation **
 ********************************** */

Cruiser::Cruiser(const std::string& name_, Point position_) :
Warships(name_, position_, 1000., 20., 10., 6., 3, 15)
{}

Cruiser::Cruiser(std::istream& is): Warships(is) {}

void Cruiser::describe() const {
    cout << "\nCruiser ";
    Warships::describe();
}

// When target is out of range, cruiser will stop attacking.
void Cruiser::target_out_of_range(shared_ptr<Ship> target) {
    cout << get_name() << " target is out of range" << endl;
    stop_attack();
}

// Will counter-attack if received hit
void Cruiser::receive_hit(int hit_force, shared_ptr<Ship> attacker_ptr) {
    Ship::receive_hit(hit_force, attacker_ptr);
    if (is_afloat() && !is_attacking())
        attack(attacker_ptr);
}


void Cruiser::save(std::ostream& os) const {
    os << "Cruiser" << endl;
    Warships::save(os);
}


/* ** Torpedo Ship Implementation **
 ********************************** */

Torpedo_boat::Torpedo_boat(const std::string& name_, Point position_) :
Warships(name_, position_, 800., 12., 5., 9., 3, 5)
{}

void Torpedo_boat::describe() const {
    cout << "\nTorpedo_boat ";
    Warships::describe();
}

Torpedo_boat::Torpedo_boat(std::istream& is): Warships(is) {}

// Torpedo boat will keep chasing target if target is out of range
void Torpedo_boat::target_out_of_range(shared_ptr<Ship> target) {
    if (can_move())
        set_destination_position_and_speed(target->get_location(), get_maximum_speed());
}

void Torpedo_boat::receive_hit(int hit_force, shared_ptr<Ship> attacker_ptr) {
    Ship::receive_hit(hit_force, attacker_ptr);
    if (!can_move())
        return;
    cout << get_name() << " taking evasive action" << endl;
    if (is_attacking())
        stop_attack();
    set_destination_island_and_speed(find_refuge_island(attacker_ptr), get_maximum_speed());
}

void Torpedo_boat::save(std::ostream& os)const {
    os << "Torpedo_boat" << endl;
    Warships::save(os);
}

/* Find the nearest island to the attacker with range more than 15 nm. If not found
 return the furthest island from attacker */
shared_ptr<Island> Torpedo_boat::find_refuge_island(shared_ptr<Ship> attacker) {
    shared_ptr<Island> refuge_island;
    
    // find the nearest island with distance >= 15nm
    double shortest_distance = numeric_limits<double>::max();
    for (const auto& island : Model::get_instance().get_all_islands()) {
        double distance = cartesian_distance(island->get_location(), attacker->get_location());
        if (distance < 15 || distance > shortest_distance)
            continue;
        if (distance < shortest_distance || island->get_name() < refuge_island->get_name()) {
            refuge_island = island;
            shortest_distance = distance;
        }
    }
    
    if (refuge_island)
        return refuge_island;
    
    // find the furest island from attacker
    double longest_distance = -1;
    for (const auto& island : Model::get_instance().get_all_islands()) {
        double distance = cartesian_distance(island->get_location(), attacker->get_location());
        if (distance < longest_distance)
            continue;
        if (distance > longest_distance || island->get_name() < refuge_island->get_name()) {
            refuge_island = island;
            longest_distance = distance;
        }
    }
    return refuge_island;
}


