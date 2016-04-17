#include "Views.h"
#include "Geometry.h"
#include "Navigation.h"
#include "Utility.h"

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <cmath>

using std::setprecision;
using std::setw;
using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::floor;

const double pi_c =  3.14159265;

/* ****************** Grid View Implementation *********************
 ****************************************************************** */

Grid_view::Grid_view(int size_, double scale_, Point origin_) :
size(size_), scale(scale_), origin(origin_) {}

/* Save the supplied name and location for future use in a draw() call
 If the name is already present,the new location replaces the previous one. */
void Grid_view::update_location(const string& name, Point location) {
    memory[name] = location;
}

// Remove the name and its location; no error if the name is not present.
void Grid_view::update_remove(const string& name) {
    memory.erase(name);
}

// draw the grid map
void Grid_view::draw() const {
    vector<vector<string>> grid_map = get_initial_map();
    vector<string> outsider;
    for (const auto& object : memory) {
        Point location = get_relative_location(object.second);
        string name = object.first;
        int ix, iy;
        if (!get_subscripts(ix, iy, location))
            outsider.push_back(name);
        else
            update_map(grid_map, ix, iy, name);
    }
    
    print_map_info(outsider);
    
    // start drawing map
    for (int y_index = (int)grid_map[0].size() - 1; y_index >= 0; --y_index) {
        draw_y_label(y_index);
        for (int x_index = 0; x_index < get_size(); ++x_index)
            cout << grid_map[x_index][y_index];
        cout << endl;
    }
    
    // draw x label
    for (int x_index = 0; x_index < get_size(); x_index += 3) {
        cout << "  ";
        print_label(x_index, "x");
    }
    cout << endl;
}


/* Calculate the cell subscripts corresponding to the supplied location parameter,
 using the current size, scale, and origin of the display.
 Return true if the location is within the grid, false if not */
bool Grid_view::get_subscripts(int &ix, int &iy, Point location) const
{
    // adjust with origin and scale
    Cartesian_vector subscripts = (location - origin) / scale;
    // truncate coordinates to appropriate integer after taking the floor
    // floor function will produce the greatest integer that is
    // smaller than the argument, even for negative values.
    // So - 0.05 is floored to -1., which will be outside the array.
    ix = int(floor(subscripts.delta_x));
    iy = int(floor(subscripts.delta_y));
    // if out of range, return false
    if ((ix < 0) || (ix >= size) || (iy < 0) || (iy >= size))
        return false;
    else
        return true;
}

// Print size, scale, and origin
void Grid_view::print_map_info(vector<string> outsider) const {
    cout << "Display size: " << size << ", scale: " << scale
         << ", origin: " << origin << endl;
}

// Given a map, update the map at (ix, iy) using name.
void Grid_view::update_map(vector<vector<string>>& grid_map,
                int ix, int iy, const std::string& name) const {
    grid_map[ix][iy] = (grid_map[ix][iy] == ". " ? name.substr(0, 2) : "* ");
}

/* Setter function */
void Grid_view::set_size(double size_) {
    size = size_;
}

void Grid_view::set_scale(double scale_) {
    scale = scale_;
}

void Grid_view::set_origin(Point origin_) {
    origin = origin_;
}



// Change output stream setting and print out the label value.
void Grid_view::print_label(int index , const string& axis) const {
    double label_value = index * scale;
    if (axis == "x")
        label_value += origin.x;
    else if (axis == "y")
        label_value += origin.y;
    auto old_settings = cout.flags();
    auto old_precision = cout.precision();
    cout << std::fixed << setprecision(0) << setw(4) << label_value;
    cout.flags(old_settings);
    cout.precision(old_precision);
}





/* ****************** Bridge View Implementation *********************
 ****************************************************************** */

Bridge_view::Bridge_view(const string& name) :
Grid_view(19, 10., Point{-90., 0}), ship_name(name){}

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

/* ****************** Map View Implementation *********************
 ****************************************************************** */

Map_view::Map_view() :
Grid_view(25, 2.0, Point(-10., -10.)){}

void Map_view::set_size(int size_) {
    if (size_ <= 6)
        throw Error("New map size is too small!");
    if (size_ > 30)
        throw Error("New map size is too big!");
    Grid_view::set_size(size_);
}

void Map_view::set_scale(double scale_) {
    if (scale_ <= 0.)
        throw Error("New map scale must be positive!");
    Grid_view::set_scale(scale_);
}

void Map_view::set_origin(Point origin_) {
    Grid_view::set_origin(origin_);
}

void Map_view::set_defaults() {
    Grid_view::set_size(25);
    Grid_view::set_scale(2.0);
    Grid_view::set_origin(Point(-10., -10.));
}

// print out the text info before the real grid map.
void Map_view::print_map_info(vector<string> outsider) const {
    cout << "Display size: " << get_size() << ", scale: " << get_scale()
         << ", origin: " << get_origin() << endl;
    for (auto iter = outsider.begin(); iter != outsider.end(); ++iter)
        cout << (iter != outsider.begin() ? ", " : "") << *iter;
    if (!outsider.empty())
        cout << " outside the map" << endl;
}

// ship's relative location does not change in Map view
Point Map_view::get_relative_location(Point location) const {
    return location;
}

// print y label
void Map_view::draw_y_label(int y_index) const {
    if (y_index % 3 == 0) {
        print_label(y_index, "y");
        cout << " ";
    } else {
        cout << "     ";
    }
}

// return an initialized grid map
vector<vector<string>> Map_view::get_initial_map() const {
    return vector<vector<string>>(get_size(), vector<string>(get_size(), ". "));
}

/* ****************** GPS View Implementation *********************
 ****************************************************************** */
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
    if (name == ship_name) {
        ship_heading = course;
    }
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

// does not show in map if not in grid_map or not within radius to center
bool GPS_view::get_subscripts(int &ix, int &iy, Point location) const {
    bool in_map = Grid_view::get_subscripts(ix, iy, location);
    return in_map && !is_outside(Point(ix,iy), get_size());
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

// print y label
void GPS_view::draw_y_label(int y_index) const {
    if (y_index % 3 == 0) {
        print_label(y_index, "y");
        cout << " ";
    } else {
        cout << "     ";
    }
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

/* ****************** Sailing View Implementation *********************
 ****************************************************************** */

// prints out textual information about all ships
void Sailing_view::draw() const {
    cout << "----- Sailing Data -----" << endl;
    cout << setw(10) << "Ship" << setw(10) << "Fuel"
    << setw(10) << "Course" << setw(10) << "Speed" << endl;
    for (const auto& data_pair : memory) {
        auto& data = data_pair.second;
        cout << setw(10) << data_pair.first << setw(10) << data.fuel
             << setw(10) << data.course << setw(10) << data.speed << endl;
    }
}

// Remove the name and its location; no error if the name is not present.
void Sailing_view::update_remove(const std::string& name) {
    memory.erase(name);
}

// Save the supplied name and fuel for future use in a draw() call
void Sailing_view::update_fuel(const std::string& name, double fuel) {
    memory[name].fuel = fuel;
}

// Save the supplied name and course for future use in a draw() call
void Sailing_view::update_course(const std::string& name, double course) {
    memory[name].course = course;
}

// Save the supplied name and speed for future use in a draw() call
void Sailing_view::update_speed(const std::string& name, double speed) {
    memory[name].speed = speed;
}



