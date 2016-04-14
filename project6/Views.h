#ifndef VIEWS_H
#define VIEWS_H

#include "View.h"

#include <map>
#include <string>

struct Point;

/* ********************** Grid View Interface *********************
 ****************************************************************** */

class Grid_view : public View {
public:
    // Save the supplied name and location for future use in a draw() call
    void update_location(const std::string& name, Point location) override;
    
    // Remove the name and its location; no error if the name is not present.
    void update_remove(const std::string& name) override;
    
protected:
    Grid_view(int size_, double scale_, Point origin_);
    
    /* Getter and Setter */
    const std::map<std::string, Point>& get_memory() const
    { return memory; }
    
    double get_size() const
    { return size; }
    
    void set_size(double size_);
    void set_scale(double scale_);
    void set_origin(Point origin_);
    
    
    /* Helper Function */
    bool get_subscripts(int &ix, int &iy, Point location) const;
    void print_map_info() const;
    void print_x_axis() const;
    void print_label(int index, std::string axis) const;
private:
    int size;			// current size of the display
    double scale;		// distance per cell of the display
    Point origin;		// coordinates of the lower-left-hand corner
    std::map<std::string, Point> memory;
};



/* ********************** Bridge View Interface *********************
 ****************************************************************** */

class Bridge_view : public Grid_view {
public:
    Bridge_view(const std::string& name);
    
    // update heading if the name is the bridge view name
    void update_course(const std::string& name, double course) override;
    
    // Save the supplied name and location for future use in a draw() call
    void update_location(const std::string& name, Point location) override;
    
    // Update view to sunk view.
    void update_remove(const std::string& name) override;
    
    // prints out the current map
    void draw() const override;
    
private:
    std::string ship_name;
    Point ship_location;
    double ship_heading = 0.;
    bool is_sunk = false;
};



/* ********************** Map View Interface *********************
 ****************************************************************** */

class Map_view : public Grid_view {
public:
    // default constructor sets the default size, scale, and origin
    Map_view();
    
    // prints out the current map
    void draw() const override;
    
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
};


struct Data {
    double fuel;
    double course;
    double speed;
};


/* ********************** Sailing View Interface *********************
 ****************************************************************** */

class Sailing_view : public View {
public:
    
    // prints out textual information about all ships
    void draw() const override;
    
    // Remove the name and its location; no error if the name is not present.
    void update_remove(const std::string& name) override;
    
    // Save the supplied name and fuel for future use in a draw() call
    void update_fuel(const std::string& name, double fuel) override;
    
    // Save the supplied name and course for future use in a draw() call
    void update_course(const std::string& name, double course) override;
    
    // Save the supplied name and speed for future use in a draw() call
    void update_speed(const std::string& name, double speed) override;
    
private:
    std::map<std::string, Data> memory;
};

#endif