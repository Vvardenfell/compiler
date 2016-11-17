#include "unordered_map.h"
#include <string>
#include <iostream>
using namespace std;


bool test_collision_of_key(UnorderedMap<String, int> map) {
//Todo
}


bool test_load_factor(UnorderedMap<String, int> map) {
//TODO: Implement test
}

bool test_resize(UnorderedMap<String, int> map) {
//TODO...
}

bool test_pointer_after_resize(UnorderedMap<String, int> map) {
//TODO
}

bool test__pointer_pointer_after_resize(UnorderedMap<String, int> map) {
//TODO
}

bool test_positioning(UnorderedMap<String, int> map) {
String test = "test";
for (int i = 0; i < (52*256); i += 256)  {
	map.insert(test, i);
}

map.insert("test", 5);
return (true);
}







int main () {

UnorderedMap<String, int> test_map_default; //Standard Constructor
UnorderedMap<String, int> test_map_sized(10000); //Constructor
test_positioning(test_map_default);
//cout << "Ergebnis Positioning test = " << test_positioning(test_map_default) << endl;


}

