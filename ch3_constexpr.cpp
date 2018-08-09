/**
 * Chapter 3. Moving to Modern C++
 * item 15 constexpr
 */
#include <iostream>
#include <vector>
#include <unordered_map>
#include "utils.h"

using namespace std;


constexpr int pow(int base, int exp) noexcept {
    int result = 1;
    for (int i = 0; i<exp; ++i) {
        result *= exp;
    }
    return result;
}


class Point {
public:
    constexpr Point(double x = 0, double y = 0) noexcept
    : x(x), y(y)
    {}

    constexpr double getX() const noexcept { return x; }
    constexpr double getY() const noexcept { return y; }
    constexpr void setX(double newx) { x = newx; }
    constexpr void setY(double newy) { y = newy; }

private:
    double x, y;
};


constexpr Point midpoint(const Point& p1, const Point& p2) noexcept {
    return {
        (p1.getX() + p2.getX()) / 2,
        (p1.getY() + p2.getY()) / 2
    };
}


constexpr Point reflect(const Point& p) {
    Point ans;
    ans.setX(-p.getX());
    ans.setY(-p.getY());
    return ans;
}

/**
 * COOL: use if constexpr to avoid SFINAE or clever meta-template hacks
 */
template<int N, typename T, typename... Ts>
auto variadic_get(T x, Ts ... xs) {
    if constexpr (N > sizeof...(xs)) {
        throw std::runtime_error("N larger than number of args");
    }
    else if constexpr (N > 0) {
        return variadic_get<N - 1>(xs ...);
    }
    else {
        return x;
    }
}


// we print a compile-time double number up to two decimal points
template<int int_part, int decimal_part>
constexpr void _compile_print() {
    
    cout << int_part << "." << decimal_part << endl;
}

constexpr int _get_decimal(double x) {
    if (x < 0) {
        x *= -1;
    }
    int int_part = static_cast<int>(x);
    return static_cast<int>((x - int_part)*100);
}


#define compile_print(x) _compile_print<static_cast<int>(x), _get_decimal(x)>()


int main() {
    cout << "7**5 = " << pow(7, 5) << endl;

    constexpr Point p1(9.42, -21.3);
    constexpr Point p2(-6.5, 11.78);
    constexpr Point pmid = midpoint(p1, p2);
    constexpr Point pref = reflect(p2);

    compile_print(pmid.getX());
    compile_print(pmid.getY());
    compile_print(pref.getX());
    compile_print(pref.getY());

    cout << "Variadic hacking with if constexpr" << endl;
    ptype(variadic_get<0>(nullptr, "const char", 35, "my string"s, 4.556));
    ptype(variadic_get<1>(nullptr, "const char", 35, "my string"s, 4.556));
    ptype(variadic_get<2>(nullptr, "const char", 35, "my string"s, 4.556));
    ptype(variadic_get<3>(nullptr, "const char", 35, "my string"s, 4.556));
    ptype(variadic_get<4>(nullptr, "const char", 35, "my string"s, 4.556));

    return 0;
}
