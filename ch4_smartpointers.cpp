/**
 * Chapter 4. Smart pointers
 * item 18 unique_ptr
 */
#include "utils.h"

using namespace std;


class Apple : public Marker {
public:
    Apple(int id, string x, string suffix1, int suffix2)
    : Marker("apple"s + x), id(id), suffix1(suffix1), suffix2(suffix2)
    {}

    string get() const override {
        return Marker::get() + "-"s + std::to_string(id) + suffix1 + std::to_string(suffix2);
    }
private:
    int id;
    string suffix1;
    int suffix2;
};

class Banana : public Marker {
public:
    Banana(double id, string x, double suffix1)
        : Marker("banana"s + x), id(id), suffix1(suffix1)
    {}
    string get() const override {
        return Marker::get() + "-"s + std::to_string(id) + std::to_string(suffix1);
    }
private:
    double id;
    double suffix1;
};

class Cherry : public Marker {
public:
    Cherry(string id, string x)
        : Marker("cherry"s + x), id(id)
    {}
    string get() const override {
        return Marker::get() + "-"s + id;
    }
private:
    string id;
};

auto custom_del = [](Marker* mp) {
    cout << "smart pointer custom deleter called" << endl;
    delete mp;
};

/**
 * Universal smart pointer factory method, VERY COOL!
 */
template<typename ArgT0, typename... ArgT>
auto make_fruit(ArgT0 arg0, ArgT... args) {
    unique_ptr<Marker, decltype(custom_del)> ptr(nullptr, custom_del);
    // is_same_v is equivalent to std::is_save<T1, T2>::value
    if constexpr (std::is_same_v<int, ArgT0>) {
        ptr.reset(new Apple(arg0, std::forward<ArgT>(args) ...));
    }
    else if constexpr (std::is_same_v<double, ArgT0>) {
        ptr.reset(new Banana(arg0, std::forward<ArgT>(args) ...));
    }
    else if constexpr (std::is_same_v<string, ArgT0> || std::is_same_v<const char*, ArgT0>) {
        ptr.reset(new Cherry(arg0, std::forward<ArgT>(args) ...));
    }
    else {
        throw std::runtime_error("unrecognized ArgT0");
    }
    return ptr;
}


int main() {
    Marker::print_enabled = true;

    unique_ptr<Marker, decltype(custom_del)> ptr(nullptr, custom_del);
    ptr.reset(new Marker("hello"));  // now unique_ptr owns the object

    auto apple_ptr = make_fruit(20, "hello", "a1", 777);
    cout << apple_ptr->get() << endl;
    auto banana_ptr = make_fruit(3.1415, "hello", 18.33);
    cout << banana_ptr->get() << endl;
    // unique_ptr can cast to shared_ptr
    shared_ptr<Marker> cherry_ptr = make_fruit("yoo", "hello");
    cout << "should be unique: " << cherry_ptr.unique() << endl;
    auto cherry_copy1 = cherry_ptr;
    auto cherry_copy2 = cherry_ptr;
    cout << cherry_ptr->get() << " use_count= " << cherry_ptr.use_count() << endl;

    // shared_ptr custom deleter is not part of the type
    shared_ptr<Marker> banana_ptr2 {new Banana(1.2, "b2", -0.5), custom_del};

    return 0;
}
