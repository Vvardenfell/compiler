/*
 * Information.h
 *
 *  Created on: 23.10.2016
 *      Author: lara
 */

#ifndef SYMBOLTABLE_SRC_INFORMATION_H_
#define SYMBOLTABLE_SRC_INFORMATION_H_

class Information {
private:
	char* lexem;
	int size;

public:
	Information(char* lexem, int length);
	virtual ~Information();

	bool compareLexem(char* lexem);
	char* getName();
	int getSize();
};

#endif /* SYMBOLTABLE_SRC_INFORMATION_H_ */
