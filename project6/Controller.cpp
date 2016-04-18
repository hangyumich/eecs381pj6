#include "Controller.h"
#include "Geometry.h"
#include "Ship_factory.h"
#include "View.h"
#include "Utility.h"
#include "Model.h"
#include "Ship.h"
#include "Island.h"
#include "Views.h"
#include "Group.h"

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <memory>

using std::string;
using std::cout;
using std::cin;
using std::endl;
using std::map;
using std::exception;
using std::shared_ptr;
using std::make_shared;
using namespace std::placeholders;

// create View object, run the program by acccepting user commands, then destroy View object
void Controller::run() {
    map<string, void (Controller::*)(shared_ptr<Commandable>)> control_cmds {
        {"course", &Controller::course_cmd},
        {"position", &Controller::position_cmd},
        {"destination", &Controller::destination_cmd},
        {"load_at", &Controller::load_at_cmd},
        {"unload_at", &Controller::unload_at_cmd},
        {"dock_at", &Controller::dock_at_cmd},
        {"attack", &Controller::attack_cmd},
        {"refuel", &Controller::refuel_cmd},
        {"stop", &Controller::stop_cmd},
        {"stop_attack", &Controller::stop_attack_cmd}
    };
    
    map<string, void (Controller::*)()> cmds {
        {"default", &Controller::default_cmd},
        {"size", &Controller::size_cmd},
        {"zoom", &Controller::zoom_cmd},
        {"pan", &Controller::pan_cmd},
        {"show", &Controller::show_cmd},
        {"status", &Controller::status_cmd},
        {"go", &Controller::go_cmd},
        {"create", &Controller::create_cmd},
        {"save", &Controller::save_cmd},
        {"restore", &Controller::restore_cmd},
        {"create_group", &Controller::create_group_cmd},
        {"delete_group", &Controller::delete_group_cmd},
        {"add_member", &Controller::add_member_cmd},
        {"delete_member", &Controller::delete_member_cmd},
        {"open_map_view", &Controller::open_map_view_cmd},
        {"close_map_view", &Controller::close_map_view_cmd},
        {"open_sailing_view", &Controller::open_sailing_view},
        {"close_sailing_view", &Controller::close_sailing_view},
        {"open_bridge_view", &Controller::open_bridge_view},
        {"close_bridge_view", &Controller::close_bridge_view},
        {"open_gps_view", &Controller::open_gps_view},
        {"close_gps_view", &Controller::close_gps_view},
        {"gps_default", &Controller::default_gps_cmd},
        {"gps_size", &Controller::size_gps_cmd},
        {"gps_zoom", &Controller::zoom_gps_cmd}
    };
    
    while (true) {
        cout << "\nTime " << Model::get_instance().get_time();
        cout << ": Enter command: ";
        try {
            string first_word;
            cin >> first_word;
            
            if (first_word == "quit") {
                quit_cmd();
                return;
            }
            if (Model::get_instance().is_ship_present(first_word) ||
                Model::get_instance().is_group_present(first_word)) {
                string cmd_word;
                cin >> cmd_word;
                auto commandable_ptr = get_commandable_object(first_word);
                (this->*get_func_ptr(control_cmds, cmd_word))(commandable_ptr);
            } else {
                (this->*get_func_ptr(cmds, first_word))();
            }
        } catch (Error& error) {
            cout << error.what() << endl;
            cin.clear();
            while (cin.get() != '\n');
        } catch (exception& e) {
            cout << e.what() << endl;
            quit_cmd();
            return;
        }
    }
}

// Given a cmd string, return the corresponding function ptr
template<typename T>
T Controller::get_func_ptr(map<string, T> cmds, const string& cmd_word) {
    auto iter = cmds.find(cmd_word);
    if (iter == cmds.end())
        throw Error("Unrecognized command!");
    return iter->second;
}

void Controller::quit_cmd() {
    cout << "Done" << endl;
}

/* View Commands */

void Controller::default_cmd() {
    check_map_is_open();
    map_view->set_defaults();
}

void Controller::size_cmd() {
    check_map_is_open();
    int new_size;
    if(!(cin >> new_size))
        throw Error("Expected an integer!");
    map_view->set_size(new_size);
}

void Controller::zoom_cmd() {
    check_map_is_open();
    double scale = read_double();
    map_view->set_scale(scale);
}

void Controller::pan_cmd() {
    check_map_is_open();
    Point origin{read_double(), read_double()};
    map_view->set_origin(origin);
}

void Controller::show_cmd() {
    for (const auto& view : Model::get_instance().get_views())
        view->draw();
}

void Controller::open_map_view_cmd() {
    if (map_view != nullptr)
        throw Error("Map view is already open!");
    map_view = make_shared<Map_view>();
    Model::get_instance().attach(map_view);
}

void Controller::close_map_view_cmd() {
    check_map_is_open();
    Model::get_instance().detach(map_view);
    map_view = nullptr;
}

// throw an error if map is not open
void Controller::check_map_is_open() {
    if (map_view == nullptr)
        throw Error("Map view is not open!");
}

void Controller::open_sailing_view() {
    if (sailing_view != nullptr)
        throw Error("Sailing data view is already open!");
    sailing_view = make_shared<Sailing_view>();
    Model::get_instance().attach(sailing_view);
}

void Controller::close_sailing_view() {
    if (sailing_view == nullptr)
        throw Error("Sailing data view is not open!");
    Model::get_instance().detach(sailing_view);
    sailing_view = nullptr;
}

void Controller::open_bridge_view() {
    string ship_name = read_string();
    auto ship_ptr = Model::get_instance().get_ship_ptr(ship_name);
    if (bridge_views.find(ship_name) != bridge_views.end())
        throw Error("Bridge view is already open for that ship!");
    shared_ptr<Bridge_view> bridge_view = make_shared<Bridge_view>(ship_name);
    bridge_views[ship_name] = bridge_view;
    Model::get_instance().attach(bridge_view);
}

void Controller::close_bridge_view() {
    string ship_name = read_string();
    auto iter = bridge_views.find(ship_name);
    if (iter == bridge_views.end())
        throw Error("Bridge view for that ship is not open!");
    Model::get_instance().detach(iter->second);
    bridge_views.erase(iter);
}

void Controller::open_gps_view() {
    string ship_name;
    cin >> ship_name;
    auto ship_ptr = Model::get_instance().get_ship_ptr(ship_name);
    if (gps_views.find(ship_name) != gps_views.end())
        throw Error("GPS view is already open for that ship!");
    shared_ptr<GPS_view> gps_view = make_shared<GPS_view>(ship_name);
    gps_views[ship_name] = gps_view;
    Model::get_instance().attach(gps_view);
}

void Controller::close_gps_view() {
    string ship_name;
    cin >> ship_name;
    auto iter = gps_views.find(ship_name);
    if (iter == gps_views.end())
        throw Error("GPS view for that ship is not open!");
    Model::get_instance().detach(iter->second);
    gps_views.erase(iter);
}

void Controller::default_gps_cmd() {
    get_open_gps_map()->set_defaults();
}

void Controller::size_gps_cmd() {
    shared_ptr<GPS_view> gps_view = get_open_gps_map();
    int new_size;
    if(!(cin >> new_size))
        throw Error("Expected an integer!");
    gps_view->set_size(new_size);
}

void Controller::zoom_gps_cmd() {
    shared_ptr<GPS_view> gps_view = get_open_gps_map();
    double scale = read_double();
    gps_view->set_scale(scale);
}

// throw an error if map is not open
shared_ptr<GPS_view> Controller::get_open_gps_map() {
    string ship_name;
    cin >> ship_name;
    auto iter = gps_views.find(ship_name);
    if (iter == gps_views.end())
        throw Error("GPS view for that ship is not open!");
    return iter->second;
}

/* Model Commands */

void Controller::status_cmd() {
    Model::get_instance().describe();
}

void Controller::go_cmd() {
    Model::get_instance().update();
}

void Controller::create_cmd() {
    string ship_name = read_string();
    if (ship_name.length() < 2)
        throw Error("Name is too short!");
    if (Model::get_instance().is_name_in_use(ship_name))
        throw Error("Name is invalid!");
    string ship_type = read_string();
    Point init_position{read_double(), read_double()};
    Model::get_instance().add_ship(create_ship(ship_name, ship_type, init_position));
}

// open the file and return fstream
template <typename T>
T Controller::read_open_file(std::istream& is) {
    string file_name;
    is >> file_name;
    T fs;
    fs.open(file_name);
    if (!fs)
    {
        throw Error("Cannot open file");
    }
    return fs;
}

void Controller::save_cmd() {
    std::ofstream os = read_open_file<std::ofstream>(cin);
    os.precision(10);
    std::list<std::shared_ptr<View>> views = Model::get_instance().get_views();
    os << views.size() << endl;
    std::for_each(views.begin(), views.end(), std::bind(&View::save, _1, std::ref(os)));
    Model::get_instance().save(os);
    os.close();
}


void Controller::restore_cmd() {
    map_view.reset();
    sailing_view.reset();
    bridge_views.clear();
    Model::get_instance().reset();
    try {
        std::ifstream is = read_open_file<std::ifstream>(cin);
        int views_size = read_int(is);
        while (views_size --) {
            string view_type;
            is >> view_type;
            if (view_type == "Map_view") {
                map_view = make_shared<Map_view>(Map_view(is));
                Model::get_instance().attach(map_view);
            } else if (view_type == "Bridge_view") {
                shared_ptr<Bridge_view> bridge_view = make_shared<Bridge_view>(Bridge_view(is));
                bridge_views[bridge_view->get_ship_name()] = bridge_view;
                Model::get_instance().attach(bridge_view);
            } else if (view_type == "Sailing_view") {
                sailing_view = make_shared<Sailing_view>(Sailing_view(is));
                Model::get_instance().attach(sailing_view);
            } else {
                throw Error("Unknow view type");
            }
        }
        Model::get_instance().restore(is);
        is.close();
    } catch (...) {
        throw Error("Invalid data found in file.");
    }
}


/* Group Commands */
void Controller::create_group_cmd() {
    string group_name = read_string();
    if (!Model::get_instance().is_group_name_valid(group_name))
        throw Error("Group name is invalid!");
    Model::get_instance().attach_group(make_shared<Group>(group_name));
    cout << "Group " << group_name << " created" << endl;
}

void Controller::delete_group_cmd() {
    string group_name = read_string();
    auto group_ptr = Model::get_instance().get_group_ptr(group_name);
    Model::get_instance().detach_group(group_ptr);
    cout << "Group " << group_name << " deleted" << endl;
}

void Controller::add_member_cmd() {
    shared_ptr<Commandable> member = get_commandable_object(read_string());
    shared_ptr<Group> group_ptr = Model::get_instance().get_group_ptr(read_string());
    group_ptr->add_member(member);
}

void Controller::delete_member_cmd() {
    shared_ptr<Commandable> member = get_commandable_object(read_string());
    shared_ptr<Group> group_ptr = Model::get_instance().get_group_ptr(read_string());
    group_ptr->delete_member(member);
}

/* Read a string from cin and get either a ship pointer or a group ptr.
 Throw an error otherwise. */
shared_ptr<Commandable> Controller::get_commandable_object(string name) {
    if (Model::get_instance().is_ship_present(name))
        return Model::get_instance().get_ship_ptr(name);
    else if (Model::get_instance().is_group_present(name))
        return Model::get_instance().get_group_ptr(name);
    else
        throw Error("Commandable object not found!");
}

/* Control Commands */

void Controller::course_cmd(shared_ptr<Commandable> commandable_ptr) {
    double course = read_double();
    if (course < 0. || course >= 360.)
        throw Error("Invalid heading entered!");
    double speed = read_speed();
    commandable_ptr->set_course_and_speed(course, speed);
}

void Controller::position_cmd(shared_ptr<Commandable> commandable_ptr) {
    Point destination{read_double(), read_double()};
    double speed = read_speed();
    commandable_ptr->set_destination_position_and_speed(destination, speed);
}

void Controller::destination_cmd(shared_ptr<Commandable> commandable_ptr) {
    shared_ptr<Island> island_ptr = read_and_get_island();
    double speed = read_speed();
    commandable_ptr->set_destination_island_and_speed(island_ptr, speed);
}

// Read in speed from cin. Throw Error if it's negative
double Controller::read_speed() {
    double speed = read_double();
    if (speed < 0.)
        throw Error("Negative speed entered!");
    return speed;
}

// Read a double from cin. Throw Error if double is not read in.
double Controller::read_double() {
    double number;
    if (!(cin >> number))
        throw Error("Expected a double!");
    return number;
}

void Controller::load_at_cmd(shared_ptr<Commandable> commandable_ptr) {
    shared_ptr<Island> island_ptr = read_and_get_island();
    commandable_ptr->set_load_destination(island_ptr);
}

void Controller::unload_at_cmd(shared_ptr<Commandable> commandable_ptr) {
    shared_ptr<Island> island_ptr = read_and_get_island();
    commandable_ptr->set_unload_destination(island_ptr);
}

void Controller::dock_at_cmd(shared_ptr<Commandable> commandable_ptr) {
    shared_ptr<Island> island_ptr = read_and_get_island();
    commandable_ptr->dock(island_ptr);
}

// Read Island name from cin and find the corresponding island pointer.
shared_ptr<Island> Controller::read_and_get_island() {
    string island_name = read_string();
    return Model::get_instance().get_island_ptr(island_name);
}

void Controller::attack_cmd(shared_ptr<Commandable> commandable_ptr) {
    string ship_name = read_string();
    shared_ptr<Ship> target_ship = Model::get_instance().get_ship_ptr(ship_name);
    commandable_ptr->attack(target_ship);
}

void Controller::refuel_cmd(shared_ptr<Commandable> commandable_ptr) {
    commandable_ptr->refuel();
}

void Controller::stop_cmd(shared_ptr<Commandable> commandable_ptr) {
    commandable_ptr->stop();
}

void Controller::stop_attack_cmd(shared_ptr<Commandable> commandable_ptr) {
    commandable_ptr->stop_attack();
}

string Controller::read_string() {
    string str;
    cin >> str;
    return str;
}

