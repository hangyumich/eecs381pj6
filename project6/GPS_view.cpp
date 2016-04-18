#include "GPS_view.h"
#include "Geometry.h"
#include "Utility.h"
#include <iostream>
#include <cmath>

using std::cout;
using std::endl;
using std::string;
using std::vector;

const double pi_c =  3.14159265;

// if location is within radius to the center of the map
bool is_outside(Point location, double map_size);

GPS_view::GPS_view(const std::string& name):
Grid_view(25, 2.0, Point(-10., -10.)), ship_name(name){}

/* Setter function */
void GPS_view::set_size(int size_) {
    if (size_ <= 6)
        throw Error("New map size is too small!");
    if (size_ > 30)
        throw Error("New map size is too big!");
    Grid_view::set_size(size_);
    reset_origin(); // size changes -> center changes -> origin changes
}

void GPS_view::set_scale(double scale_) {
    if (scale_ <= 0.)
        throw Error("New map scale must be positive!");
    Grid_view::set_scale(scale_);
    reset_origin(); // scale changes -> center changes -> origin changes
}

void GPS_view::set_defaults() {
    Grid_view::set_size(25);
    Grid_view::set_scale(2.0);
    reset_origin(); // scale changes -> center changes -> origin changes
}

void GPS_view::update_course(const string& name, double course) {
    if (name == ship_name)
        ship_heading = course;
}

void GPS_view::update_location(const string& name, Point location) {
    Grid_view::update_location(name, location);
    if (name == ship_name) {
        ship_location = location;
        reset_origin(); // ship_location change -> center changes -> origin changes
    }
}

// change the state to sunk if name is the GPS view name
void GPS_view::update_remove(const string& name) {
    Grid_view::update_remove(name);
    if (name == ship_name)
        is_sunk = true;
}

// return an initialized grid map
vector<vector<string>> GPS_view::get_initial_map() const {
    vector<vector<string>> grid_map(get_size(), vector<string>(get_size(), ". "));
    for ( int i = 0; i < grid_map.size(); i++ ) {
        for ( int j = 0; j < grid_map.size(); j++ ) {
            if ( is_outside(Point(i,j), grid_map.size()) ) { // outside the circle
                grid_map[i][j] = "  ";
            }
        }
    }
    return grid_map;
}

// print out the text info before the real grid map.
void GPS_view::print_map_info(vector<string> outsider) const {
    cout << "GPS view from " << ship_name;
    if (is_sunk)
        cout << " sunk at " << ship_location << endl;
    else
        cout << " position " << ship_location << " heading " << ship_heading << endl;
    cout << "Display size: " << get_size() << ", scale: " << get_scale()
    << ", origin: " << get_origin() << endl;
}

// transforms location into ratoated view so ship_heading is relative north
Point GPS_view::get_relative_location(Point location) const {
    double rotate = ship_heading;
    location = Point(location.x - ship_location.x, location.y - ship_location.y );
    double x, y;
    x = location.x * cos(rotate * pi_c / 180.0) - location.y * sin(rotate * pi_c / 180.0);
    y = location.x * sin(rotate * pi_c / 180.0) + location.y * cos(rotate * pi_c / 180.0);
    return Point(x + ship_location.x, y + ship_location.y);
}

// if (ix, iy) is further than radius to center, no need to update the grid map.
void GPS_view::update_map(vector<vector<string>>& grid_map,
                          int ix, int iy, const std::string& name) const {
    if (!is_outside(Point(ix,iy), get_size()))
        Grid_view::update_map(grid_map, ix, iy, name);
}

// sets origin so that ship_location is center
void GPS_view::reset_origin() {
    int offset = (get_size())/2;
    Grid_view::set_origin(Point(ship_location.x - offset * get_scale(),
                                ship_location.y - offset * get_scale()));
}

// if location is within radius to the center of the map
bool is_outside(Point location, double map_size){
    Point center((map_size/2), (map_size/2));
    return cartesian_distance(center, location) > (map_size)/2.0;
    
}
