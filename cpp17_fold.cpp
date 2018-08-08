/*
 * Template magic in C++14 and C++17
 */

#include "utils.h"


class Elem {
public:
    Elem(string x)
    : x(x)
    {}

    Elem operator+(const Elem& other) {
        return {"(" + x + "+" + other.x + ")"};
    }

    string x;
};


ostream& operator<<(ostream& oss, const Elem& elem) {
    return oss << elem.x << endl;
}


/*
 * C++ variadic folding
 * https://en.cppreference.com/w/cpp/language/fold
 */
template<typename... Ts>
auto left_sum(Ts&&... args) {
    return (args + ...);
}

template<typename... Ts>
auto right_sum(Ts&& ... args) {
    return (... + args);
}

/*
 * comma is also a unary operator that returns nothing
 */
template<typename T, typename... Ts>
vector<T>& multi_push(vector<T>& vec, Ts&& ... elems) {
    (vec.push_back(elems), ...);
    return vec;
}


int main() {
    cout << left_sum(Elem{"hello"}, Elem{"my"}, Elem{"world"}, Elem{"yo"}) << endl;
    cout << right_sum(Elem{"hello"}, Elem{"my"}, Elem{"world"}, Elem{"yo"}) << endl;
    vector<int> vec {3, 5, 6, 10, 2};
    cout << multi_push(vec, -5, 7, 18, 200) << endl;

    // template fold magic
    cout << sprint("hello ", 3.1415, " my ", -20, -1.11f) << endl;
    cout << sjoin(" <-> ", "hello", 3.1415, "my", -20, -1.11f) << endl;
}

