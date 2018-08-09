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


template <typename... Args1, typename... Args2>
constexpr decltype(auto) operator+(const std::tuple<Args1...> &tup1,
    const std::tuple<Args2...> &tup2) {
    return std::tuple_cat(tup1, tup2);
}

template <typename T, typename... Ts>
constexpr decltype(auto) tuple_multi_concat(T&& arg, Ts&&... rest) {
    if constexpr (sizeof ...(rest) == 0) {
        return arg;
    }
    else {
        return arg + tuple_multi_concat(std::forward<Ts>(rest) ...);
    }
}

/*
 * tuple_multi_concat2 is the right way to go.
 * However, due to compiler bug in clang 4.0 (Xcode 9.0), this wouldn't compile
 * Throws compile error "'operator<<' that is neither visible in
 * the template definition nor found by argument-dependent lookup"
 * https://stackoverflow.com/questions/45569698/clang-cant-find-template-binary-operator-in-fold-expression
 */
template <typename... Args>
constexpr decltype(auto) tuple_multi_concat2(Args &&... args) {
    return (args + ...);
}


int main() {
    cout << left_sum(Elem{"hello"}, Elem{"my"}, Elem{"world"}, Elem{"yo"}) << endl;
    cout << right_sum(Elem{"hello"}, Elem{"my"}, Elem{"world"}, Elem{"yo"}) << endl;
    vector<int> vec {3, 5, 6, 10, 2};
    cout << multi_push(vec, -5, 7, 18, 200) << endl;

    // template fold magic
    cout << any_str("hello ", 3.1415, " my ", -20, -1.11f) << endl;
    cout << join_str(" <-> ", "hello", 3.1415, "my", -20, -1.11f) << endl;

    auto tup1 = std::make_tuple(1, "tup1"s);
    auto tup2 = std::make_tuple("tup2"s, -3.1415, "hello"s);
    auto tup3 = std::make_tuple("tup3"s, 501, -30);
    ptuple(tuple_multi_concat(tup1, tup3, tup2));
    ptuple(tuple_multi_concat(tup3, tup1, tup2, tup1, tup3));
}

