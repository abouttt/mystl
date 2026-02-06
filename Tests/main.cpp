#include <iostream>

#include "mystl/utility.h"

int main()
{
    auto p = mystl::make_pair(1, 3.14);
    std::cout << '(' << mystl::get<0>(p) << ", " << mystl::get<1>(p) << ")\n";
    std::cout << '(' << mystl::get<int>(p) << ", " << mystl::get<double>(p) << ")\n";
}
