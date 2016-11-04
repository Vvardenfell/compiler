#include "../includes/Buffer.h"
#include "../../Utility/exception/exception.h"


Buffer::Buffer(const char* file, std::size_t buffer_size) : source(file) {
	this->back_buffer_begin = this->back_buffer_end = static_cast<char*>(::operator new(buffer_size << 1));
	this->begin = this->end = this->next = this->back_buffer_begin + buffer_size;
	this->next_chunk_loaded = this->previous_chunk_loaded = false;

	if (!(this->source.is_open())) {
		throw BufferInitializationException(std::string("Failed to initialize Buffer, because the file \"") + std::string(file) + std::string("\" couldn't be opened!"));
	}
}

void Buffer::read_next_chunk() {
	this->source.read(this->begin, buffer_size);
	this->end = this->begin + this->source.gcount();
	this->next = this->begin;

	if (this->begin == this->end) {
		throw BufferBoundsExceededException("Exceeded bounds of file while trying to read chunk!");
	}
}

void Buffer::read_previous_chunk() {
	this->source.seekg(-((this->end - this->begin) + buffer_size), std::ios_base::cur);
	this->read_next_chunk();
	this->next = this->end;
}

void Buffer::read_next_chunk_on_demand() {
    if (this->next == this->end) {
	    this->switch_buffer();
	    if (this->next_chunk_loaded) {
	        this->next_chunk_loaded = false;
	        this->next = this->begin;
	    }
	    else {
            this->previous_chunk_loaded = true;
	        this->read_next_chunk();
	    }
	}
}

void Buffer::read_previous_chunk_on_demand() {
    if (this->next == this->begin) {
	    this->switch_buffer();
	    if (this->previous_chunk_loaded) {
	        this->previous_chunk_loaded = false;
	        this->next = this->end;
	    }
	    else {
	        this->next_chunk_loaded = true;
            this->read_previous_chunk();
	    }
	}
}

void Buffer::switch_buffer() {
    std::swap(this->begin, this->back_buffer_begin);
    std::swap(this->end, this->back_buffer_end);
}

char Buffer::get() {
    this->read_next_chunk_on_demand();
	return *(this->next++);
}

char Buffer::unget() {
    this->read_previous_chunk_on_demand();
	return *(--this->next);
}

Buffer::~Buffer() {
	::operator delete(this->begin < this->back_buffer_begin ? this->begin : this->back_buffer_begin);
}
