/*
 * Boost.Hana template magic
 * We will implement compile time meta-switch for type selection from boost::any
 * doc: http://boostorg.github.io/hana/
 * https://jguegant.github.io/blogs/tech/sfinae-introduction.html
 */

#include "utils.h"
#include <boost/hana.hpp>
#include <boost/any.hpp>

namespace hana = boost::hana;

using namespace hana::literals;

struct Fish { string name; };
struct Cat  { string name; };
struct Dog  { string name; };

ostream& operator<<(ostream& oss, const Fish& x) {
    return oss << "Fish(" << x.name << ")";
}
ostream& operator<<(ostream& oss, const Cat& x) noexcept {
    return oss << "Cat(" << x.name << ")";
}
ostream& operator<<(ostream& oss, const Dog& x) noexcept {
    return oss << "Dog(" << x.name << ")";
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
 * associate each type to a function
 * pair.first is type and pair.second is actual function
 */
template<typename T>
auto case_ = [](auto f) {
    return hana::make_pair(hana::type_c<T>, f);
};

struct default_t;
auto default_ = case_<default_t>;

// base case: no more cases left, execute the default
template <typename Any, typename Default>
auto _switch_process(Any&, std::type_index const&, Default& default_) {
    return default_();
}
// recursion on ...Rest
template <typename Any, typename Default, typename Case, typename ...Rest>
auto _switch_process(Any& a, std::type_index const& t, Default& default_,
    Case& case_, Rest& ...rest)
{
    using T = typename decltype(+hana::first(case_))::type;
    // if Any's type matches the case type (first of the tuple), execute its associated function
    return t == typeid(T) ? hana::second(case_)(*boost::unsafe_any_cast<T>(&a))
        : _switch_process(a, t, default_, rest...);
}

/*
 * switch_(arg) returns a lambda function that does the type dispatching
 */
template<typename T>
auto switch_(T& arg) {
    return [&arg](auto ... cases_) {
        // put into a hana tuple to mainipulate
        auto cases = hana::make_tuple(cases_ ...);
        // find the default case first, returns hana::optional
        // predicate must be generic because of heterogenous typing and must return hana IntegralConstant
        // http://boostorg.github.io/hana/index.html#tutorial-algorithms-cross_phase
        auto default_ = hana::find_if(cases, [](auto const& c) {
            // recall: first of the case tuple is the type
            return hana::first(c) == hana::type_c<default_t>;  // same as c[1_c]
        });
        static_assert(!hana::is_nothing(default_), "hana switch_ is missing default case!");
        // remove the default case first
        auto rest = hana::filter(cases, [](auto const& c) {
            return hana::first(c) != hana::type_c<default_t>;
        });
        // unpack(args, f) is similar to python unpacking call f(*args)
        return hana::unpack(rest, [&](auto& ...rest) {
            return _switch_process(arg, arg.type(), hana::second(*default_), rest...);
        });
    };
}


int main() {
//    boost::any a = 1000;

    vector<boost::any> anys {
        'x', 1000, -3.1415f, 23.09, vector<int>{4, 22, -1, 0, 1}, Fish{"tako"}, Dog{"snoopy"}
    };

    for (auto& a : anys) {
        string r = switch_(a)(
            case_<int>([](auto i) { return "int: "s + std::to_string(i); }),
            case_<char>([](auto c) { return "char: "s + string{c}; }),
            case_<float>([](auto f) { return "float: "s + std::to_string(f); }),
            case_<vector<int>>([](const auto& vec) { return "vector<int>: "s + any_str(vec); }),
            case_<Fish>([](const auto& x) { return "my fish: "s + any_str(x); }),
            default_([] { return "unknown type"s; })
        );
        cout << r << endl;
    }
}

