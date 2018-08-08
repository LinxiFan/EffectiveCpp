/*
 * Boost.Hana template magic
 * doc: http://boostorg.github.io/hana/
 * https://jguegant.github.io/blogs/tech/sfinae-introduction.html
 */

#include "utils.h"
#include <boost/hana.hpp>

namespace hana = boost::hana;

using namespace hana::literals;
struct Fish { string name; };
struct Cat  { string name; };
struct Dog  { string name; };

struct SFish : public Fish {
    string serialize() const {
        return "fish-serial:" + name;
    }
};
struct SCat : public Cat {
    string serialize() const {
        return "cat-serial:" + name;
    }
};
struct SDog : public Dog {
    string serialize() const {
        return "dog-serial:" + name;
    }
};


ostream& operator<<(ostream& oss, const Fish& x) {
    return oss << "Fish(" << x.name << ")";
}
ostream& operator<<(ostream& oss, const Cat& x) noexcept {
    return oss << "Cat(" << x.name << ")";
}
ostream& operator<<(ostream& oss, const Dog& x) noexcept {
    return oss << "Dog(" << x.name << ")";
}

/*
 * check if serialize exists
 */
auto is_serializable = hana::is_valid([](auto&& x) -> decltype((void) x.serialize()) { });

template<typename T>
string smart_serialize(const T& obj) {
    if constexpr (decltype(is_serializable(obj))::value) {
        return obj.serialize();
    }
    else {
        return sprint(obj);
    }
}

/*
 * Equivalent, using hana::sfinae compile time optional
 */
auto maybe_serialize = hana::sfinae([](auto&& x) -> decltype(x.serialize()) {
    return x.serialize();
});

template<typename T>
string smart_serialize2(const T& obj) {
    return maybe_serialize(obj).value_or(sprint(obj));
}

template<typename T>
string smart_serialize3(const T& obj) {
    auto ans = maybe_serialize(obj);
    if constexpr (hana::is_just(ans)) {
        return ans.value();  // must if-constexpr, otherwise .value() won't compile
    }
    else {  // hana::is_nothing
        return sprint(obj);
    }
}

template<typename T>
string hana_str(const T& tuple) {
    ostringstream oss;
    oss << "Hana<";
    hana::transform(tuple, [&oss](auto a) {
        oss << a << ", ";
        return 0;
    });
    string s = oss.str();
    return s.substr(0, s.size()-2) + ">";
}

template<typename T>
void hana_print(const T& tuple) {
    cout << hana_str(tuple) << endl;
}

int main() {
    auto animals = hana::make_tuple(Fish{"Nemo"}, Cat{"Garfield"}, Dog{"Snoopy"});
    print_type(animals);

    // Access tuple elements with operator[] instead of std::get.
    Dog snoopy = animals[2_c];
    // Perform high level algorithms on tuples (this is like std::transform)
    auto names = hana::transform(animals, [](auto a) {
        return a.name;
    });
    hana_print(hana::reverse(names));

    auto animal_types = hana::make_tuple(hana::type_c<Fish*>, hana::type_c<Cat&>, hana::type_c<Dog>);
    auto no_pointers = hana::remove_if(animal_types, [](auto a) {
        return hana::traits::is_pointer(a);
    });
    print_type(no_pointers);
    static_assert(no_pointers == hana::make_tuple(hana::type_c<Cat&>, hana::type_c<Dog>), "");

    auto has_name = hana::is_valid([](auto&& x) -> decltype(x.name) { });
    cout << has_name(snoopy) << endl;  // true
    cout << has_name(3.5) << endl;  // false

    cout << smart_serialize(Fish {"leviathan"}) << endl;
    cout << smart_serialize(SFish {"leviathan"}) << endl;
    cout << smart_serialize2(Dog {"wolf"}) << endl;
    cout << smart_serialize2(SDog {"wolf"}) << endl;
    cout << smart_serialize3(Cat {"kitty"}) << endl;
    cout << smart_serialize3(SCat {"kitty"}) << endl;

    print_type<decltype(is_serializable(SFish {"me"}))>();
    cout << "SFish decltype::value" << decltype(is_serializable(SFish {"me"}))::value << endl;

    auto maybe_add = hana::sfinae([](auto x, auto y) -> decltype(x + y) {
        return x + y;
    });
    auto ans1 = maybe_add(1, 2); // hana::just(3)
    std::vector<int> v;
    auto ans2 = maybe_add(v, "foobar"); // hana::nothing
    print_type(ans1);
    print_type(ans2);
    static_assert(hana::is_nothing(ans2));
}

