#include "Map_view.h"
#include "Utility.h"
#include <iostream>
#include <vector>

using std::cout;
using std::endl;
using std::string;
using std::vector;

Map_view::Map_view() :
Grid_view(25, 2.0, Point(-10., -10.)){}

Map_view::Map_view(std::istream & is) : Grid_view(is) {}

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

// save view state to os
void Map_view::save(std::ostream &os) const {
    os << "Map_view" << endl;
    Grid_view::save(os);
}

// return an initialized grid map
vector<vector<string>> Map_view::get_initial_map() const {
    return vector<vector<string>>(get_size(), vector<string>(get_size(), ". "));
}
