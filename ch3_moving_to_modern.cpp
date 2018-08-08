/**
 * Chapter 3. Moving to Modern C++
 * 1. Distinguish between () and {}
 */
#include <iostream>
#include <vector>
#include <unordered_map>
#include <boost/type_index.hpp>
#include <type_traits>
#include "utils.h"


class Dummy {
public:
    vector<int> vec;
    Dummy() : vec({1, 2, 3}) {}
    Dummy(initializer_list<int> init) : vec(init) {}
    Dummy(const Dummy& other) = delete;
};

ostream& operator<<(ostream& oss, const Dummy& dummy) {
    return oss << " Dummy" << dummy.vec;
}

template<typename T, typename... Ts>
T make_object_with_brace(Ts&& ... params) {
    T new_obj{forward<Ts>(params) ...};
    return new_obj;
};

template<typename T, typename... Ts>
T make_object_with_paren(Ts&& ... params) {
    T new_obj(forward<Ts>(params) ...);
    return new_obj;
};

// Ch3 item 9, typedef VS using
template<typename T>
using MyType = vector<T>;

// if you must use typedef instead of using, you have to use it inside a struct
template<typename T>
struct MyType2 {
    typedef vector<T> type;
};

// show a counter-example to justify the required `typename` for derived types
enum class ColorType { Red, Blue, Green };


ostream& operator<<(ostream& oss, const ColorType& color) {
    return oss << "Color[" << static_cast<int>(color) << "]";
}

// also support rvalue
//ostream& operator<<(ostream& oss, ColorType&& color) {
//    return oss << "Color[" << static_cast<int>(color) << "]";
//}

// a specialization of MyType2
template<>
struct MyType2<ColorType> {
    static ColorType type;
};
// MUST initialize outside!! Otherwise will complain about linker error
ColorType MyType2<ColorType>::type = ColorType::Red;


/***************** Item 11. Deleted Functions *****************/
template<typename T>
void myfunc(T*) { }

template<>
void myfunc(void*) = delete;

template<>
void myfunc(char*) = delete;


int main() {
    // calls default Ctor
    Dummy dum{};
    cout << "default ctor" << dum << endl;
    // {} is cast to integer 0
    Dummy dum2{{}};
    cout << "initializer_list with a single zero that's also brace-initialized" << dum2 << endl;
    // truly empty initializer list
    Dummy dum3({});
    cout << "empty initializer_list" << dum3 << endl;

    cout << make_object_with_brace<vector<int>>(5, -7) << endl;
    cout << make_object_with_brace<vector<int>>() << endl;
    cout << make_object_with_paren<vector<int>>(5, -7) << endl;
    cout << make_object_with_paren<vector<int>>() << endl;
    print_type(nullptr);

    MyType<int> vec{1, 2, 3};
    typename MyType2<int>::type vec2{11, 22, 33};  // derived type MUST have `typename`
    cout << vec << vec2 << endl;
    auto not_typename = MyType2<ColorType>::type;
    cout << not_typename << endl;  // see, this is not a typename!
    cout << MyType2<ColorType>::type << endl;

//    void* x; myfunc(x); // deleted function won't compile

    return 0;
}
