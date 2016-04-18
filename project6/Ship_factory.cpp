#include "Ship_factory.h"
#include "Navigation.h"
#include "Warships.h"
#include "Tanker.h"
#include "Cruise_ship.h"
#include "Refuel_ship.h"
#include "Utility.h"
#include "Model.h"

#include <memory>
using std::shared_ptr;

/* This is a very simple form of factory, a function; you supply the information, it creates
 the specified kind of object and returns a pointer to it. The Ship is allocated
 with new, so some other component is resposible for deleting it.
 */
shared_ptr<Ship> create_ship(const std::string& name, const std::string& type,
                              Point initial_position) {
    if (type == "Cruiser")
        return shared_ptr<Ship>(new Cruiser(name, initial_position));
    else if (type == "Tanker")
        return shared_ptr<Ship>(new Tanker(name, initial_position));
    else if (type == "Cruise_ship")
        return shared_ptr<Ship>(new Cruise_ship(name, initial_position));
    else if (type == "Torpedo_boat")
        return shared_ptr<Ship>(new Torpedo_boat(name, initial_position));
    else if (type == "Refuel_ship")
        return shared_ptr<Ship>(new Refuel_ship(name, initial_position));
    else
        throw Error("Trying to create ship of unknown type!");
}

template <typename T>
void update_model_ship(const shared_ptr<Ship> new_ship) {
    if (Model::get_instance().is_ship_present(new_ship->get_name())) {
        shared_ptr<T> ship_in_model =
        std::dynamic_pointer_cast<T>(Model::get_instance().get_ship_ptr(new_ship->get_name()));
        *ship_in_model = *(std::dynamic_pointer_cast<T>(new_ship));
        ship_in_model->broadcast_current_state();
    }
}

shared_ptr<Ship> restore_ship(std::istream& is) {
    std::string type;
    is >> type;
    shared_ptr<Ship> new_ship;
    if (type == "Cruiser") {
        new_ship = shared_ptr<Ship>(new Cruiser(is));
        update_model_ship<Cruiser>(new_ship);
    } else if (type == "Tanker") {
        new_ship = shared_ptr<Ship>(new Tanker(is));
        update_model_ship<Tanker>(new_ship);
    } else if (type == "Cruise_ship") {
        new_ship = shared_ptr<Ship>(new Cruise_ship(is));
        update_model_ship<Cruise_ship>(new_ship);
    } else if (type == "Torpedo_boat") {
        new_ship = shared_ptr<Ship>(new Torpedo_boat(is));
        update_model_ship<Torpedo_boat>(new_ship);
    } else if (type == "Refuel_ship") {
        new_ship = shared_ptr<Ship>(new Refuel_ship(is));
        update_model_ship<Refuel_ship>(new_ship);
    } else {
        throw Error("Trying to create ship of unknown type!");
    }
    return new_ship;
}