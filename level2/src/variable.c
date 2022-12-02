#include <memory.c>
#include <heap.c>


typedef struct {
	Byte*  name;
	Number index;
}
Variable;


Variable* find_variable(Stack* variables, Byte* name)
{
	Number    i;
	Variable* variable;

	for(i = 0; i < variables->size; i += sizeof(Variable*)) {
		variable = *(Variable**)(variables->data + i);

		if(!compare_null_terminated_bytes(variable->name, name)) {
			return variable;
		}
	}

	return 0;
}


Variable* create_variable(Stack* variables, Byte* name)
{
	Variable* variable;

	variable = allocate_memory(sizeof(Variable));
	variable->name = name;
	variable->index = variables->size / sizeof(Variable*);
	add_bytes_in_stack(variables, &variable, sizeof(variable));
	
	return variable;
}


Variable* find_or_create_variable(Stack* variables, Byte* name)
{
	Variable* variable;

	variable = find_variable(variables, name);

	if(!variable) {
		variable = create_variable(variables, name);
	}

	return variable;
}