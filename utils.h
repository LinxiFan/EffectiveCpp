#ifndef EFFECTIVECPP_UTILS_H
#define EFFECTIVECPP_UTILS_H

#include <iostream>
#include <vector>
#include <unordered_map>
#include <memory>
#include <sstream>
#include <cassert>
#include <boost/type_index.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/fusion/adapted/std_tuple.hpp>
#include <boost/fusion/algorithm/iteration/for_each.hpp>

using namespace std;

// https://stackoverflow.com/questions/25654632/how-to-check-at-compile-time-if-a-function-is-called/25654759#25654759
// static_assert(false, ...) will result in compile error, so we fake a false type
// static_assert(dependent_false<T>::value, "error message");
template<class T> struct dependent_false : std::false_type {};

template <typename OstreamT, typename T>  // to work with both
OstreamT& operator<<(OstreamT& oss, const vector<T>& vec) {
    if constexpr (std::is_base_of_v<std::ostream, OstreamT>) {
        oss << "[";
        string delim = "";
        for (auto& i : vec) {
            oss << delim << i;
            delim = ", ";
        }
        oss << "]";
        return oss;
    }
    else {
        /* test with this:
         struct BadOstream {
            int operator<<(int shit) {return 0;}
         };
         */
        static_assert(dependent_false<T>::value, "vector must be written to ostream subclasses");
    }
}


/*
 * Convert anything to string, just like python print
 */
template<typename T, typename... Ts>
ostringstream& _any_str_helper(ostringstream& oss, T&& arg, Ts&& ... rest) {
    oss << arg;
    if constexpr (sizeof... (rest) > 0) {
        return _any_str_helper(oss, std::forward<Ts>(rest) ...);
    }
    else {
        return oss;
    }
}

// alternative way to write any_str_helper without if constexpr
ostringstream& _any_str_helper2(ostringstream& oss) {
    return oss;
}
template<typename T, typename... Ts>
ostringstream& _any_str_helper2(ostringstream& oss, T&& arg, Ts&& ... rest) {
    oss << arg;
    return _any_str_helper(oss, std::forward<Ts>(rest) ...);
}

template<typename... Ts>
string any_str(Ts&& ... args) {
    ostringstream oss;
    _any_str_helper(oss, std::forward<Ts>(args) ...);
    return oss.str();
}

/*
 * Due to compiler bug in clang 4.0 (Xcode 9.0), this wouldn't work with custom defined
 * operator<< with vector. Throws compile error "'operator<<' that is neither visible in
 * the template definition nor found by argument-dependent lookup"
 * https://stackoverflow.com/questions/45569698/clang-cant-find-template-binary-operator-in-fold-expression
 */
template<typename... Ts>
string any_str2(Ts&& ... args) {
    ostringstream oss;
    (oss << ... << args);
    return oss.str();
}

template<typename T, typename... Ts>
string join_str(string sep, T arg0, Ts&& ... args) {
    ostringstream oss;
    oss << arg0;
    if constexpr (sizeof ...(args) > 0) {
        oss << sep << join_str(sep, std::forward<Ts>(args) ...);
    }
    return oss.str();
}

/*
 * Convert tuple to string
 * https://stackoverflow.com/questions/23436406/converting-tuple-to-string
 */
template<typename TupleT>
string tuple_str(const TupleT& tup) {
    string result = "tuple<";
    boost::fusion::for_each(tup, [&result](auto &s) {
        result += boost::lexical_cast<string>(s) + ", ";
    });
    return result.substr(0, result.size()-2) + ">";
}

template<typename TupleT>
void ptuple(const TupleT& tup) {
    cout << tuple_str(tup) << endl;
}

template<typename T>
string type_str() {
    return boost::typeindex::type_id_with_cvr<T>().pretty_name();
}

/**
 * Work with raw type:
 * ptype<my_type>();
 */
template<typename T>
void ptype() {
    cout << "type= " << type_str<T>() << endl;
}

/**
 * Work with values:
 * ptype(my_object);
 */
template<typename T>
void ptype(const T& param) {
    cout << "type= " << type_str<T>() << endl;
}


void ptitle(string title) {
    cout << "========== " << title << " ==========" << endl;
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
