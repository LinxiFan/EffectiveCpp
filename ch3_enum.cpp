/**
 * Chapter 3. Moving to Modern C++
 * item 10 enum
 */
#include "utils.h"


/* Use unscoped enum to index tuples */
using UserInfo = tuple<string, string, int>;  // name, email, score
enum UserInfoFields {uiName, uiEmail, uiScore};
enum class CUserInfoFields {uiName, uiEmail, uiScore};


// get the underlying integral type of Enum as a compile time constant
template<typename E>
constexpr std::underlying_type_t<E>  // equivalent in C++11: typename underlying_type<E>::type
    getEtype(E enumerator) noexcept {
    return static_cast<std::underlying_type_t<E>>(enumerator);
}


int main() {
    UserInfo myinfo {"DrFord", "yoyo@gmail.com", 777};
    cout << std::get<uiEmail>(myinfo) << endl;
    cout << std::get<getEtype(uiScore)>(myinfo) << endl;
    return 0;
}
