/**
 * CRTP pattern: Curiously Recurring Template Pattern
 * Blog posts:
 * 1. https://www.fluentcpp.com/2017/05/12/curiously-recurring-template-pattern/
 * 2. https://www.fluentcpp.com/2017/05/16/what-the-crtp-brings-to-code/
 * 3. https://www.fluentcpp.com/2017/05/19/crtp-helper/
 *
 * A interesting point to note is that, although the CRTP uses inheritance, its usage of
 * it does not have the same meaning as other cases of inheritance.
 * In general, a class deriving from another class expresses that the derived class somehow
 * conceptually "is a" base class. The purpose is to use the base class in generic code,
 * and to redirect calls to the base class over to code in the derived class.
 *
 * With the CRTP the situation is radically different.
 * The derived class does not express the fact it "is a" base class. Rather, it expands
 * its interface by inherting from the base class, in order to add more functionality.
 * In this case it makes sense to use the derived class directly, and to never use the base class
 *
 * Therefore the base class is not the interface, and the derived class is not the
 * implementation. Rather, it is the other way around: the base class uses the derived
 * class methods (such as getValue and setValue). In this regard, the derived class offers
 * an interface to the base class. This illustrates again the fact that inheritance in the
 * context of the CRTP can express quite a different thing from classical inheritance.
 */
#include <iostream>
using namespace std;

/*
 * CRTP generic helper, use this to add functionality to derived classes
 * "template template parameter" to avoid diamond inheritance of death
 * https://en.cppreference.com/w/cpp/language/template_parameters
 * https://stackoverflow.com/questions/213761/what-are-some-uses-of-template-template-parameters
 */
template <typename T, template<typename> class _crtp_type>
struct CRTP
{
    // cast to the actual instance
    T& instance() { return static_cast<T&>(*this); }
    T const& instance() const { return static_cast<T const&>(*this); }

private:
    /*
     * "private-ctor-and-friend" trick to ensure the correct inheritance
     * The constructors of the derived class have to call the constructor of the base class
     * (even if you don’t write it explicitly in the code, the compiler will do his best
     * to do it for you). Since the constructor in the base class is private, no one can
     * access it except the friend classes. And the only friend class is the template class!
     * So if the derived class is different from the template class, the code doesn’t compile.
     */
    CRTP(){}
    friend _crtp_type<T>;
};


template<typename T>
struct MultOp : public CRTP<T, MultOp> {
    void mult(double multiplier) {
        double new_val = this->instance().get_value() * multiplier;
        this->instance().set_value(new_val);
    }
};


template<typename T>
struct SquareOp : public CRTP<T, SquareOp> {
    void square() {
        double old_val = this->instance().get_value();
        this->instance().set_value(old_val * old_val);
    }
};


struct MyScalar : MultOp<MyScalar>, SquareOp<MyScalar> {
    MyScalar(double val): val(val) {}

    double get_value() const {
        return val;
    }

    void set_value(double val) {
        this->val = val;
    }

private:
    double val;
};


int main() {
    MyScalar x{1.7};
    x.mult(10);
    cout << x.get_value() << endl;
    x.square();
    cout << x.get_value() << endl;
}
