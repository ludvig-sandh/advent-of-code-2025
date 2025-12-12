#include "utils.hpp"

#include <sstream>

int main() {
    auto input = Util::LoadInput(Util::Day(6), Util::Part::A);
    std::stringstream s(input[0]);
    int a, b;
    s >> a >> b;
    std::cout << a << ", " << b << "\n";
}