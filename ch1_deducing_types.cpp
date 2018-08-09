/**
 * Chapter 1. Deducing Types
 * 1. Understand template type deduction
 * 2. Understand auto type deduction
 * 3. Understand decltype
 * 4. Know how to view deduced types
 */
#include <iostream>
#include <vector>
#include <boost/type_index.hpp>

using namespace std;

template <typename T>
class TD;  // type displayer, hack compiler to display type as error msg

template<typename T>
void ptype(const T& param) { // universal reference
    using boost::typeindex::type_id_with_cvr;
    cout << "type T = "
         << type_id_with_cvr<T>().pretty_name()
         << "; type param = "
         << type_id_with_cvr<decltype(param)>().pretty_name()
         << endl;
}


/*
 * very cool hack to get array size at compile time
 */
template<typename T, int N>
constexpr int array_size(const T (&)[N]) noexcept {
    return N;
};

/*
 * practice decltype
 * Container&& is universal reference
 */
template<typename Container, typename Index>
decltype(auto) access_element(Container&& c, Index i) {
    cout << "accessing element " << i << endl;
    return forward<Container>(c)[i];  // perfect forwarding
};


// disable CLion "access_element()" not assignable false alarm
#pragma clang diagnostic push
#pragma ide diagnostic ignored "NotAssignable"
int main() {
    auto x = 3;
    auto x2 = {1, 2, 3};
    auto& x3 = x;
    const auto& x4 = x;
    int x5[] = {1, 2, 3, 4, 5, 6, 7};

    ptype(x);
    ptype(x2);
    ptype(x3);
    ptype(x4);
    ptype(x5);
    cout << "Array size of x5 " << array_size(x5) << endl;

    vector<double> vec {1.3, -2.2, 3.5, 0.1};
    access_element(vec, 2) = 1000;
    cout << vec[1] << ", " << vec[2] << endl;

    return 0;
}
#pragma clang diagnostic pop