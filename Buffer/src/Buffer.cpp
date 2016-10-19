#include "../includes/Buffer.h"
#include "../../Utility/exception/exception.h"


Buffer::Buffer(const char* file) : source(file) {

	start = end = next = new char[BUFFER_SIZE];
		
	if (!(source.isOpen())) {
		throw BufferInitializationException(std::string("Failed to initialize Buffer, because the file \"") + std::string(file) + std::string("\" couldn't be opened!")); 
	}

}

void Buffer::read_next_chunk() {

	source.read(start , BUFFER_SIZE);
	end = start + source.gcount();
	next = start;

	if (start == end) {
		throw BufferBoundsExceededException("Exceeded bounds of file while trying to read chunk!");
	}

}

void Buffer::read_previous_chunk() {
	source.seekg(-((end - start) + BUFFER_SIZE), std::ios_base::cur);
	read_next_chunk();
	next = end;
}


char Buffer::get() {
	
	if (next == end) {
		read_next_chunk();
	}
	
	return *(next++);
}

char Buffer::unget() {

	if (next == start) {
		read_previous_chunk();
	}
	
	return *(--next);
}

Buffer::~Buffer() {
	delete[] start;
}
