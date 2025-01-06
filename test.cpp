#include "test.h"
#include "id.h"
#include <iostream>

Test::Test() {}

void Test::id()
{
    Id id1(10, 0);
    std::cout << id1;

    Id id2(10, 0);
    std::cout << id2;

    std::cout << "id1 == id2: " << std::boolalpha << (id1 == id2) << std::endl << std::noboolalpha;

    Id id3(20, 1);
    std::cout << id3;

    Id id4(20, 2);
    std::cout << id4;

    std::cout << id1.distance(id2) << std::endl;
    std::cout << id3.distance(id4) << std::endl;

    std::cout << "id3 == id4: " << std::boolalpha << (id3 == id4) << std::endl;
    std::cout << "id3 < id4: " << (id3 < id4) << std::endl << std::noboolalpha;


    Id id5;
    std::cout << "default: " << id5 << ", size: " << id5.getValue().size() << ", default size: " << gIdLength << std::endl;
}
