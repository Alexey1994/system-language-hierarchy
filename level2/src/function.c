typedef struct {
	Byte*  name;
	Stack  links;
	Stack  arguments;
	Stack  returns;
	Stack  local_variables;
	Stack  variables;
	Stack  body;
}
Function;


Stack     functions;
Function* current_function = 0;


Function* find_function(Byte* name)
{
	Number    i;
	Function* function;

	for(i = 0; i < functions.size; i += sizeof(Function*)) {
		function = *(Function**)(functions.data + i);

		if(!compare_null_terminated_bytes(function->name, name)) {
			return function;
		}
	}

	return 0;
}