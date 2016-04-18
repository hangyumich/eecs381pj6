#ifndef BRIDGE_VIEW_H
#define BRIDGE_VIEW_H

#include "Grid_view.h"
#include <vector>
#include <string>

struct Point;

class Bridge_view : public Grid_view {
public:
    Bridge_view(const std::string& name);
    Bridge_view(std::istream & os);

    // update heading if the name is the bridge view name
    void update_course(const std::string& name, double course) override;
    
    // Save the supplied name and location for future use in a draw() call
    void update_location(const std::string& name, Point location) override;
    
    // Update view to sunk view.
    void update_remove(const std::string& name) override;
   
    // Save the current view status to os
    void save(std::ostream& os) const override;
    
    std::string get_ship_name() const {return ship_name;}
    
private:
    /* Helper Function */
    void print_map_info(std::vector<std::string> outsider) const override;
    Point get_relative_location(Point location) const override;
    std::vector<std::vector<std::string>> get_initial_map() const override;
    void draw_y_label(int y_index) const override;
    void update_map(std::vector<std::vector<std::string>>& grid_map,
                    int ix, int iy, const std::string& name) const override;
    
    /* Private Member Variables */
    std::string ship_name;
    Point ship_location;
    double ship_heading = 0.;
    bool is_sunk = false;
};

#endif