#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <map>
#include <string>
#include <memory>
#include <list>
#include <iosfwd>

/* Controller
This class is responsible for controlling the Model and View according to interactions
with the user.
*/

class View;
class Ship;
class Island;
class Map_view;
class Sailing_view;
class Bridge_view;

class Controller {
public:
	// create View object, run the program by acccepting user commands, then destroy View object
	void run();

private:
	// Controller keeps its own pointer to the View because it has to manage the View.
	// Future versions will need to manage more than one view.
    std::shared_ptr<Map_view> map_view;
    std::shared_ptr<Sailing_view> sailing_view;
    std::map<std::string, std::shared_ptr<Bridge_view>> bridge_views;
    
    template<typename T>
    T get_func_ptr(std::map<std::string, T> cmds, const std::string& cmd_word);

    void quit_cmd();
    
    /* View Command Function */
    void default_cmd();
    void size_cmd();
    void zoom_cmd();
    void pan_cmd();
    void show_cmd();
    void open_map_view_cmd();
    void close_map_view_cmd();
    void open_sailing_view();
    void close_sailing_view();
    void open_bridge_view();
    void close_bridge_view();
    void check_map_is_open();
    
    
    /* Model Command Function */
    void status_cmd();
    void go_cmd();
    void create_cmd();
    void save_cmd();
    void restore_cmd();
    
    /* Group Command Function */
    void create_group_cmd() {}
    void delete_group_cmd() {}
    void add_member_cmd() {}
    void delete_member_cmd() {}
    
    /* Ship Command Function */
    void course_cmd(std::shared_ptr<Ship>);
    void position_cmd(std::shared_ptr<Ship>);
    void destination_cmd(std::shared_ptr<Ship>);
    void load_at_cmd(std::shared_ptr<Ship>);
    void unload_at_cmd(std::shared_ptr<Ship>);
    void dock_at_cmd(std::shared_ptr<Ship>);
    void attack_cmd(std::shared_ptr<Ship>);
    void refuel_cmd(std::shared_ptr<Ship>);
    void stop_cmd(std::shared_ptr<Ship>);
    void stop_attack_cmd(std::shared_ptr<Ship>);
    
    /* Auxiliary Function */
    std::shared_ptr<Island> read_and_get_island();
    double read_speed();
    double read_double();
    template <typename T>
    T read_open_file(std::istream &);
};
	
#endif