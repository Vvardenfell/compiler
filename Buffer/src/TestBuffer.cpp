#include "../includes/Buffer.h"
#include <iostream>
#include "../../Utility/exception/exception.h"



bool test1() { 
	
	std::cout << std::endl;
	std::cout << "test1" << std::endl;
	std::cout << std::endl;	
	
	char a[] = {"abcdefghijklmnopqrstuvwxyz0123456789"};
	Buffer test_buffer_size_5("testfile.txt", 5);
	int i = 0;

	try{
		while(true) {
			if(a[i] != test_buffer_size_5.get()) return false;
			std::cout << a[i] << std::endl;
			i++;
		}
	} catch(...) {

	return true;
	}
}

bool test2() {
	Buffer test_buffer_size_5("testfile.txt", 5);
	
	std::cout << std::endl;
	std::cout << "test2" << std::endl;
	std::cout << std::endl;

	std::cout << test_buffer_size_5.get() << std::endl; //a
	std::cout << test_buffer_size_5.get() << std::endl; //b
	std::cout << test_buffer_size_5.get() << std::endl; //c
	std::cout << test_buffer_size_5.get() << std::endl; //d
	std::cout << test_buffer_size_5.get() << std::endl; //e
	
	// Should be in back buffer
	
	std::cout << test_buffer_size_5.get() << std::endl; //f
	std::cout << test_buffer_size_5.get() << std::endl; //g
		
	std::cout << "Should be g" << std::endl;
		
	std::cout << test_buffer_size_5.unget() << std::endl; //g
	std::cout << test_buffer_size_5.unget() << std::endl; //f
		
	// Should be in front buffer
	std::cout << test_buffer_size_5.unget() << std::endl; //e
	std::cout << test_buffer_size_5.unget() << std::endl; //d
	std::cout << test_buffer_size_5.unget() << std::endl; //c
	std::cout << test_buffer_size_5.unget() << std::endl; //b
	std::cout << test_buffer_size_5.unget() << std::endl; //a

	return true;


}

bool test3() {
	Buffer test_buffer_size_5("testfile.txt", 5);

	std::cout << std::endl;
	std::cout << "test3" << std::endl;
	std::cout << std::endl;

	std::cout << test_buffer_size_5.get() << std::endl; //a
	std::cout << test_buffer_size_5.get() << std::endl; //b
	std::cout << test_buffer_size_5.get() << std::endl; //c
	std::cout << test_buffer_size_5.get() << std::endl; //d
	std::cout << test_buffer_size_5.get() << std::endl; //e
	
	// Should be in back buffer
	
	std::cout << test_buffer_size_5.get() << std::endl; //f
	std::cout << test_buffer_size_5.get() << std::endl; //g
		
	std::cout << "Should be g" << std::endl;
		
	std::cout << test_buffer_size_5.unget() << std::endl; //g
	std::cout << test_buffer_size_5.unget() << std::endl; //f
		
	// Should be in front buffer

	std::cout << test_buffer_size_5.unget() << std::endl; //e
	std::cout << test_buffer_size_5.unget() << std::endl; //d
	std::cout << test_buffer_size_5.unget() << std::endl; //c
	std::cout << test_buffer_size_5.unget() << std::endl; //b
	std::cout << test_buffer_size_5.unget() << std::endl; //a

	std::cout << test_buffer_size_5.unget() << std::endl; //a

	try {
	//	std::cout << test_buffer_size_5.unget() << std::endl;
	} catch (const BufferBoundsExceededException& buffer_test) {
		return true;
	}
	return false;

}

bool test4() {
	
	std::cout << std::endl;
	std::cout << "test4" << std::endl;
	std::cout << std::endl;
	
	Buffer test_buffer_size_5("testfile.txt", 5);

	std::cout << test_buffer_size_5.get() << std::endl;

	for(int i = 0 ; i < 5 ; i++) {
		std::cout << test_buffer_size_5.get() << std::endl;
	}

	//std::cout << "Should be EOL" << std::endl;
	//std::cout << buffer.get() << std::endl; 

	try {
	//	buffer.get();
	} catch (...) {
		return true;
	}
	return false;
}




int main() {

	std::cout << test1() << std::endl; // test get
	std::cout << test2() << std::endl; // test unget == size
	std::cout << test3() << std::endl; // test unget > get
	//std::cout << test4() << std::endl; // test injected \n
}

