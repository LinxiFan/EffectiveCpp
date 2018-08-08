/**
 * Chapter 3. Moving to Modern C++
 * item 12: declare overriding functions override
 */
#include <iostream>
#include <vector>
#include <unordered_map>
#include "utils.h"

using namespace std;


class Widget {
public:
    using DataType = vector<double>;

    // lvalue reference qualifier
    virtual DataType& get() & {
        cout << "lvalue function" << endl;
        return values;
    }
    // rvalue reference qualifier
    virtual DataType get() const && {
        cout << "rvalue function" << endl;
        return std::move(values);
    }

protected:
    DataType values;
};


class SubWidget : public Widget {
public:
    // lvalue reference qualifier
    DataType& get() & override {
        cout << "sub lvalue function" << endl;
        return values;
    }
    // rvalue reference qualifier
    DataType get() const && override {  // const qualifier must match superclass
        cout << "sub rvalue function" << endl;
        return std::move(values);
    }
};


int main() {
    auto w = Widget();
    w.get();
    Widget().get();

    auto ws = SubWidget();
    ws.get();
    SubWidget().get();

    return 0;
}
