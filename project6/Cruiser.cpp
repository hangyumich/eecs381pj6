#include "Cruiser.h"
#include <iostream>
#include <memory>

using std::cout;
using std::endl;
using std::shared_ptr;

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
