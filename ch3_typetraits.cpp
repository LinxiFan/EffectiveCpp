/**
 * Chapter 3. Moving to Modern C++
 * item 9 typetraits
 */
#include <iostream>
#include <vector>
#include <unordered_map>
#include "utils.h"

using namespace std;

// easily convert from C++11 to C++14
template<typename T>
using my_remove_const_t = typename remove_const<T>::type;


int main() {
    /* Item 9 type traits */
    ptype<typename remove_const<const int>::type>();  // C++11, must prefix with `typename`
    ptype<remove_const_t<const int>>();  // C++14
    ptype<my_remove_const_t<const int>>();  // easily DIY if C++14 unavailable
    ptype<typename add_const<char>::type>();  // C++11, must prefix with `typename`
    ptype<remove_reference_t<const double&>>();  // C++14
    ptype<add_lvalue_reference_t<unsigned int>>();  // C++14
    ptype<add_rvalue_reference_t<int*>>();  // C++14


    return 0;
}
