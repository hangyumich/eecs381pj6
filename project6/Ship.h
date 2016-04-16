#ifndef SHIP_H
#define SHIP_H

#include "Sim_object.h"
#include "Commandable.h"
#include "Track_base.h"

#include <memory>

/***** Ship Class *****/
/* A Ship has a name, initial position, amount of fuel, and parameters that govern its movement.
The initial amount of fuel is equal to the supplied fuel capacity - a full fuel tank.
A Ship can be commanded to move to either a position, and Island, or follow a course, or stop,
dock at or refuel at an Island. It consumes fuel while moving, and becomes immobile
if it runs out of fuel. It inherits the Sim_object interface to the rest of the system,
and the Track_base class provides the basic movement functionality, with the unit of time
corresponding to 1.0 for one "tick" - an hour of simulated time. The speeds and rates
are specified as per unit time, but in this project, the update time is always 1.0.

The update function updates the position and/or state of the ship.
The describe function outputs information about the ship state.
Accessors make the ship state available to either the public or to derived classes.
The is a "fat interface" for the capabilities of derived types of Ships. These
functions are implemented in this class to throw an Error exception.
*/

enum class Ship_state;

class Island;

class Ship : public Sim_object, public Commandable, public std::enable_shared_from_this<Ship>{
public:
	/*** Readers ***/
	// return the current position
    Point get_location() const override {return track_base.get_position();}
	
	// Return true if ship can move (it is not dead in the water or in the process or sinking); 
	bool can_move() const;
	
	// Return true if ship is moving
	bool is_moving() const;

	// Return true if ship is docked
	bool is_docked() const;
	
	// Return true if ship is afloat (not in process of sinking), false if not
	bool is_afloat() const;
		
	// Return true if the ship is Stopped and the distance to the supplied island
	// is less than or equal to 0.1 nm
    bool can_dock(std::shared_ptr<Island> island_ptr) const;
	
	/*** Interface to derived classes ***/
	// Update the state of the Ship
	void update() override;
	// output a description of current state to cout
	void describe() const override;
	
	void broadcast_current_state()  const override;
    
    // interactions with other objects
    // receive a hit from an attacker
    virtual void receive_hit(int hit_force, std::shared_ptr<Ship> attacker_ptr);
	
    
	/*** Command functions ***/
    
	// Start moving to a destination position at a speed
     // may throw Error("Ship cannot move!")
     // may throw Error("Ship cannot go that fast!")
	void set_destination_position_and_speed(Point destination_position, double speed) override;
    
	// Start moving to a destination Island at a speed
     // may throw Error("Ship cannot move!")
     // may throw Error("Ship cannot go that fast!")
    void set_destination_island_and_speed(std::shared_ptr<Island> destination_island,
                                          double speed) override;
	// Start moving on a course and speed
     // may throw Error("Ship cannot move!")
     // may throw Error("Ship cannot go that fast!");
    void set_course_and_speed(double course, double speed) override;
    
	// Stop moving
     // may throw Error("Ship cannot move!");
    void stop() override;
    
	// dock at an Island - set our position = Island's position, go into Docked state
     // may throw Error("Can't dock!");
    void dock(std::shared_ptr<Island> island_ptr) override;
    
	// Refuel - must already be docked at an island; fill takes as much as possible
     // may throw Error("Must be docked!");
	void refuel() override;

	/*** Fat interface command functions ***/
	// These functions throw an Error exception for this class
    // will always throw Error("Cannot load at a destination!");
    void set_load_destination(std::shared_ptr<Island>) override;
    
    // will always throw Error("Cannot unload at a destination!");
    void set_unload_destination(std::shared_ptr<Island>) override;
    
    // will always throw Error("Cannot attack!");
    void attack(std::shared_ptr<Ship> target_ptr) override;
    
    // will always throw Error("Cannot attack!");
    void stop_attack() override;
    
protected:
    // Make constructor protected so that client cannot create this object.
    Ship(const std::string& name_, Point position_, double fuel_capacity_,
         double maximum_speed_, double fuel_consumption_, int resistance_);
    
    double get_maximum_speed() const
    {return maximum_speed;}
	// return pointer to the Island currently docked at, or nullptr if not docked
    std::shared_ptr<Island> get_docked_Island() const
    {return docked_Island;}
	// return pointer to current destination Island, nullptr if not set
    std::shared_ptr<Island> get_destination_Island() const
    {return destination_Island;}

private:
    Track_base track_base;
	double fuel;                            // Current amount of fuel
	double fuel_consumption;                // tons/nm required
    double fuel_capacity;
    double maximum_speed;
    int resistance;
    Ship_state ship_state;
	Point destination_point;                // Current destination position
    std::shared_ptr<Island> destination_Island;	// Current destination Island, if any
    std::shared_ptr<Island> docked_Island;

	// Updates position, fuel, and movement_state, assuming 1 time unit (1 hr)
	void calculate_movement();
    void set_course_speed_and_dest(Point destination_position, double speed);
    void check_state_and_speed(double speed);
};

#endif