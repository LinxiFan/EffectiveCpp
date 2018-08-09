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
struct StaticDog  { static char name[4]; };
struct NestedDog  { struct Inner; };

struct SFish : public Fish {
    string serialize() const {
        return "Fish-serial:" + name;
    }
};
struct SCat : public Cat {
    string serialize() const {
        return "Cat-serial:" + name;
    }
};
struct SDog : public Dog {
    string serialize() const {
        return "Dog-serial:" + name;
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
string smart_serialize1(const T& obj) {
    if constexpr (decltype(is_serializable(obj))::value) {
        return obj.serialize();
    }
    else {
        return any_str(obj);
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
    return maybe_serialize(obj).value_or(any_str(obj));
}

template<typename T>
string smart_serialize3(const T& obj) {
    auto ans = maybe_serialize(obj);
    if constexpr (hana::is_just(ans)) {
//        return ans.value();  // must if-constexpr, otherwise .value() won't compile
        return *ans;  // equivalent to ans.value()
    }
    else {  // hana::is_nothing
        return any_str(obj);
    }
}

template<typename T>
string smart_serialize4(T&& obj) {
    return hana::if_(
        is_serializable(obj),
        [](auto&& obj) { return obj.serialize(); },
        [](auto&& obj) { return any_str(obj); }
    )(std::forward<T>(obj));
}

template<typename T>
string smart_serialize5(T&& obj) {
    return hana::eval_if(
        is_serializable(obj),
        // explanation for `_` see below
        [&](auto _) { return _(obj).serialize(); },
        [&](auto _) { return any_str(_(obj)); }
    );
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


/*
 * hana compile time branching
 * vector can be constructed by either ()-ctor or {}-ctor, prefer the () one
 */
template <typename T, typename ...Args>
std::unique_ptr<T> my_make_unique1(Args&&... args) {
    return hana::if_(std::is_constructible<T, Args...>{},
        [](auto&& ...x) { return std::unique_ptr<T>(new T(std::forward<Args>(x)...)); },
        [](auto&& ...x) { return std::unique_ptr<T>(new T{std::forward<Args>(x)...}); }
    )(std::forward<Args>(args)...);
}

/*
 * make the bodies of the lambdas artificially dependent on something,
 * to prevent the compiler from being able to perform semantic analysis
 * before the lambda is actually used. To make this possible,
 * hana::eval_if will call the selected branch with an identity function
 * (a function that returns its argument unchanged), if the branch accepts such an argument
 */
template <typename T, typename ...Args>
std::unique_ptr<T> my_make_unique2(Args&&... args) {
    return hana::eval_if(std::is_constructible<T, Args...>{},
        // `_` will be passed an identity function
        [&](auto _) { return std::unique_ptr<T>(new T(std::forward<Args>(_(args))...)); },
        [&](auto _) { return std::unique_ptr<T>(new T{std::forward<Args>(_(args))...}); }
    );
}

int main() {
    auto animals = hana::make_tuple(Fish{"Nemo"}, Cat{"Garfield"}, Dog{"Snoopy"});
    ptype(animals);

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
    ptype(no_pointers);
    static_assert(no_pointers == hana::make_tuple(hana::type_c<Cat&>, hana::type_c<Dog>), "");

    auto has_member = hana::is_valid([](auto&& x) -> decltype(x.name) { });
    BOOST_HANA_CONSTANT_CHECK(has_member(snoopy));
    BOOST_HANA_CONSTANT_CHECK(!has_member(3.5));

    // if not checking an object instance, just the type
    auto has_member_type = hana::is_valid([](auto t) -> decltype(
        (void) hana::traits::declval(t).name  // declval obtains a placeholder instantiation (rvalue) of the type
    ) { });
    BOOST_HANA_CONSTANT_CHECK(has_member_type(hana::type_c<Dog>));  // C++14 variable template
    BOOST_HANA_CONSTANT_CHECK(!has_member_type(hana::type_c<vector<int>>));

    ptype<decltype(hana::type_c<StaticDog>)>();  // prints a boost::hana built-in type
    ptype<decltype(hana::type_c<StaticDog>)::type>();  // prints `StaticDog`
    // check static member
    auto has_static_member = hana::is_valid([](auto t) -> decltype(
        (void) decltype(t)::type::name  // decltype(hana::type_c<X>)::type extracts the actual c++ type
    ) { });
    BOOST_HANA_CONSTANT_CHECK(has_static_member(hana::type_c<StaticDog>));
    BOOST_HANA_CONSTANT_CHECK(!has_static_member(hana::type_c<vector<float>>));

    auto has_nested = hana::is_valid([](auto t) -> hana::type<
        typename decltype(t)::type::Inner
    > { });
    BOOST_HANA_CONSTANT_CHECK(has_nested(hana::type_c<NestedDog>));
    BOOST_HANA_CONSTANT_CHECK(!has_nested(hana::type_c<Dog>));

    ptitle("smart_serialize experiments");
    cout << smart_serialize1(Fish{"leviathan"}) << endl;
    cout << smart_serialize1(SFish{"leviathan"}) << endl;
    cout << smart_serialize2(Dog {"innu"}) << endl;
    cout << smart_serialize2(SDog {"innu"}) << endl;
    cout << smart_serialize3(Cat {"kitty"}) << endl;
    cout << smart_serialize3(SCat {"kitty"}) << endl;
    cout << smart_serialize4(Fish {"dory"}) << endl;
    cout << smart_serialize4(SFish {"dory"}) << endl;
    cout << smart_serialize5(Cat {"neko"}) << endl;
    cout << smart_serialize5(SCat {"neko"}) << endl;

    ptype<decltype(is_serializable(SFish {"me"}))>();
    cout << "SFish decltype::value" << decltype(is_serializable(SFish {"me"}))::value << endl;

    auto maybe_add = hana::sfinae([](auto x, auto y) -> decltype(x + y) {
        return x + y;
    });
    auto ans1 = maybe_add(1, 2); // hana::just(3)
    std::vector<int> v;
    auto ans2 = maybe_add(v, "foobar"); // hana::nothing
    ptype(ans1);
    ptype(ans2);
    static_assert(hana::is_nothing(ans2));

    cout << "hana:if_ experiments" << endl;
    auto tup1 = hana::if_(hana::true_c,
        hana::make_tuple('t', 'r', 'u', 'e'),
        hana::make_tuple('f', 'a', 'l', 's', 'e')
    );
    hana_print(tup1);
    BOOST_HANA_CONSTANT_CHECK(hana::not_(hana::true_c) == hana::false_c);

    cout << *my_make_unique1<vector<int>>(5, 7) << endl;  // invokes ()-ctor
    cout << *my_make_unique1<vector<int>>(5, 7, -2, 3) << endl;  // invokes {}-ctor
    cout << *my_make_unique2<vector<int>>(5, 7) << endl;  // invokes ()-ctor
    cout << *my_make_unique2<vector<int>>(5, 7, -2, 3) << endl;  // invokes {}-ctor
    cout << *my_make_unique2<vector<double>>(4, 3.1415) << endl;  // invokes ()-ctor
    cout << *my_make_unique2<vector<double>>(4.0, 3.1415, -2.1, 3.22, -6.98) << endl;  // invokes {}-ctor

}

