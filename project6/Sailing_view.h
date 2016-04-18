#ifndef SAILING_VIEW_H
#define SAILING_VIEW_H

#include "View.h"

#include <map>
#include <string>

struct Data {
    double fuel;
    double course;
    double speed;
};

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