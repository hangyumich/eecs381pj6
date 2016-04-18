#ifndef MODEL_H
#define MODEL_H

#include "Sim_object.h"

#include <set>
#include <map>
#include <list>
#include <string>
#include <iosfwd>
#include <memory>

/*
Model is part of a simplified Model-View-Controller pattern.
Model keeps track of the Sim_objects in our little world. It is the only
component that knows how many Islands and Ships there are, but it does not
know about any of their derived classes, nor which Ships are of what kind of Ship. 
It has facilities for looking up objects by name, and removing Ships.  When
created, it creates an initial group of Islands and Ships using the Ship_factory.
Finally, it keeps the system's time.

Controller tells Model what to do; Model in turn tells the objects what do, and
when asked to do so by an object, tells all the Views whenever anything changes that might be relevant.
Model also provides facilities for looking up objects given their name.
*/

class Sim_object;
class Ship;
class View;
class Group;
class Island;
struct Point;

struct Comp {
    using is_transparent = std::true_type;
    bool operator() (const std::shared_ptr<Sim_object> object1, const std::shared_ptr<Sim_object> object2) const
    { return object1->get_name() < object2->get_name(); }
    bool operator() (const std::shared_ptr<Sim_object> object, const std::string& name) const
    { return object->get_name() < name; }
    bool operator() (const std::string& name, const std::shared_ptr<Sim_object> object) const
    { return name < object->get_name(); }
};

class Model {
public:

    /*************************** General Functions ****************************/
    
    // get the singleton model object
    static Model& get_instance();
    
    // return the current time
    int get_time() {return time;}
    
    // disallow copy/move construction or assignment
    Model(const Model& other) = delete;
    Model(Model&& other) = delete;
    Model& operator=(const Model& other) = delete;
    
    /*********************** Ship and Islands Functions **********************/
    
	/* is name already in use for either ship or island?
     either the identical name, or identical in first two characters counts as in-use */
	bool is_name_in_use(const std::string& name) const;

	// is there such an island?
	bool is_island_present(const std::string& name) const;
    
	// will throw Error("Island not found!") if no island of that name
    std::shared_ptr<Island> get_island_ptr(const std::string& name) const;
    // return the views list
    std::list<std::shared_ptr<View>> get_views() const {return views;}
    
    // return all islands pointers.
    const std::set<std::shared_ptr<Island>, Comp>& get_all_islands() const
    {return islands;}
    
    // return all ship pointers.
    const std::set<std::shared_ptr<Ship>, Comp>& get_ships() const
    {return ships;}
    
	// is there such an ship?
	bool is_ship_present(const std::string& name) const;
	// add a new ship to the list, and update the view
    void add_ship(std::shared_ptr<Ship>);
	// will throw Error("Ship not found!") if no ship of that name
    std::shared_ptr<Ship> get_ship_ptr(const std::string& name) const;
	
	// tell all objects to describe themselves
	void describe() const;
	// increment the time, and tell all objects to update themselves
	void update();	
	
    
    /************************** Group Functions *******************************/
    
    // Test whether the group name has already been used by other ships, islands
    // or groups.
    bool is_group_name_valid(const std::string& group_name) const;
    
    // Test wheter a group exists with given name.
    bool is_group_present(const std::string& group_name) const;
    
    // get group pointer. Throw an error if no group with that name exists.
    std::shared_ptr<Group> get_group_ptr(const std::string& group_name) const;
    
    // Attach a group to the container
    void attach_group(std::shared_ptr<Group>);
    
    // Detach the group from the container
    void detach_group(std::shared_ptr<Group>);
    
    
    
	/****************************** View Functions *****************************/
    
	// Attaching a View adds it to the container and causes it to be updated
    // with all current objects'location (or other state information.
    void attach(std::shared_ptr<View>);
    
	// Detach the View by discarding the supplied pointer from the container of Views
    // - no updates sent to it thereafter.
    void detach(std::shared_ptr<View>);
    
    // remove the Ship from the containers.
    void remove_ship(std::shared_ptr<Ship> ship_ptr);
	
    // notify the views about an object's location
	void notify_location(const std::string& name, Point location);
    
    // notify the views about an object's fuel
    void notify_fuel(const std::string& name, double fuel);
    
    // notify the views about an object's course
    void notify_course(const std::string& name, double course);
    
    // notify the views about an object's speed
    void notify_speed(const std::string& name, double speed);
    
	// notify the views that an object is now gone
	void notify_gone(const std::string& name);
    
    // save model data to a file
    void save(std::ostream&);
    
    // restore model from a file
    void restore(std::istream&);
    
    // reset the model to init state
    void reset();
private:
    // create the initial objects.
    Model();
    ~Model(){}
    
	int time;		// the simulated time
    std::map<std::string, std::shared_ptr<Sim_object>> objects;
    std::set<std::shared_ptr<Island>, Comp> islands;
    std::set<std::shared_ptr<Ship>, Comp> ships;
    std::list<std::shared_ptr<View>> views;
    std::map<std::string, std::shared_ptr<Group>> groups;
};

#endif