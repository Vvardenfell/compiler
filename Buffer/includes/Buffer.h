#ifndef BUFFER_H
#define BUFFER_H

#include <fstream>
#include <string>


class Buffer {
private:
	const static int BUFFER_SIZE = 4096;
	char* start;
	char* end;
	char* next;

	std::ifstream source;

	void read_next_chunk();
	void read_previous_chunk();

public:
	explicit Buffer(const char* file);
	~Buffer();

	Buffer(const Buffer& source) = delete;
	Buffer(const Buffer&& source) = delete;
	Buffer& operator=(const Buffer& source) = delete;
	Buffer& operator=(const Buffer&& source) = delete;

	char get();
	char unget();
};

#endif /* BUFFER_H */
