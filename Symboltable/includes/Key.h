/*
 * Key.h
 *
 *  Created on: 24.10.2016
 *      Author: lara
 */

#ifndef KEY_H
#define KEY_H

class Key {
private:
	const unsigned int key;
public:
	Key(unsigned int key);
	unsigned int get_key() const;
};

#endif /* KEY_H */
