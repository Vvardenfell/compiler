#include "../includes/Buffer.h"
#include <iostream>

int main(int argc, char **argv) {

	std::cout << ichhabkeinelustmehr() << std::endl;

}

bool ichhabkeinelustmehr() {
	char a[] = {"abcdefghijklmnopqrstuvwxyz0123456789"};
	Buffer test_buffer_size_5("testfile.txt", 5);
	int i = 0;

	try{
		while(true) {
			if(a[i] != test_buffer_size_5.get()) return false;
			i++;
		}
	} catch(...) {

	return true;
	}
}
