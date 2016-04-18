#include "Grid_view.h"
#include "Geometry.h"
#include "Utility.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>
#include <algorithm>

using std::for_each;
using std::setprecision;
using std::setw;
using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::floor;

Grid_view::Grid_view(int size_, double scale_, Point origin_) :
size(size_), scale(scale_), origin(origin_) {}

Grid_view::Grid_view(std::istream & is) :
size(read_int(is)), scale(read_double(is)), origin(read_point(is))
{
    // read the size, then put in key and value
    int memory_size = read_int(is);
    while (memory_size--) {
        std::string key;
        is >> key;
        memory[key] = read_point(is);
    }
}

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

// save view status to os
void Grid_view::save(std::ostream& os) const {
    os << size << " " << scale << " " << origin << endl;
    os << memory.size() << endl;
    std::for_each(memory.begin(), memory.end(), [&os](const std::pair<const std::string, Point>& pair){os << pair.first << " " << pair.second << endl;});
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

void Grid_view::draw_y_label(int y_index) const {
    if (y_index % 3 == 0) {
        print_label(y_index, "y");
        cout << " ";
    } else {
        cout << "     ";
    }
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