#include "../includes/Buffer.h"
#include "../../Utility/exception/exception.h"

Buffer::Buffer(const char* file, std::size_t buffer_size) : source(file) {
	this->back_buffer_begin = this->back_buffer_end = static_cast<char*>(::operator new[](buffer_size << 1));
	this->begin = this->end = this->next = this->back_buffer_begin + buffer_size;
	this->next_chunk_loaded = this->previous_chunk_loaded = false;
	this->buffer_size = buffer_size;
	this->injected_newline = false;
	this->seek_double_buffer_size = false;

	if (!(this->source.is_open())) {
		throw BufferInitializationException(std::string("Failed to initialize Buffer, because the file \"") + std::string(file) + std::string("\" couldn't be opened!"));
	}
}

void Buffer::inject_newline() {
	this->injected_newline = true;
	if (this->buffer_size > 0) {
		*(this->begin) = '\n';
		++this->end;
	}
}

void Buffer::read_next_chunk() {
	this->source.read(this->begin, this->buffer_size);
	this->end = this->begin + this->source.gcount();
	this->next = this->begin;

	if (this->begin == this->end) {
		if (this->injected_newline || (source.tellg() < 0 && !source.eof())) throw BufferBoundsExceededException("Exceeded bounds of file while trying to read chunk!");
		else this->inject_newline();
	}
}

void Buffer::read_previous_chunk() {
	this->source.clear();
	this->source.seekg(-((this->back_buffer_end - this->back_buffer_begin) + (this->buffer_size << (this->seek_double_buffer_size ? 1 : 0))), std::ios_base::cur);
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
	        this->read_next_chunk();
		this->seek_double_buffer_size = true;
	    }
            if (this->back_buffer_begin != this->back_buffer_end) this->previous_chunk_loaded = true;
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
		this->read_previous_chunk();
		this->seek_double_buffer_size = false;
	    }
            if (this->back_buffer_begin != this->back_buffer_end) this->next_chunk_loaded = true;
	}
}

void Buffer::switch_buffer() {
    using std::swap;
    swap(this->begin, this->back_buffer_begin);
    swap(this->end, this->back_buffer_end);
}

char Buffer::get() {
    this->read_next_chunk_on_demand();
    return *(this->next++);
}

char Buffer::unget() {
    if (this->injected_newline) {
        this->injected_newline = false;
        return '\n';
    }
    else {
        this->read_previous_chunk_on_demand();
        return *(--this->next);
    }
}

Buffer::~Buffer() {
	::operator delete[](this->begin < this->back_buffer_begin ? this->begin : this->back_buffer_begin);
}
