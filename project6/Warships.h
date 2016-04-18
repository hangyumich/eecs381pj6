#ifndef WARSHIPS_H
#define WARSHIPS_H

#include "Ship.h"

#include <memory>


/* ********************** Warships Interface *********************
 ****************************************************************** */

class Warships : public Ship{
public:
    Warships(const std::string& name_, Point position_, double fuel_capacity_,
            double maximum_speed_, double fuel_consumption_, int resistance_,
            double firepower_, double max_attack_range_);
    Warships(std::istream& is);
    
    void update() override;
    
    void describe() const override;
    
    // start an attack on a target ship
    void attack(std::shared_ptr<Ship> target_ptr_) override;
    
    // stop attacking and discard target pointer
    void stop_attack() override;
    
    void save(std::ostream&) const override;

    virtual Warships& operator= (const Warships&);
protected:
    virtual void target_out_of_range(std::shared_ptr<Ship> target) = 0;
    
    bool is_attacking()
    { return attacking; }
    
private:
    double firepower;
    double max_attack_range;
    std::weak_ptr<Ship> target;
    bool attacking = false;
    
};


/* ********************** Cruiser Interface *********************
 ****************************************************************** */

/*
 A Cruiser is a Ship with moderate speed, firepower, and resistance.
 When told to attack it will start firing at the target if it is in range.
 At each update, it will stop attacking if the target is either no longer afloat
 (i.e. is sinking or sunk), or is out of range. As long as the target is both afloat
 and in range, it will keep firing at it.
 
 Initial values:
 fuel capacity and initial amount: 1000, maximum speed 20., fuel consumption 10.tons/nm,
 resistance 6, firepower 3, maximum attacking range 15
 */

class Cruiser : public Warships{
public:
    Cruiser(const std::string& name_, Point position_);
    Cruiser(std::istream &);
    void describe() const override;
    
    // Will counter-attack if received hit
    void receive_hit(int hit_force, std::shared_ptr<Ship> attacker_ptr) override;
    
    void save(std::ostream &) const override;
    
private:
    void target_out_of_range(std::shared_ptr<Ship> target) override;
};



/* ********************** Torpedo Boat Interface *********************
 ****************************************************************** */

class Torpedo_boat : public Warships {
public:
    Torpedo_boat(const std::string& name_, Point position_);
    Torpedo_boat(std::istream&);
    void describe() const override;
    
    // Will escape after received hit
    void receive_hit(int hit_force, std::shared_ptr<Ship> attacker_ptr) override;
    
    void save(std::ostream &) const override;
private:
    void target_out_of_range(std::shared_ptr<Ship> target) override;
    std::shared_ptr<Island> find_refuge_island(std::shared_ptr<Ship> attacker);
};

#endif