#include <reader.c>


void generate_node_code(Node* node, Boolean name_as_link)
{
	switch(node->token) {
		case ASM_TOKEN: {
			Assembly_Code_Node* n = node;
			Byte* code = n->code;
			
			for(; *code;) {
				if(*code == '%') {
					++code;
					
					if(*code == '%') {
						print("%c", '%');
						++code;
						continue;
					}
					
					Byte* variable_name = code;
						
					while(*code && *code != '%') {
						++code;
					}
						
					*code = '\0';
					++code;
						
					Variable* variable = find_variable(&current_function->variables, variable_name);
						
					if(!variable) {
						error(n->line_number, "переменная %s не найдена", variable_name);
					}
						
					print("%d", variable->index * 4);
				}
				else {
					print("%c", *code);
					++code;
				}
			}
			
			print("\n");
			break;			
		}

		case LINK_TOKEN: {
			Link_Node* n = node;
			print("%s:\n", n->link->name);
			break;
		}
		
		case '@': {
			Goto_Node* n = node;
			print("push %s jump\n", n->link->name);
			break;
		}
		
		case ARROW_TOKEN: {
			Branch_Node* n = node;
			generate_node_code(n->condition, 0);
			print("push %s jump_if_zero\n", n->link->name);
			break;
		}
		
		case NOT_ARROW_TOKEN: {
			Branch_Node* n = node;
			generate_node_code(n->condition, 0);
			print("push %s jump_if_not_zero\n", n->link->name);
			break;
		}

		case CONST_NUMBER_TOKEN: {
			print("push %d\n", ((Const_Number_Node*)node)->value);
			break;
		}
		
		case CONST_STRING_TOKEN: {
			print("push %s\n", ((Const_String_Node*)node)->id);
			break;
		}
		
		case GLOBAL_VARIABLE_TOKEN: {
			if(name_as_link) {
				print("push %d local_to_global\n", ((Variable_Node*)node)->variable->index * 4);
			}
			else {
				print("push %d local_to_global load\n", ((Variable_Node*)node)->variable->index * 4);
			}
			break;
		}
		
		case '~': {
			Unary_Expression_Node* n = node;
			generate_node_code(n->child, 0);
			
			print("not\n");
			break;
		}

		case UNARY_MINUS_OPERATION: {
			Unary_Expression_Node* n = node;
			generate_node_code(n->child, 0);
			
			print("negate\n");
			break;
		}
		
		case '[': {
			Unary_Expression_Node* n = node;
			generate_node_code(n->child, 0);
				
			if(!name_as_link) {
				print("load\n");
			}
			break;
		}
		
		case '|': {
			Binary_Expression_Node* n = node;
			generate_node_code(n->left, 0);
			generate_node_code(n->right, 0);
			
			print("or\n");
			break;
		}
		
		case '&': {
			Binary_Expression_Node* n = node;
			generate_node_code(n->left, 0);
			generate_node_code(n->right, 0);
			
			print("and\n");
			break;
		}
		
		case SHIFT_LEFT_TOKEN: {
			Binary_Expression_Node* n = node;
			generate_node_code(n->left, 0);
			generate_node_code(n->right, 0);
			
			print("shift_left\n");
			break;
		}
		
		case SHIFT_RIGHT_TOKEN: {
			Binary_Expression_Node* n = node;
			generate_node_code(n->left, 0);
			generate_node_code(n->right, 0);
			
			print("shift_right\n");
			break;
		}
		
		case '+': {
			Binary_Expression_Node* n = node;
			generate_node_code(n->left, 0);
			generate_node_code(n->right, 0);
			
			print("add\n");
			break;
		}
		
		case '-': {
			Binary_Expression_Node* n = node;
			generate_node_code(n->left, 0);
			generate_node_code(n->right, 0);
			
			print("sub\n");
			break;
		}
		
		case '*': {
			Binary_Expression_Node* n = node;
			generate_node_code(n->left, 0);
			generate_node_code(n->right, 0);
			
			print("multiply\n");
			break;
		}
		
		case '/': {
			Binary_Expression_Node* n = node;
			generate_node_code(n->left, 0);
			generate_node_code(n->right, 0);
			
			print("divide\n");
			break;
		}
		
		case EQUAL_TOKEN: {
			Binary_Expression_Node* n = node;
			generate_node_code(n->left, 0);
			generate_node_code(n->right, 0);

			print("equal\n");
			break;
		}
			
		case NOT_EQUAL_TOKEN: {
			Binary_Expression_Node* n = node;
			generate_node_code(n->left, 0);
			generate_node_code(n->right, 0);

			print("not_equal\n");
			break;
		}
			
		case '<': {
			Binary_Expression_Node* n = node;
			generate_node_code(n->left, 0);
			generate_node_code(n->right, 0);

			print("lesser\n");
			break;
		}
			
		case LESSER_OR_EQUAL_TOKEN: {
			Binary_Expression_Node* n = node;
			generate_node_code(n->left, 0);
			generate_node_code(n->right, 0);

			print("lesser_or_equal\n");
			break;
		}
			
		case '>': {
			Binary_Expression_Node* n = node;
			generate_node_code(n->left, 0);
			generate_node_code(n->right, 0);

			print("greater\n");
			break;
		}
			
		case GREATHER_OR_EQUAL_TOKEN: {
			Binary_Expression_Node* n = node;
			generate_node_code(n->left, 0);
			generate_node_code(n->right, 0);

			print("greater_or_equal\n");
			break;
		}
		
		case ',': {
			Binary_Expression_Node* n = node;
			
			if(name_as_link) {
				generate_node_code(n->right, name_as_link);
				
				if(n->right->token != ',') {
					print("store\n");
				}
				
				generate_node_code(n->left, name_as_link);
			
				if(n->left->token != ',') {
					print("store\n");
				}
			}
			else {
				generate_node_code(n->left, name_as_link);
				generate_node_code(n->right, name_as_link);
			}
			
			break;
		}
		
		case GET_ADDRESS_OPERATION: {
			Unary_Expression_Node* n = node;
			
			if(n->child->token == GLOBAL_VARIABLE_TOKEN) {
				generate_node_code(n->child, 1);
			}
			else {
				error(n->line_number, "& должен быть от имени переменной");
			}
			
			break;
		}
		
		case '=': {
			Binary_Expression_Node* n = node;
			
			generate_node_code(n->right, 0);
			generate_node_code(n->left, 1); //!!! здесь не должно быть вызова функции, так как значение в правом узле потеряется
			
			if(n->left->token != ',')  {
				print("store\n");
			}

			break;
		}
		
		case '{': {
			Block_Node* n = node;
			Number i;
	
			for(i = 0; i < n->body.size; i += sizeof(Node**)) {
				generate_node_code(*(Node**)(n->body.data + i), 0);
			}
			break;
		}
		
		case IF_TOKEN: {
			If_Node* n = node;
			
			generate_node_code(n->condition, 0);
			print("push %s jump_if_zero\n", n->end_id);
			
			generate_node_code(n->body, 0);
			
			if(n->else_body) {
				print("push %s jump\n%s:\n", n->else_end_id, n->end_id);
				generate_node_code(n->else_body, 0);
				print("%s:\n", n->else_end_id);
			}
			else {
				print("%s:\n", n->end_id);
			}

			break;
		}
		
		case WHILE_TOKEN: {
			While_Node* n = node;
			
			print("%s:\n", n->start_id);
			generate_node_code(n->condition, 0);
			print("push %s jump_if_zero\n", n->end_id);
			
			generate_node_code(n->body, 0);
			
			print("push %s jump\n%s:\n", n->start_id, n->end_id);
			
			break;
		}
		
		case SYSTEM_CALL_TOKEN: {
			System_Call_Node* n = node;
			
			Node*  argument;
			Number i;

			for(i = 0; i < n->arguments.size; i += sizeof(Node*)) {
				argument = *(Node**)(n->arguments.data + i);

				//!!! нужно разворачивать в локальные переменные макросом все внутренние вызовы
				generate_node_code(argument, 0);
			}

			print("push %d system_call\n", n->call_function_number);
			
			break;
		}
		
		case CALL_TOKEN: {
			Call_Node* n = node;
			
			Node*  argument;
			Number i;
			
			print("begin_call\n");
			
			Function* function = find_function(n->call_function_name);
			
			for(i = 0; i < function->returns.size; i += sizeof(void*)) {
				print("push 0\n");
			}
			
			if(n->arguments.size / sizeof(Node*) != function->arguments.size / sizeof(void*)) {
				error(n->line_number, "нужно %d аргументов, получено %d", function->arguments.size / sizeof(void*), n->arguments.size / sizeof(Node*));
			}

			for(i = 0; i < n->arguments.size; i += sizeof(Node*)) {
				argument = *(Node**)(n->arguments.data + i);
				
				//!!! нужно разворачивать в локальные переменные макросом все внутренние вызовы
				generate_node_code(argument, 0);
			}

			print("push %s call\n", n->call_function_name);
			break;
		}
		
		case DECLARE_VARIABLE_TOKEN:
		case FROM_TOKEN:
		case TO_TOKEN: {
			break;
		}
		
		default: {
			print_token(node->token);
			error(node->line_number, "не реализовано");
		}
	}
}


void copy_variables(Function* function, Stack* source)
{
	Number    i;
	Variable* variable;
	
	for(i = 0; i < source->size; i += sizeof(Variable*)) {
		variable = *(Variable**)(source->data + i);

		variable->index = function->variables.size / sizeof(Variable*);
		add_bytes_in_stack(&function->variables, &variable, sizeof(variable));
	}
}


void generate_code(Function* function)
{
	Number i;
	
	current_function = function; //clean
	
	copy_variables(function, &function->returns);
	copy_variables(function, &function->arguments);
	copy_variables(function, &function->local_variables);
	
	for(i = 0; i < function->local_variables.size; i += sizeof(Variable*)) {
		print("push 0\n");
	}
	
	for(i = 0; i < function->body.size; i += sizeof(Node**)) {
		Node* node = *(Node**)(function->body.data + i);
		
		if(node->token == GLOBAL_VARIABLE_TOKEN) {
			node->token = DECLARE_VARIABLE_TOKEN;
		}
		
		generate_node_code(node, 0);
	}
}