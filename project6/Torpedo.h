#ifndef TORPEDO_H
#define TORPEDO_H

#include "Warship.h"
#include <memory>

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