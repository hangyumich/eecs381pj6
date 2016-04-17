#ifndef VIEW_H
#define VIEW_H

#include "Geometry.h"

#include <map>
#include <string>
#include <iosfwd>

/* *** View class ***
 View class is the base class for all other view classes.
 */

struct Point;

class View {
public:
    virtual ~View() {}
    
	// Remove the name and its location; no error if the name is not present.
    virtual void update_remove(const std::string& name) = 0;
    
    // Save the supplied name and location for future use in a draw() call
    virtual void update_location(const std::string& name, Point location) {}
    
    // Save the supplied name and fuel for future use in a draw() call
    virtual void update_fuel(const std::string& name, double fuel) {}
    
    // Save the supplied name and course for future use in a draw() call
    virtual void update_course(const std::string& name, double course) {}
    
    // Save the supplied name and speed for future use in a draw() call
    virtual void update_speed(const std::string& name, double speed) {}
	
	// prints out the current map
	virtual void draw() const = 0;
    
    // Save current view status to so
    virtual void save(std::ostream&) const {};
};

#endif
