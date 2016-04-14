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

// Print size, scale, and origin
void Grid_view::print_map_info() const {
    cout << "Display size: " << size << ", scale: " << scale
    << ", origin: " << origin << endl;
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

// Change output stream setting and print out the label value.
void Grid_view::print_label(int index , string axis) const {
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

// draw water pattern if ship is sunk. Otherwise, draw map looking over from ship bow */
void Bridge_view::draw() const {
    // Start drawing map
    vector<vector<string>> grid_map(get_size(), vector<string>(3, is_sunk ? "w-" : ". "));
    cout << "Bridge view from " << ship_name;
    if (is_sunk) {
        cout << " sunk at " << ship_location << endl;
    } else {
        cout << " position " << ship_location << " heading " << ship_heading << endl;
        for (const auto& object : get_memory()) {
            double distance = cartesian_distance(ship_location, object.second);
            if (distance < 0.005 || distance > 20)
                continue;
            double bearing = Compass_position(ship_location, object.second).bearing;
            bearing -= ship_heading;
            if (bearing < -180.)
                bearing += 360.;
            else if (bearing > 180.)
                bearing -= 360;
            int ix, iy;
            if (!get_subscripts(ix, iy, Point{bearing, 0}))
                continue;
            grid_map[ix][iy] = (grid_map[ix][iy] == ". " ? object.first.substr(0, 2) : "* ");
        }
    }
    
    for (int y = (int)grid_map[0].size() - 1; y >= 0; --y) {
        cout << "     ";
        for (int x = 0; x < get_size(); ++x)
            cout << grid_map[x][y];
        cout << endl;
    }
    
    cout << "  ";
    for (int x_index = 0; x_index < get_size(); x_index += 3) {
        cout << (x_index != 0 ? "  " : "");
        print_label(x_index, "x");
    }
    cout << endl;
}





/* ****************** Map View Implementation *********************
 ****************************************************************** */

Map_view::Map_view() :
Grid_view(25, 2.0, Point(-10., -10.)){}

// prints out the current map
void Map_view::draw() const {
    vector<vector<string>> grid_map(get_size(), vector<string>(get_size(), ". "));
    vector<string> outsider;
    for (const auto& object : get_memory()) {
        int ix, iy;
        string name = object.first;
        Point location = object.second;
        if (!get_subscripts(ix, iy, location))
            outsider.push_back(name);
        else
            grid_map[ix][iy] = (grid_map[ix][iy] == ". " ? name.substr(0, 2) : "* ");
    }
    
    // print out map information
    print_map_info();
    for (auto iter = outsider.begin(); iter != outsider.end(); ++iter)
        cout << (iter != outsider.begin() ? ", " : "") << *iter;
    if (!outsider.empty())
        cout << " outside the map" << endl;
    
    // start drawing map
    for (int y_index = get_size() - 1; y_index >= 0; --y_index) {
        // draw ylabel
        if (y_index % 3 == 0) {
            print_label(y_index, "y");
            cout << " ";
        } else {
            cout << "     ";
        }
        for (int x_index = 0; x_index < get_size(); ++x_index)
            cout << grid_map[x_index][y_index];
        cout << endl;
    }
    // draw x label
    cout << "  ";
    for (int x_index = 0; x_index < get_size(); x_index += 3) {
        cout << (x_index != 0 ? "  " : "");
        print_label(x_index, "x");
    }
    cout << endl;
}

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



