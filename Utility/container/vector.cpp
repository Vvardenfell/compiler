#include "vector.h"
#include <stdio.h>

template<typename T> const double Vector<T>::RESIZE_FACTOR = 1.4;


int main() {
	
	Vector<int> vec;
	int i = 1;

	for (int j = 0 ; j < 20; j++) {
		vec.push_back(j);
	}


	//std::cout << vec.size() << std::endl;
	for (auto iter = vec.begin(); iter != vec.end(); iter++) {
		printf("%i\n", *iter);
	}




return 0;
}