/**
 * Chapter 2. auto
 * 1. Prefer auto to explicit type declaration
 * 2. Use the explicitly typed initializer idiom when auto deduces undesired types
 */
#include <iostream>
#include <vector>
#include <unordered_map>
#include <boost/type_index.hpp>
#include "utils.h"

using namespace std;

class Dummy {
    int x;
public:
    Dummy(int x) : x(x) {}
    Dummy(const Dummy& other)
    : x(other.x) {
        cout << "copy ctor " << x << endl;
    }

    int get() const { return x; }
};

ostream& operator<<(ostream& oss, const Dummy& dummy) {
    oss << dummy.get();
    return oss;
}


int main() {
    unordered_map<string, Dummy> mymap {
        {"a", Dummy(10)},
        {"b", Dummy(20)},
        {"c", Dummy(30)}
    };

    cout << Dummy{50} << endl;

    cout << "wrong:" << endl;
    for (const pair<string, Dummy>& p: mymap) {
        // WRONG, should be pair<const string, Dummy>, so will invoke copy ctor
        cout << p.first << "->" << p.second << endl;
    }
    cout << "correct should be:" << endl;
    for (const auto& p: mymap) {
        cout << p.first << "->" << p.second << endl;
    }
    cout << "C++17 structure binding" << endl;
    for (const auto& [key, value]: mymap) {
//        print_type(key);
        cout << key << "->" << value << endl;
    }

    // vector<bool> is a specialization of vector, has special behavior
    vector<bool> mybools {true, true, false, false, true};
    auto wrong_bool = mybools[3]; // proxy object
    auto right_bool = static_cast<bool>(mybools[3]);
    print_type(right_bool);

    return 0;
}
