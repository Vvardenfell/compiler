#ifndef BUFFER_H
#define BUFFER_H

#include <fstream>
#include <string>


class Buffer {
private:
	const static int BUFFER_SIZE = 4096;
	char *begin, *end, *next, *back_buffer_begin, *back_buffer_end;
	bool next_chunk_loaded, previous_chunk_loaded;
	std::size_t buffer_size;
	bool injected_newline, seek_double_buffer_size;

	std::ifstream source;

	void read_next_chunk();
	void read_previous_chunk();
	void read_next_chunk_on_demand();
	void read_previous_chunk_on_demand();
	void switch_buffer();
	void inject_newline();

public:
	explicit Buffer(const char* file, std::size_t buffer_size = BUFFER_SIZE);
	~Buffer();

	Buffer(const Buffer& source) = delete;
	Buffer(const Buffer&& source) = delete;
	Buffer& operator=(const Buffer& source) = delete;
	Buffer& operator=(const Buffer&& source) = delete;

	char get();
	char unget();
};

#endif /* BUFFER_H */
