/*
 * Template magic in C++14 and C++17
 */

#include "utils.h"
#include <boost/hana.hpp>

namespace hana = boost::hana;
struct Fish { std::string name; };
struct Cat  { std::string name; };
struct Dog  { std::string name; };


int main() {
    auto animals = hana::make_tuple(Fish{"Nemo"}, Cat{"Garfield"}, Dog{"Snoopy"});
    display_type(animals);
}

