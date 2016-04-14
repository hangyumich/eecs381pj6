#include "Ship_factory.h"
#include "Navigation.h"
#include "Warships.h"
#include "Tanker.h"
#include "Cruise_ship.h"
#include "Utility.h"

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
    else
        throw Error("Trying to create ship of unknown type!");
}