/*
 * Key.h
 *
 *  Created on: 24.10.2016
 *      Author: lara
 */

#ifndef SYMBOLTABLE_SRC_KEY_H_
#define SYMBOLTABLE_SRC_KEY_H_

class Key {
private:
	unsigned int key;
public:
	Key(unsigned int key);
	virtual ~Key();
	unsigned int getKey();
};

#endif /* SYMBOLTABLE_SRC_KEY_H_ */
