#include "Model.h"
#include "View.h"
#include "Island.h"
#include "Ship.h"
#include "Ship_factory.h"
#include "Utility.h"
#include "Group.h"

#include <type_traits>
#include <algorithm>
#include <iostream>
#include <string>
#include <map>
#include <memory>

using std::string;
using std::copy;
using std::cout;
using std::endl;
using std::shared_ptr;
using std::make_shared;
using std::map;
using std::true_type;

/*************************** General Functions ****************************/

// create the initial objects, output constructor message
Model::Model() : time(0) {
    islands.insert(make_shared<Island>("Exxon", Point(10, 10), 1000, 200));
    islands.insert(make_shared<Island>("Shell", Point(0, 30), 1000, 200));
    islands.insert(make_shared<Island>("Bermuda", Point(20, 20)));
    islands.insert(make_shared<Island>("Treasure_Island", Point(50, 5), 100, 5));
    for (auto& island : islands)
        objects[island->get_name()] = island;
    
    ships.insert(create_ship("Ajax", "Cruiser", Point (15, 15)));
    ships.insert(create_ship("Xerxes", "Cruiser", Point (25, 25)));
    ships.insert(create_ship("Valdez", "Tanker", Point (30, 30)));
    for (auto& ship : ships)
        objects[ship->get_name()] = ship;
}

// get the singleton model object
Model& Model::get_instance() {
    static Model model;
    return model;
}


/*********************** Ship and Islands Functions **********************/

/* is name already in use for either ship or island?
 either the identical name, or identical in first two characters counts as in-use */
bool Model::is_name_in_use(const string& name) const {
    // Use first two characters to test lower bound
    auto lower_bound = objects.lower_bound(name.substr(0, 2));
    if (lower_bound == objects.end())
        return false;
    return (lower_bound->first.substr(0, 2) == name.substr(0, 2)) ||
           (objects.find(name) != objects.end());
}

// is there such an island?
bool Model::is_island_present(const string& name) const {
    return islands.find(name) != islands.end();
}

// will throw Error("Island not found!") if no island of that name
shared_ptr<Island> Model::get_island_ptr(const string& name) const {
    auto iter = islands.find(name);
    if (iter == islands.end())
        throw Error("Island not found!");
    return *iter;
}

// is there such an ship?
bool Model::is_ship_present(const string& name) const {
    return ships.find(name) != ships.end();
}

// add a new ship to the list, and update the view
void Model::add_ship(shared_ptr<Ship> ship) {
    objects[ship->get_name()] = ship;
    ships.insert(ship);
    ship->broadcast_current_state();
}

// will throw Error("Ship not found!") if no ship of that name
shared_ptr<Ship> Model::get_ship_ptr(const std::string& name) const {
    auto iter = ships.find(name);
    if (iter == ships.end())
        throw Error("Ship not found!");
    return *iter;
}

// tell all objects to describe themselves
void Model::describe() const {
    for (const auto& object : objects)
        object.second->describe();
}

// increment the time, and tell all objects to update themselves
void Model::update() {
    ++time;
    for (const auto& object : objects)
        object.second->update();
}

/************************** Group Functions *******************************/

/* Test whether the group name has already been used by other ships, islands
 or other groups. */
bool Model::is_group_name_valid(const string& group_name) const {
    return objects.find(group_name) == objects.end() &&
           groups.find(group_name) == groups.end();
    
}

// Test wheter a group exists with given name.
bool Model::is_group_present(const std::string& group_name) const {
    return groups.find(group_name) != groups.end();
}

shared_ptr<Group> Model::get_group_ptr(const string& group_name) const {
    auto iter = groups.find(group_name);
    if (iter == groups.end())
        throw Error("Group not found!");
    return iter->second;
}

void Model::attach_group(shared_ptr<Group> group_ptr) {
    groups[group_ptr->get_name()] = group_ptr;
}

void Model::detach_group(shared_ptr<Group> group_ptr) {
    groups.erase(group_ptr->get_name());
}



/****************************** View Functions *****************************/

/* Attaching a View adds it to the container and causes it to be updated
 with all current objects'location (or other state information. */
void Model::attach(shared_ptr<View> view) {
    views.push_front(view);
    for (const auto& object : objects)
        object.second->broadcast_current_state();
}

/* Detach the View by discarding the supplied pointer from the container of Views
 - no updates sent to it thereafter. */
void Model::detach(shared_ptr<View> detach_view) {
    views.remove(detach_view);
}

// remove the Ship from the containers.
void Model::remove_ship(shared_ptr<Ship> ship_ptr) {
    objects.erase(ship_ptr->get_name());
    ships.erase(ship_ptr);
}

// notify the views about an object's location
void Model::notify_location(const string& name, Point location) {
    for (auto& view : views)
        view->update_location(name, location);
}

// notify the views that an object is now gone
void Model::notify_gone(const string& name) {
    for (auto& view : views)
        view->update_remove(name);
}

// notify the views about an object's fuel
void Model::notify_fuel(const string& name, double fuel) {
    for (auto& view : views)
        view->update_fuel(name, fuel);
}

// notify the views about an object's course
void Model::notify_course(const string& name, double course) {
    for (auto& view : views)
        view->update_course(name, course);
}

// notify the views about an object's speed
void Model::notify_speed(const std::string& name, double speed) {
    for (auto& view : views)
        view->update_speed(name, speed);
}

