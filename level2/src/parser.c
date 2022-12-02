typedef struct {
	Token  token;
	Number line_number;
	Byte*  function_name;
}
Node;

typedef struct {
	Token  token;
	Number line_number;
	Byte*  function_name;
	Byte*  code;
}
Assembly_Code_Node;

typedef struct {
	Token  token;
	Number line_number;
	Byte*  function_name;
	Link*  link;
}
Link_Node;

typedef struct {
	Token  token;
	Number line_number;
	Byte*  function_name;
	Number value;
}
Const_Number_Node;

typedef struct {
	Token  token;
	Number line_number;
	Byte*  function_name;
	Byte*  bytes;
	Number number_of_bytes;
	Byte   id[16];
}
Const_String_Node;

typedef struct {
	Token     token;
	Number    line_number;
	Byte*     function_name;
	Variable* variable;
}
Variable_Node;

typedef struct {
	Token  token;
	Number line_number;
	Byte*  function_name;
	Link*  link;
}
Goto_Node;

typedef struct {
	Token  token;
	Number line_number;
	Byte*  function_name;
	Node*  child;
}
Unary_Expression_Node;

typedef struct {
	Token  token;
	Number line_number;
	Byte*  function_name;
	Node*  left;
	Node*  right;
}
Binary_Expression_Node;

typedef struct {
	Token  token;
	Number line_number;
	Byte*  function_name;
	Node*  condition;
	Link*  link;
}
Branch_Node;

typedef struct {
	Token  token;
	Number line_number;
	Byte*  function_name;
	Number call_function_number;
	Stack  arguments;
	Number number_of_arguments;
}
System_Call_Node;

typedef struct {
	Token  token;
	Number line_number;
	Byte*  function_name;
	Byte*  call_function_name;
	Stack  arguments;
	Number number_of_arguments;
}
Call_Node;

typedef struct {
	Token  token;
	Number line_number;
	Byte*  function_name;
	Stack  body;
}
Block_Node;

typedef struct {
	Token  token;
	Number line_number;
	Byte*  function_name;
	Stack  body;
}
Else_Node;

typedef struct {
	Token  token;
	Number line_number;
	Byte*  function_name;
	Node*  condition;
	Node*  body;
	Node*  else_body;
	Byte   end_id[16];
	Byte   else_end_id[16];
}
If_Node;

typedef struct {
	Token  token;
	Number line_number;
	Byte*  function_name;
	Node*  condition;
	Node*  body;
	Byte   start_id[16];
	Byte   end_id[16];
}
While_Node;

typedef struct {
	Token  token;
	Number line_number;
	Byte*  function_name;
}
From_Node;

typedef struct {
	Token  token;
	Number line_number;
	Byte*  function_name;
}
To_Node;


Token token;
Stack const_strings;


Node* create_node(Number size)
{
	Node* node = allocate_memory(size);
	
	node->token = token;
	node->line_number = line_number;
	node->function_name = current_function->name;
	
	return node;
}


Node* assembly_code()
{
	if(token != ASM_TOKEN) {
		return 0;
	}
	
	Assembly_Code_Node* new_node = create_node(sizeof(Assembly_Code_Node));
	new_node->code = asm_value.data;

	token = read_next_token();
	
	return new_node;
}


Node* link_definition()
{
	if(token != LINK_TOKEN) {
		return 0;
	}
	
	if(find_link(name_value.data)) {
		error(line_number, "ссылка %s уже определена", name_value.data);
	}
	
	Link_Node* new_node = create_node(sizeof(Link_Node));
	new_node->link = find_or_create_link(name_value.data);

	token = read_next_token();
	
	return new_node;
}


Node* parse_expression();
Function* parse_function(Byte* name);


Node* parse_operand()
{
	switch(token) {
		case CONST_NUMBER_TOKEN: {
			Number const_number_value_copy = const_number_value;
			
			token = read_next_token();
			
			if(token == '(') {
				System_Call_Node* new_node = create_node(sizeof(System_Call_Node));
				new_node->token = SYSTEM_CALL_TOKEN;
				new_node->call_function_number = const_number_value_copy;

				token = read_next_token();
				
				initialize_stack(&new_node->arguments, 64);
				new_node->number_of_arguments = 0;
				
				while(token != ')') {
					Node* argument = parse_expression();
					add_bytes_in_stack(&new_node->arguments, &argument, sizeof(argument));
					
					if(token == ',') {
						token = read_next_token();
					}
				}
				
				if(token != ')') {
					error(line_number, "отсутствует )");
				}
				
				token = read_next_token();
				
				return new_node;
			}
			else {
				Const_Number_Node* new_node = create_node(sizeof(Const_Number_Node));
				new_node->token = CONST_NUMBER_TOKEN;
				new_node->value = const_number_value_copy;
				
				return new_node;
			}
		}
		
		case CONST_STRING_TOKEN: {
			Const_String_Node* new_node = create_node(sizeof(Const_String_Node));
			new_node->bytes = const_string_value.data;
			new_node->number_of_bytes = const_string_value.size;
			generate_id(new_node->id);
			
			add_bytes_in_stack(&const_strings, &new_node, sizeof(new_node));
			
			token = read_next_token();

			return new_node;
		}
		
		case NAME_TOKEN: {
			Byte* name = name_value.data;
			
			token = read_next_token();
			
			if(token == '(') {
				parse_function(name);
				
				Call_Node* new_node = create_node(sizeof(Call_Node));
				new_node->token = CALL_TOKEN;
				new_node->call_function_name = name;

				token = read_next_token();

				initialize_stack(&new_node->arguments, 64);
				new_node->number_of_arguments = 0;
				
				while(token != ')') {
					Node* argument = parse_expression();
					add_bytes_in_stack(&new_node->arguments, &argument, sizeof(argument));
					
					if(token == ',') {
						token = read_next_token();
					}
				}
				
				if(token != ')') {
					error(line_number, "отсутствует )");
				}
				
				token = read_next_token();
				
				return new_node;
			}
			else {
				Variable_Node* new_node = create_node(sizeof(Variable_Node));
				new_node->token = GLOBAL_VARIABLE_TOKEN;
				
				new_node->variable = find_variable(&current_function->arguments, name);
				
				if(!new_node->variable) {
					new_node->variable = find_variable(&current_function->returns, name);
				}
				
				if(!new_node->variable) {
					new_node->variable = find_variable(&current_function->local_variables, name);
				}
				
				if(!new_node->variable) {
					new_node->variable = create_variable(&current_function->local_variables, name);
				}
				
				return new_node;
			}
		}
		
		case '@': {
			Goto_Node* new_node = create_node(sizeof(Goto_Node));
			
			token = read_next_token();

			if(token != NAME_TOKEN) {
				error(line_number, "@ должен быть от имени ссылки");
			}
			
			new_node->link = find_or_create_link(name_value.data);
			token = read_next_token();
			
			return new_node;
		}
		
		case '~': {
			Unary_Expression_Node* new_node = create_node(sizeof(Unary_Expression_Node));

			token = read_next_token();

			new_node->child = parse_operand();

			return new_node;
		}
		
		case '-': {
			Unary_Expression_Node* new_node = create_node(sizeof(Unary_Expression_Node));
			new_node->token = UNARY_MINUS_OPERATION;

			token = read_next_token();

			new_node->child = parse_operand();

			return new_node;
		}
		
		case '[': {
			Unary_Expression_Node* new_node = create_node(sizeof(Unary_Expression_Node));

			token = read_next_token();

			new_node->child = parse_expression();

			if(token != ']') {
				error(line_number, "отсутствует ]");
			}
			
			token = read_next_token();
			return new_node;
		}
		
		case '&': {
			Unary_Expression_Node* new_node = create_node(sizeof(Unary_Expression_Node));
			new_node->token = GET_ADDRESS_OPERATION;

			token = read_next_token();

			new_node->child = parse_operand();

			return new_node;
		}
		
		case '(': {
			token = read_next_token();

			Node* new_node = parse_expression();
			
			if(token != ')') {
				error(line_number, "отсутствует )");
			}
			
			token = read_next_token();
			return new_node;
		}

		default: {
			print_token(token);
			error(line_number, "не тот токен");
		}
	}
}


Node* parse_arithmetic2()
{
	Node* new_node;

	new_node = parse_operand();
	
	while(
		token == '&'
		|| token == '*'
		|| token == '/'
	) {
		Binary_Expression_Node* arithmetic2_node = create_node(sizeof(Binary_Expression_Node));
		arithmetic2_node->left = new_node;

		token = read_next_token();
		
		arithmetic2_node->right = parse_operand();
		
		new_node = arithmetic2_node;
	}
	
	return new_node;
}


Node* parse_arithmetic1()
{
	Node* new_node;

	new_node = parse_arithmetic2();
	
	while(
		token == '|'
		|| token == SHIFT_LEFT_TOKEN
		|| token == SHIFT_RIGHT_TOKEN
		|| token == '+'
		|| token == '-'
	) {
		Binary_Expression_Node* arithmetic1_node = create_node(sizeof(Binary_Expression_Node));
		arithmetic1_node->left = new_node;

		token = read_next_token();
		
		arithmetic1_node->right = parse_arithmetic2();
		
		new_node = arithmetic1_node;
	}
	
	return new_node;
}


Node* parse_comparision()
{
	Node* new_node;

	new_node = parse_arithmetic1();
	
	while(
		token == EQUAL_TOKEN
		|| token == NOT_EQUAL_TOKEN
		|| token == '<'
		|| token == LESSER_OR_EQUAL_TOKEN
		|| token == '>'
		|| token == GREATHER_OR_EQUAL_TOKEN
	) {
		Binary_Expression_Node* expression_node = create_node(sizeof(Binary_Expression_Node));
		expression_node->left = new_node;

		token = read_next_token();
		
		expression_node->right = parse_arithmetic1();
		
		new_node = expression_node;
	}
	
	return new_node;
}


Node* parse_expression()
{
	Node* new_node;

	new_node = parse_comparision();
	
	while(
		token == ','
	) {
		Binary_Expression_Node* expression_node = create_node(sizeof(Binary_Expression_Node));
		expression_node->left = new_node;

		token = read_next_token();
		
		expression_node->right = parse_comparision();
		
		new_node = expression_node;
	}
	
	return new_node;
}


Node* assignment_or_branch_or_operand()
{
	Node* new_node;

	new_node = parse_expression();
	
	if(token == '=') {
		Binary_Expression_Node* assignment_node = create_node(sizeof(Binary_Expression_Node));
		assignment_node->left = new_node;

		token = read_next_token();
		
		assignment_node->right = parse_expression();
		
		new_node = assignment_node;
	}
	else if(token == ARROW_TOKEN || token == NOT_ARROW_TOKEN) {
		Branch_Node* branch_node = create_node(sizeof(Branch_Node));
		branch_node->condition = new_node;

		token = read_next_token();
		
		if(token != NAME_TOKEN) {
			error(line_number, "после => должно быть имя ссылки");
		}
		
		branch_node->link = find_or_create_link(name_value.data);
		
		new_node = branch_node;
	}
	else {
		if(new_node->token == LOCAL_VARIABLE_TOKEN) {
			new_node->token = DECLARE_VARIABLE_TOKEN;
		}
	}

	return new_node;
}


Node* statement();


Node* block()
{	
	if(token != '{') {
		return 0;
	}
	
	Block_Node* new_node = create_node(sizeof(Block_Node));
	initialize_stack(&new_node->body, 16);
	
	token = read_next_token();

	while(token != EOF_TOKEN && token != '}') {
		Node* block_statement = statement();
		add_bytes_in_stack(&new_node->body, &block_statement, sizeof(block_statement));
	}

	if(token != '}') {
		error(line_number, "отсутствует }");
	}
	
	token = read_next_token();
	
	return new_node;
}


Node* if_statement()
{
	if(token != IF_TOKEN) {
		return 0;
	}

	If_Node* new_node = create_node(sizeof(If_Node));
	generate_id(new_node->end_id);

	token = read_next_token();
	
	new_node->condition = parse_expression();
	
	new_node->body = block();
	if(!new_node->body) {
		new_node->body = statement();
	}
	
	if(token == ELSE_TOKEN) {
		token = read_next_token();

		generate_id(new_node->else_end_id);
		
		new_node->else_body = block();
		if(!new_node->else_body) {
			new_node->else_body = statement();
		}
	}
	else {
		new_node->else_body = 0;
	}

	return new_node;
}


Node* while_statement()
{
	if(token != WHILE_TOKEN) {
		return 0;
	}
	
	While_Node* new_node = create_node(sizeof(While_Node));
	generate_id(new_node->start_id);
	generate_id(new_node->end_id);
	
	token = read_next_token();
	
	new_node->condition = parse_expression();
	
	new_node->body = block();
	if(!new_node->body) {
		new_node->body = statement();
	}
	
	return new_node;
}


Node* from_statement()
{
	if(token != FROM_TOKEN) {
		return 0;
	}
	
	From_Node* new_node = create_node(sizeof(From_Node));
	
	do {
		token = read_next_token();
		
		if(token != NAME_TOKEN) {
			error(line_number, "ожидалось имя аргумента");
		}
		
		if(find_variable(&current_function->arguments, name_value.data)) {
			error(line_number, "переменная %s определена как аргумент", name_value.data);
		}
		
		if(find_variable(&current_function->returns, name_value.data)) {
			error(line_number, "переменная %s определена как возвращаемое значение", name_value.data);
		}
		
		if(find_variable(&current_function->local_variables, name_value.data)) {
			error(line_number, "переменная %s определена как локальная", name_value.data);
		}
		
		create_variable(&current_function->arguments, name_value.data);
		
		token = read_next_token();
	}
	while(token == ',');
	
	return new_node;
}


Node* to_statement()
{
	if(token != TO_TOKEN) {
		return 0;
	}
	
	To_Node* new_node = create_node(sizeof(To_Node));
	
	do {
		token = read_next_token();
		
		if(token != NAME_TOKEN) {
			error(line_number, "ожидалось имя возвращаемого значения");
		}
		
		if(find_variable(&current_function->arguments, name_value.data)) {
			error(line_number, "переменная %s определена как аргумент", name_value.data);
		}
		
		if(find_variable(&current_function->returns, name_value.data)) {
			error(line_number, "переменная %s определена как возвращаемое значение", name_value.data);
		}
		
		if(find_variable(&current_function->local_variables, name_value.data)) {
			error(line_number, "переменная %s определена как локальная", name_value.data);
		}
		
		create_variable(&current_function->returns, name_value.data);
		
		token = read_next_token();
	}
	while(token == ',');
	
	return new_node;
}


Node* statement()
{
	Node* node;
	
	node = assembly_code();
	
	if(node) {
		return node;
	}
	
	node = link_definition();
	
	if(node) {
		return node;
	}
	
	node = block();
	
	if(node) {
		return node;
	}
	
	node = if_statement();
	
	if(node) {
		return node;
	}
	
	node = while_statement();
	
	if(node) {
		return node;
	}
	
	node = from_statement();
	
	if(node) {
		return node;
	}
	
	node = to_statement();
	
	if(node) {
		return node;
	}
	
	return assignment_or_branch_or_operand();
}


Function* parse_function(Byte* name)
{
	Function* function;
	Function* old_function;
	Number    old_line_number;
	Byte      old_head_data;
	Number    old_head_size;
	Boolean   old_end_of_stream;
	File      old_file;

	function = find_function(name);
				
	if(function) {
		return function;
	}
	
	function = allocate_memory(sizeof(Function));
	add_bytes_in_stack(&functions, &function, sizeof(function));
	
	old_function = current_function;
	old_head_data = head_data;
	old_head_size = head_size;
	old_end_of_stream = end_of_stream;
	old_file = stream_file;
	
	current_function = function;
	Stack function_file_name;
	initialize_stack(&function_file_name, strlen(name));
	add_bytes_in_stack(&function_file_name, name, strlen(name));
	add_bytes_in_stack(&function_file_name, ".lv2", 5);
	stream_file = open_file(function_file_name.data);
	
	head_size = 0;
	end_of_stream = 0;
	
	if(!stream_file) {
		error(0, "файл %s не найден", function_file_name.data);
	}

	function->name = name;
	initialize_stack(&function->links, 16);
	initialize_stack(&function->arguments, 16);
	initialize_stack(&function->returns, 16);
	initialize_stack(&function->local_variables, 16);
	initialize_stack(&function->variables, 16);
	initialize_stack(&function->body, 16);
	
	old_line_number = line_number;
	line_number = 1;
	
	token = read_next_token();

	while(token != EOF_TOKEN) {
		Node* body_statement = statement();
		add_bytes_in_stack(&function->body, &body_statement, sizeof(body_statement));
	}
	
	current_function = old_function;
	line_number = old_line_number;
	head_data = old_head_data;
	head_size = old_head_size;
	end_of_stream = old_end_of_stream;
	stream_file = old_file;
	
	return function;
}