#ifndef CRUISE_SHIP_H
#define CRUISE_SHIP_H

#include "Ship.h"

#include <string>
#include <memory>
#include <list>

enum class Cruise_state;

class Cruise_ship : public Ship {
public:
    // initialize, then output constructor message
    Cruise_ship(const std::string& name_, Point position_);
    Cruise_ship(std::istream&);
    
    // perform Cruise_ship specific behavior
    void update() override;
    
    void describe() const override;

    // Cancel the current cruise and start a new cruise when arrives at island
    void set_destination_position_and_speed(Point destination_point, double speed) override;
    
    // Cancel the current cruise and perform the ship specific behaviors
    void set_destination_island_and_speed(std::shared_ptr<Island> destination_island,
                                          double speed) override;
    
    // Cancel the current cruise and perform the ship specific behaviors
    void set_course_and_speed(double course, double speed) override;
    
    // Cancel the current cruise and perform the ship specific behaviors
    void stop() override;
    void save(std::ostream &) const override;
    
private:
    Cruise_state cruise_state;
    double cruise_speed = 0.;
    std::shared_ptr<Island> init_island;
    std::list<std::shared_ptr<Island>> unvisited_island;
    
    /* Helper functions */
    void cancel_cruise();
    std::shared_ptr<Island> get_next_Island();
};

#endif