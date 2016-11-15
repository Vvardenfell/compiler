#include "../includes/Buffer.h"
#include <iostream>

int main(int argc, char **argv) {

	Buffer buffer("./testfile", 5);

	std::cout << "get" << std::endl;
	for (size_t i = 0; i < 20; i++) std::cout << buffer.get() << std::endl;

	std::cout << std::endl << "unget" << std::endl;
	for (size_t i = 0; i < 20; i++) std::cout << buffer.unget() << std::endl;

}
