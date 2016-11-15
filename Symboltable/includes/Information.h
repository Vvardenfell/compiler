/*
 * Information.h
 *
 *  Created on: 23.10.2016
 *      Author: lara
 */

#ifndef INFORMATION_H
#define INFORMATION_H

#include <cstddef>

class Information {
private:
	const char* lexem;
	const std::size_t size;

public:
	Information(const char* lexem, std::size_t length);

	const char* get_name() const;
	std::size_t get_size() const;
};

#endif /* INFORMATION_H */
