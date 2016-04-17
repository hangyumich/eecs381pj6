#include "Group.h"
#include "Utility.h"
#include "Geometry.h"

#include <memory>
#include <algorithm>
#include <iostream>
#include <functional>

using std::shared_ptr;
using std::for_each;
using std::mem_fn;
using std::find;
using std::cout;
using std::endl;
using std::function;
using std::bind;
using namespace std::placeholders;

/*********************** General Functions *********************************/
void Group::add_member(shared_ptr<Commandable> new_member) {
    for (auto& member : members) {
        if (!member.expired() && member.lock() == new_member) {
            throw Error("A member with that name already exists");
        }
    }
    members.push_back(new_member);
    cout << "Member has been added to " << name << endl;
}

void Group::delete_member(std::shared_ptr<Commandable> member) {
    for (auto iter = members.begin(); iter != members.end(); ++iter) {
        if (iter->lock() == member) {
            members.erase(iter);
            cout << "Member has been removed from " << name << endl;
            return;
        }
    }
    throw Error("No member with that name exists in the group");
}


/******************** Commandable Interface Override **********************/

// All members start moving on a course and speed
void Group::set_course_and_speed(double course, double speed) {
    control_members(bind(&Commandable::set_course_and_speed, _1, course, speed));
}

// All members start moving to a destination position at a speed
void Group::set_destination_position_and_speed(Point destination_position,
                                               double speed) {
    control_members(bind(&Commandable::set_destination_position_and_speed,
                         _1, destination_position, speed));
}

// All members start moving to a destination Island at a speed
void Group::set_destination_island_and_speed(shared_ptr<Island> destination_island,
                                             double speed) {
    control_members(bind(&Commandable::set_destination_island_and_speed,
                         _1, destination_island, speed));
}

// All members stop moving
void Group::stop() {
    control_members(mem_fn(&Commandable::stop));
}

// All members dock at an Island
void Group::dock(shared_ptr<Island> island_ptr) {
    control_members(bind(&Commandable::dock, _1, island_ptr));
}

// All members refuel at the docked island
void Group::refuel() {
    control_members(mem_fn(&Commandable::refuel));
}

/* set load destination for all members. If a ship in the group does not have that
 ability, we just skip that ship */
void Group::set_load_destination(std::shared_ptr<Island> load_island) {
    control_members(bind(&Commandable::set_load_destination, _1, load_island));
}

/* set unload destination for all members. If a ship in the group does not have that
 ability, we just skip that ship */
void Group::set_unload_destination(std::shared_ptr<Island> unload_island) {
    control_members(bind(&Commandable::set_unload_destination, _1, unload_island));
}

// All members start attacking target
void Group::attack(std::shared_ptr<Ship> target_ptr) {
    control_members(bind(&Commandable::attack, _1, target_ptr));
}

// All members stop attacking target
void Group::stop_attack() {
    control_members(mem_fn(&Commandable::stop_attack));
}

/* For all members in the group, first check whether that member still exists.
 if so, let that member execute the command. Otherwise, remove that memeber
 from the group */
void Group::control_members(function<void (shared_ptr<Commandable>)> control_func) {
    for (auto iter = members.begin(); iter != members.end();) {
        try {
            auto member = iter->lock();
            if (member) {
                control_func(member);
                ++iter;
            } else {
                members.erase(iter++);
            }
        } catch (const Not_have_ability& e) { ++iter; }
    }
}