
#ifndef REFUEL_SHIP_H
#define REFUEL_SHIP_H

#include "Ship.h"
#include <string>
#include <memory>

enum class Refuel_state;

class Refuel_ship : public Ship {
public:
    // initialize, then output constructor message
    Refuel_ship(const std::string& name_, Point position_);
    
    // perform Refuel_ship specific behavior
    void update() override;
    
    void describe() const override;
    
    // throw Error if not in not_refueling; else normal behavior
    void set_destination_position_and_speed(Point destination_point, double speed) override;
    
    // Sets new base_island and starts entering refuel states
    void set_destination_island_and_speed(std::shared_ptr<Island> destination_island,
                                          double speed) override;
    
    // throw Error if not in not_refueling; else normal behavior
    void set_course_and_speed(double course, double speed) override;
    
    // stops and forgets Refuel_ship specific information -> becomes not_refueling
    void stop() override;
    
private:
    Refuel_state refuel_state;
    double cargo;
    double cargo_capacity;
    std::shared_ptr<Island> base_island;
    std::shared_ptr<Ship> target_ship; // ship to refuel
    
    /* Helper functions */
    void throw_if_in_cycle();
    void find_next_ship();
};

#endif


