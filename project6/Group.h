#include "Commandable.h"

#include <memory>
#include <vector>

/* Group consists of other ships and groups. It can be controlled by user as a
 single unit. Users can also add member to the group and delete members from
 the group. When receiving a command, a group will let all members do that command*/

class Group : public Commandable {
public:
    // All members start moving on a course and speed
    void set_course_and_speed(double course, double speed) override;
    
    // All members start moving to a destination position at a speed
    void set_destination_position_and_speed(Point destination_position,
                                                    double speed) override;
    
    // All members start moving to a destination Island at a speed
    void set_destination_island_and_speed(std::shared_ptr<Island> destination_island,
                                          double speed) override;
    
    // All members stop moving
    void stop() override;
    
    // All members dock at an Island
    void dock(std::shared_ptr<Island> island_ptr) override;
    
    // All members refuel at the docked island
    void refuel() override;
    
    /* set load destination for all members. If a ship in the group does not have that
     ability, we just skip that ship */
    void set_load_destination(std::shared_ptr<Island> load_islad) override;
    
    /* set unload destination for all members. If a ship in the group does not have that
     ability, we just skip that ship */
    void set_unload_destination(std::shared_ptr<Island> unload_island) override;
    
    /* All members start attacking target. If a ship in the group does not have that
     ability, we just skip that ship */
    void attack(std::shared_ptr<Ship> target_ptr) override;
    
    /* All members stop attacking target. If a ship in the group does not have that
    ability, we just skip that ship */
    void stop_attack() override;
    
private:
    std::vector<std::shared_ptr<Commandable>> members;
};