#ifndef FILE_SREAM_INCLUDED
#define FILE_SREAM_INCLUDED


#include <file.c>


File    stream_file;
Byte    head_data;
Number  head_size = 0;
Boolean end_of_stream = 0;

void read_next_from_file()
{
	if(read_bytes_from_file(stream_file, &head_data, 1)) {
		++head_size;
	}
	else {
		head_data = 0;
		end_of_stream = 1;
	}
}

void next()
{
	if(head_size) {
		--head_size;
	}
	else {
		read_next_from_file();
	}
}

Byte head()
{
	if(!head_size) {
		read_next_from_file();
	}
	
	if(head_size) {
		return head_data;
	}
	else {
		return 0;
	}
}


#endif//FILE_SREAM_INCLUDED