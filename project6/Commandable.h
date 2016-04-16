#include <memory>

/* This is an abstract interface class supposed to be inherited by
 group and ship. Those classes inherit from this class can be controlled as
 one unit */

class Island;
class Point;
class Ship;

class Commandable {
public:
    // Start moving on a course and speed
    virtual void set_course_and_speed(double course, double speed) = 0;
    
    // Start moving to a destination position at a speed
    virtual void set_destination_position_and_speed(Point destination_position,
                                                    double speed) = 0;
    
    // Start moving to a destination Island at a speed
    virtual void set_destination_island_and_speed(std::shared_ptr<Island> destination_island,
                                                  double speed) = 0;
    
    // Stop the unit
    virtual void stop() = 0;
    
    // dock at an Island
    virtual void dock(std::shared_ptr<Island> island_ptr) = 0;
    
    // let the unit refuel at docked island
    virtual void refuel() = 0;
    
    // set load destination for the unit
    virtual void set_load_destination(std::shared_ptr<Island>) = 0;
    
    // set unload destination for the unit
    virtual void set_unload_destination(std::shared_ptr<Island>) = 0;
    
    // let the unt start attacking target
    virtual void attack(std::shared_ptr<Ship> target_ptr) = 0;
    
    // let the unit stop attacking target
    virtual void stop_attack() = 0;
};