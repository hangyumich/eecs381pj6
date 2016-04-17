#include "Utility.h"
#include "iostream"
#include "Geometry.h"
#include "Model.h"

int read_int(std::istream& is) {
    int int_;
    is >> int_;
    if (!is.good()) {
        throw Error("Cannot read a integer value.");
    }
    return int_;
}

double read_double(std::istream& is) {
    double double_;
    is >> double_;
    if (!is.good()) {
        throw Error("Cannot read a double value.");
    }
    return double_;
}

Point read_point(std::istream& is) {
    char unused;
    is >> unused;
    int x = read_int(is);
    is >> unused;
    int y = read_int(is);
    is >> unused;
    return Point(x, y);
}

std::shared_ptr<Island> read_island_ptr(std::istream& is) {
    std::string ship_name;
    is >> ship_name;
    return Model::get_instance().get_island_ptr(ship_name);
}
