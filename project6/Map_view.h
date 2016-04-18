#ifndef MAP_VIEW_H
#define MAP_VIEW_H

#include "Grid_view.h"
#include <vector>
#include <string>

struct Point;

class Map_view : public Grid_view {
public:
    // default constructor sets the default size, scale, and origin
    Map_view();
    Map_view(std::istream &);
    
    // modify the display parameters
    // if the size is out of bounds will throw Error("New map size is too big!")
    // or Error("New map size is too small!")
    void set_size(int size_);
    
    // If scale is not postive, will throw Error("New map scale must be positive!");
    void set_scale(double scale_);
    
    // any values are legal for the origin
    void set_origin(Point origin_);
    
    // set the parameters to the default values
    void set_defaults();
    
    // Save the current view status to os
    void save(std::ostream& os) const override;

private:
    /* Helper Function */
    void print_map_info(std::vector<std::string> outsider) const override;
    Point get_relative_location(Point location) const override;
    std::vector<std::vector<std::string>> get_initial_map() const override;
};


#endif