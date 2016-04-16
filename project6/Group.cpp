#include "Group.h"
#include "Utility.h"
#include "Geometry.h"

#include <memory>
#include <algorithm>
#include <functional>

using std::shared_ptr;
using std::for_each;
using std::mem_fn;
using std::find;

/*********************** General Functions *********************************/
void Group::add_member(shared_ptr<Commandable> member) {
    if (find(members.begin(), members.end(), member) != members.end())
        throw Error("A member with that name already exists");
    members.push_back(member);
}

void Group::delete_member(std::shared_ptr<Commandable> member) {
    auto iter = find(members.begin(), members.end(), member);
    if (iter == members.end())
        throw Error("No member with that name exists in the group");
    members.erase(iter);
}


/******************** Commandable Interface Override **********************/

// All members start moving on a course and speed
void Group::set_course_and_speed(double course, double speed) {
    for (auto& member : members)
        member->set_course_and_speed(course, speed);
}

// All members start moving to a destination position at a speed
void Group::set_destination_position_and_speed(Point destination_position,
                                               double speed) {
    for (auto& member : members)
        member->set_destination_position_and_speed(destination_position, speed);
}

// All members start moving to a destination Island at a speed
void Group::set_destination_island_and_speed(shared_ptr<Island> destination_island,
                                             double speed) {
    for (auto& member : members)
        member->set_destination_island_and_speed(destination_island, speed);
}

// All members stop moving
void Group::stop() {
    for_each(members.begin(), members.end(), mem_fn(&Commandable::stop));
}

// All members dock at an Island
void Group::dock(shared_ptr<Island> island_ptr) {
    for (auto& member : members)
        member->dock(island_ptr);
}

// All members refuel at the docked island
void Group::refuel() {
    for_each(members.begin(), members.end(), mem_fn(&Commandable::refuel));
}

/* set load destination for all members. If a ship in the group does not have that
 ability, we just skip that ship */
void Group::set_load_destination(std::shared_ptr<Island> load_island) {
    for (auto& member : members) {
        try {
            member->set_load_destination(load_island);
        } catch (const Not_have_ability& e) {}
    }
}

/* set unload destination for all members. If a ship in the group does not have that
 ability, we just skip that ship */
void Group::set_unload_destination(std::shared_ptr<Island> unload_island) {
    for (auto& member : members) {
        try {
            member->set_unload_destination(unload_island);
        } catch (const Not_have_ability& e) {}
    }
}

// All members start attacking target
void Group::attack(std::shared_ptr<Ship> target_ptr) {
    for (auto& member : members) {
        try {
            member->attack(target_ptr);
        } catch (const Not_have_ability& e) {}
    }
}

// All members stop attacking target
void Group::stop_attack() {
    for (auto& member : members) {
        try {
            member->stop_attack();
        } catch (const Not_have_ability& e) {}
    }
}