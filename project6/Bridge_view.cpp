#include "Bridge_view.h"
#include "Geometry.h"
#include "Navigation.h"
#include "Utility.h"

#include <iostream>
#include <vector>

using std::cout;
using std::endl;
using std::string;
using std::vector;

Bridge_view::Bridge_view(const string& name) :
Grid_view(19, 10., Point{-90., 0}), ship_name(name){}

Bridge_view::Bridge_view(std::istream & is) :
Grid_view(is) {
    is >> ship_name;
    ship_location = read_point(is);
    ship_heading = read_double(is);
    is_sunk = read_int(is);
}

// update the course if name is the bridge view name
void Bridge_view::update_course(const string& name, double course) {
    if (name == ship_name)
        ship_heading = course;
}

// update location if name is the bridge view name
void Bridge_view::update_location(const string& name, Point location) {
    Grid_view::update_location(name, location);
    if (name == ship_name)
        ship_location = location;
}

// change the state to sunk if name is the bridge view name
void Bridge_view::update_remove(const string& name) {
    Grid_view::update_remove(name);
    if (name == ship_name)
        is_sunk = true;
}

// Save the state to os
void Bridge_view::save(std::ostream &os) const {
    os << "Bridge_view" << endl;
    Grid_view::save(os);
    os << ship_name << " " << ship_location << " " << ship_heading << " " << is_sunk << endl;
}

// Print size, scale, and origin
void Bridge_view::print_map_info(vector<string> outsider) const {
    cout << "Bridge view from " << ship_name;
    if (is_sunk)
        cout << " sunk at " << ship_location << endl;
    else
        cout << " position " << ship_location << " heading " << ship_heading << endl;
}

// Transform ship's location to relative location in the map
Point Bridge_view::get_relative_location(Point location) const {
    double distance = cartesian_distance(ship_location, location);
    // return origin - 1 to indicate not on map
    if (distance < 0.005 || distance > 20)
        return Point{get_origin().x - 1, 0};
    double bearing = Compass_position(ship_location, location).bearing;
    bearing -= ship_heading;
    if (bearing < -180.)
        bearing += 360.;
    else if (bearing > 180.)
        bearing -= 360;
    return Point{bearing, 0};
}


// return an initialized grid map
vector<vector<string>> Bridge_view::get_initial_map() const {
    return vector<vector<string>>(get_size(), vector<string>(3, is_sunk ? "w-" : ". "));
}

// draw y label
void Bridge_view::draw_y_label(int y_index) const {
    cout << "     ";
}

// if ship is sunk, we don't need to update the grid map.
void Bridge_view::update_map(vector<vector<string>>& grid_map,
                             int ix, int iy, const std::string& name) const {
    if (is_sunk)
        return;
    Grid_view::update_map(grid_map, ix, iy, name);
}
