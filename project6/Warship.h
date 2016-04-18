#ifndef WARSHIP_H
#define WARSHIP_H

#include "Ship.h"
#include <memory>

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

#endif