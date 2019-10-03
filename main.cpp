#include <fstream>
#include <vector>
#include <list>
#include <map>
#include <functional>
#include <stdexcept>
#include <iostream>

int main()
{
    std::string s{ "123456789" };
    auto it = s.begin();
    for(int i = 0 ; i < 3; i++) it++;
    s.erase(s.begin(), it);
    std::cout << s << std::endl;
    return 0;
}