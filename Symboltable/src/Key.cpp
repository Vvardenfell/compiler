/*
 * Key.cpp
 *
 *  Created on: 24.10.2016
 *      Author: lara
 */

#include "../includes/Key.h"

/*
 * Creates a new key.
 *
 * @param the key to be stored
 */
Key::Key(unsigned int key) : key(key) {}


/*
 * Gets the key.
 *
 * @return the key as unsigned integer
 */
unsigned int Key::get_key() const {
	return this->key;
}

