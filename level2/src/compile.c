#include <types.c>


Number last_id = 1;

void write_id(Byte** id_string, Number id)
{
	if(!id) {
		return;
	}

	write_id(id_string, id / 10);
	**id_string = id % 10 + '0';
	++*id_string;
}

void generate_id(Byte* id_string)
{
	id_string[0] = '_';
	++id_string;
	write_id(&id_string, last_id);
	*id_string = '\0';
	
	++last_id;
}


#include <memory.c>
#include <data-structures/stack.c>
#include <string.c>

#include "file-stream.c"
#include "function.c"
#include "link.c"
#include "variable.c"
#include "tokener.c"
#include "parser.c"
#include "generator.c"


void remove_extension(Byte* name)
{
	Byte* original_name = name;
	
	while(*name) {
		++name;
	}
	
	--name;
	
	while(*name != '.' && name > original_name) {
		--name;
	}
	
	*name = '\0';
}


Number main(Number number_of_arguments, Byte** arguments)
{
	Byte*     file_name;
	Function* function;
	Number    i;
	
	if(number_of_arguments < 2) {
		error(0, "нужно имя файла функции");
	}
	
	file_name = arguments[1];
	remove_extension(file_name);

	initialize_stack(&functions, 1024);
	initialize_stack(&const_strings, 1024);

	parse_function(file_name);
	
	for(i = 0; i < functions.size; i += sizeof(Function*)) {
		function = *(Function**)(functions.data + i);
		
		if(i) {
			print("%s:\n", function->name);
		}
		
		generate_code(function);
		print("return\n");
	}
	
	for(i = 0; i < const_strings.size; i += sizeof(Const_String_Node*)) {
		Const_String_Node* node = *(Const_String_Node**)(const_strings.data + i);
		
		print("%s:", node->id);
		
		Number j;
		for(j = 0; j < node->number_of_bytes; ++j) {
			print("%d ", node->bytes[j]);
		}
		print("%d\n", 0);
	}

	return 0;
}