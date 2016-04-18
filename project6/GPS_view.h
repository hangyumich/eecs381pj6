#ifndef GPS_VIEW_H
#define GPS_VIEW_H

#include "Grid_view.h"
#include <vector>
#include <string>

struct Point;

class GPS_view : public Grid_view {
public:
    // default constructor sets the default size, scale, and origin
    GPS_view(const std::string& name);
    GPS_view(std::istream&);
    
    // modify the display parameters
    // if the size is out of bounds will throw Error("New map size is too big!")
    // or Error("New map size is too small!")
    // update origin
    void set_size(int size_);
    
    // If scale is not postive, will throw Error("New map scale must be positive!");
    // update origin
    void set_scale(double scale_);
    
    // set the parameters to the default values; update origin
    void set_defaults();
    
    // update heading and origin if the name is the gps view name
    void update_course(const std::string& name, double course) override;
    
    // Save the supplied name and location for future use in a draw() call
    void update_location(const std::string& name, Point location) override;
    
    // removes from view; if applies, update view to sunk view.
    void update_remove(const std::string& name) override;
    
    void save(std::ostream &) const override;
    
    std::string get_ship_name() {return ship_name;}
private:
    /* Helper Function */
    std::vector<std::vector<std::string>> get_initial_map() const override;
    void print_map_info(std::vector<std::string> outsider) const override;
    Point get_relative_location(Point location) const override;
    void update_map(std::vector<std::vector<std::string>>& grid_map,
                    int ix, int iy, const std::string& name) const override;
    
    // own helper functions
    // sets origin so that ship_location is map center
    void reset_origin();
    
    /* Private Member Variables */
    std::string ship_name;
    Point ship_location; // center of map
    double ship_heading; // relative north
    bool is_sunk = false;
};

#endif