#include "vector.h"
#include <stdio.h>

template<typename T> const double Vector<T>::RESIZE_FACTOR = 1.4;

#include <vector>
#include <iostream>

int main() {
	//std::vector<char> vec;
	Vector<char> vec;
	int i = 1;

	for (int j = 0 ; j < 4000000; j++) {
		vec.push_back(static_cast<char>(j&0xff));
		if (j%1000==0)printf("%i\n",j);
	}


	std::cout << vec.size() << std::endl;
	for (auto iter = vec.begin(); iter != vec.end(); iter++) {
		printf("%i\n", *iter);
	}




return 0;
}

/*
909
548
164*/
