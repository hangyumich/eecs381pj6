#include "Warship.h"
#include "Utility.h"
#include "Model.h"
#include "Ship_factory.h"
#include <iostream>
#include <memory>
#include <cassert>

using std::cout;
using std::endl;
using std::shared_ptr;

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
