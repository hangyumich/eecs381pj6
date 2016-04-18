#ifndef GRID_VIEW_H
#define GRID_VIEW_H

#include "View.h"
#include <map>
#include <vector>
#include <string>

struct Point;

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
    virtual void draw_y_label(int y_index) const;
    void print_label(int index, const std::string& axis) const;
private:
    int size;			// current size of the display
    double scale;		// distance per cell of the display
    Point origin;		// coordinates of the lower-left-hand corner
    std::map<std::string, Point> memory;
};

#endif