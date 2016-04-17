#include "Sim_object.h"

#include <string>
#include <iostream>

using std::string;

Sim_object::Sim_object(const string& name_) : name(name_) {}

Sim_object::Sim_object(std::istream& is) {
    string sim_name;
    is >> sim_name;
    name = sim_name;
}

void Sim_object::save(std::ostream &os) const {
    os << name << std::endl;
}