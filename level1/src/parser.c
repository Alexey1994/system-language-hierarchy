typedef struct {
	Token  token;
	Number line_number;
}
Node;


typedef struct {
	Token  token;
	Number line_number;
	Link*  link;
}
Link_Node;


typedef struct {
	Token  token;
	Number line_number;
	Number value;
}
Push_Number_Node;

typedef struct {
	Token  token;
	Number line_number;
	Byte*  link_name;
	Link*  link;
}
Push_Link_Node;

typedef struct {
	Token  token;
	Number line_number;
	Number number_of_bytes_to_pop;
}
Pop_Node;

typedef struct {
	Token  token;
	Number line_number;
	Number value;
}
Number_Node;


Token token;


Node* create_node(Number size)
{
	Node* node = allocate_memory(size);
	
	node->token = token;
	node->line_number = line_number;
	
	return node;
}


Node* parse_instruction()
{
	switch(token) {
		case LINK_TOKEN: {
			Link_Node* new_node = create_node(sizeof(Link_Node));
			new_node->link = create_link(name_value.data);
			
			if(!new_node->link) {
				error(line_number, "ссылка %s уже определена", name_value.data);
			}
			
			token = read_next_token();
			return new_node;
		}
		
		case NUMBER_TOKEN: {
			Number_Node* new_node = create_node(sizeof(Node));
			new_node->value = const_number_value;

			token = read_next_token();
			
			return new_node;
		}
		
		case PUSH_TOKEN: {
			token = read_next_token();
			
			switch(token) {
				case NUMBER_TOKEN: {
					Push_Number_Node* new_node = create_node(sizeof(Push_Number_Node));
					new_node->token = PUSH_NUMBER_TOKEN;
					new_node->value = const_number_value;
					
					token = read_next_token();
					
					return new_node;
				}
					
				case NAME_TOKEN: {
					Push_Link_Node* new_node = create_node(sizeof(Push_Link_Node));
					new_node->token = PUSH_LINK_TOKEN;
					new_node->link_name = name_value.data;
					new_node->link = 0;
					
					token = read_next_token();
					
					return new_node;
				}
					
				default: {
					error(line_number, "ожидалось число или ссылка после push");
				}
			}
		}
		
		case POP_TOKEN: {
			Pop_Node* new_node = create_node(sizeof(Pop_Node));
			
			token = read_next_token();
			
			switch(token) {
				case NUMBER_TOKEN: {
					if(const_number_value < 64) {
						new_node->number_of_bytes_to_pop = const_number_value;
					}
					else {
						error(line_number, "нельзя извлечь больше 63 байт");
					}
					
					token = read_next_token();
						
					break;
				}
					
				default: {
					error(line_number, "ожидалось число после pop");
				}
			}

			return new_node;
		}
		
		case SET_DATA_ADDRESS_TOKEN:
		case LOAD_TOKEN:
		case STORE_TOKEN:
		case LOCAL_TO_GLOBAL_TOKEN:

		case SYSTEM_CALL_TOKEN:
		case BEGIN_CALL_TOKEN:
		case CALL_TOKEN:
		case RETURN_TOKEN:
		case RETURN_LINK_TOKEN:

		case JUMP_TOKEN:
		case JUMP_IF_ZERO_TOKEN:
		case JUMP_IF_NOT_ZERO_TOKEN:
			
		case NOT_TOKEN:
			
		case OR_TOKEN:
		case AND_TOKEN:
		case SHIFT_LEFT_TOKEN:
		case SHIFT_RIGHT_TOKEN:
			
		case NEG_TOKEN:
			
		case ADD_TOKEN:
		case SUB_TOKEN:
		case MUL_TOKEN:
		case DIV_TOKEN:
			
		case EQUAL_TOKEN:
		case NOT_EQUAL_TOKEN:
		case LESSER_TOKEN:
		case LESSER_OR_EQUAL_TOKEN:
		case GREATER_TOKEN:
		case GREATER_OR_EQUAL_TOKEN: {
			Node* new_node = create_node(sizeof(Node));

			token = read_next_token();
			
			return new_node;
		}
		
		default: {
			print_token(token);
			error(line_number, "не тот токен");
		}
	}
}


void parse_program(Stack* program)
{
	Node* instruction;
	
	token = read_next_token();

	while(token != EOF_TOKEN) {
		instruction = parse_instruction();
		add_bytes_in_stack(program, &instruction, sizeof(instruction));
	}
}