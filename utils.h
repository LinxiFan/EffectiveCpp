#ifndef EFFECTIVECPP_UTILS_H
#define EFFECTIVECPP_UTILS_H

#include <iostream>
#include <vector>
#include <unordered_map>
#include <memory>
#include <boost/type_index.hpp>
#include <sstream>
#include <cassert>
using namespace std;


template <typename T>
ostream& operator<<(ostream& oss, const vector<T>& vec) {
    oss << "[";
    string delim = "";
    for (auto& i : vec) {
        oss << delim << i;
        delim = ", ";
    }
    return oss << "]";
}


/*
 * Convert anything to string, just like python print
 */
template<typename... Ts>
string sprint(Ts&& ... args) {
    ostringstream oss;
    (oss << ... << args);
    return oss.str();
}

template<typename T, typename... Ts>
string sjoin(string sep, T arg0, Ts&& ... args) {
    ostringstream oss;
    oss << arg0;
    if constexpr (sizeof ...(args) > 0) {
        oss << sep << sjoin(sep, std::forward<Ts>(args) ...);
    }
    return oss.str();
}

template<typename T>
string type_str() {
    return boost::typeindex::type_id_with_cvr<T>().pretty_name();
}

/**
 * Work with raw type:
 * display_type<my_type>();
 */
template<typename T>
void display_type() {
    cout << "type= " << type_str<T>() << endl;
}

/**
 * Work with values:
 * display_type(my_object);
 */
template<typename T>
void display_type(const T& param) {
    cout << "type= " << type_str<T>() << endl;
}


class Marker {
public:
    Marker(string x)
    : x(x)
    {}

    Marker(const Marker& other) {
        if (print_enabled)
            cout << "copy-ctor " << x << endl;
        x = other.x;
    };
    Marker& operator=(const Marker& other) {
        if (print_enabled)
            cout << "copy-assign " << x << endl;
        x = other.x;
        return *this;
    };

    Marker(Marker&& other) noexcept {
        if (print_enabled)
            cout << "move-ctor " << x << endl;
        x = std::move(other.x);
        other.x = "__MOVE_DESTROYED__";
    };
    Marker& operator=(Marker&& other) noexcept {
        if (print_enabled)
            cout << "move-assign " << x << endl;
        x = std::move(other.x);
        other.x = "__MOVE_DESTROYED__";
        return *this;
    };

    ~Marker() {
        if (print_enabled)
            cout << "dtor " << x << endl;
    };

    virtual string get() const { return x; }

    static bool print_enabled;

protected:
    string x;
};


bool Marker::print_enabled = true;


#endif //EFFECTIVECPP_UTILS_H
