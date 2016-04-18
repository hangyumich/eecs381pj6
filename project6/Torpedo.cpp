#include "Torpedo.h"
#include "Model.h"
#include "Island.h"
#include <iostream>
#include <memory>
#include <limits>

using std::cout;
using std::endl;
using std::shared_ptr;
using std::numeric_limits;

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


