#ifndef VIEWS_H
#define VIEWS_H

#include "View.h"

#include <map>
#include <vector>
#include <string>
#include <iosfwd>

struct Point;

/* ********************** Grid View Interface *********************
 ****************************************************************** */

class Grid_view : public View {
public:
    // Save the supplied name and location for future use in a draw() call
    void update_location(const std::string& name, Point location) override;
    
    // Remove the name and its location; no error if the name is not present.
    void update_remove(const std::string& name) override;
    
    // draw the grid map
    void draw() const override;
    void save(std::ostream& os) const override;
    
protected:
    Grid_view(int size_, double scale_, Point origin_);
    Grid_view(std::istream &);
    
    /* Getter and Setter */
    int get_size() const { return size; }
    double get_scale() const { return scale; }
    Point get_origin() const { return origin; }
    
    void set_size(double size_);
    void set_scale(double scale_);
    void set_origin(Point origin_);
    
    
    /* Helper Function */
    bool get_subscripts(int &ix, int &iy, Point location) const;
    virtual std::vector<std::vector<std::string>> get_initial_map() const = 0;
    virtual void print_map_info(std::vector<std::string> outsider) const = 0;
    virtual Point get_relative_location(Point location) const = 0;
    virtual void update_map(std::vector<std::vector<std::string>>& grid_map,
                            int ix, int iy, const std::string& name) const;
    virtual void draw_y_label(int y_index) const = 0;
    void print_label(int index, const std::string& axis) const;
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



/* ********************** Map View Interface *********************
 ****************************************************************** */

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
    void draw_y_label(int y_index) const override;
    std::vector<std::vector<std::string>> get_initial_map() const override;
};

/* ********************** GPS View Interface *********************
 ****************************************************************** */

class GPS_view : public Grid_view {
public:
    // default constructor sets the default size, scale, and origin
    GPS_view(const std::string& name);
    
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
    
private:
    /* Helper Function */
    bool get_subscripts(int &ix, int &iy, Point location) const;
    std::vector<std::vector<std::string>> get_initial_map() const override;
    void print_map_info(std::vector<std::string> outsider) const override;
    Point get_relative_location(Point location) const override;
    void update_map(std::vector<std::vector<std::string>>& grid_map,
                    int ix, int iy, const std::string& name) const override;
    void draw_y_label(int y_index) const override;
    
    // own helper functions
    // sets origin so that ship_location is map center
    void reset_origin();
    
    /* Private Member Variables */
    std::string ship_name;
    Point ship_location; // center of map
    double ship_heading; // relative north
    bool is_sunk = false;
};

/* ********************** Sailing View Interface *********************
 ****************************************************************** */

struct Data {
    Data(double fuel_=0, double course_=0, double speed_=0): fuel(fuel_), course(course_), speed(speed_) {}
    double fuel;
    double course;
    double speed;
};

std::ostream& operator<< (std::ostream&, const Data&);

class Sailing_view : public View {
public:
    Sailing_view() {}
    Sailing_view(std::istream& is);
    
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
    
    // Save the current view status to os
    void save(std::ostream& os) const override;
    
private:
    std::map<std::string, Data> memory;
};

#endif