#include "../../level0/src/opcodes.c"


void print_address(Number32 address)
{
	print(
		"%c%c%c%c",
		address & 0xFF,
		(address >> 8) & 0xFF,
		(address >> 16) & 0xFF,
		(address >> 24) & 0xFF
	);
	/*switch(sizeof(Number)) {
		case 4:
			print(
				"%c%c%c%c",
				address & 0xFF,
				(address >> 8) & 0xFF,
				(address >> 16) & 0xFF,
				(address >> 24) & 0xFF
			);
			break;
		
		case 8:
			print(
				"%c%c%c%c%c%c%c%c",
				address & 0xFF,
				(address >> 8) & 0xFF,
				(address >> 16) & 0xFF,
				(address >> 24) & 0xFF,
				(address >> 32) & 0xFF,
				(address >> 40) & 0xFF,
				(address >> 48) & 0xFF,
				(address >> 56) & 0xFF
			);
			break;
	}*/
}


Number current_address = 0;

void calculate_links(Stack* program)
{
	Number i;
	Node*  node;
	
	for(i = 0; i < program->size; i += sizeof(Node*)) {
		node = *(Node**)(program->data + i);
		
		switch(node->token) {
			case LINK_TOKEN: {
				Link_Node* n = node;
				n->link->address = current_address;
				
				break;
			}
			
			case PUSH_NUMBER_TOKEN: {
				Push_Number_Node* n = node;
				
				if(n->value < 32) {
					++current_address;
				}
				else {
					current_address += 1 + sizeof(Number32);
				}
				
				break;
			}
			
			case PUSH_LINK_TOKEN: {
				Push_Link_Node* n = node;

				n->link = find_link(n->link_name);
				
				if(!n->link) {
					error(n->line_number, "ссылка %s не найдена", n->link_name);
				}
				
				//if(n->link->address < 32) {
				//	++current_address;
				//}
				//else {
				//	current_address += 1 + sizeof(Number32);
				//}
				
				current_address += 1 + sizeof(Number32);
				
				break;
			}
			
			case POP_TOKEN: {
				++current_address;
				break;
			}
			
			default: {
				++current_address;
			}
		}
	}
}


void generate(Stack* program)
{
	Number i;
	Node*  node;
	
	for(i = 0; i < program->size; i += sizeof(Node*)) {
		node = *(Node**)(program->data + i);
		
		switch(node->token) {
			case LINK_TOKEN: {
				break;
			}
			
			case NUMBER_TOKEN: {
				Number_Node* n = node;
				
				print("%c", n->value);
				
				break;
			}
			
			case SET_DATA_ADDRESS_TOKEN: print("%c", SET_DATA_ADDRESS_COMMAND); break;
			
			case PUSH_NUMBER_TOKEN: {
				Push_Number_Node* n = node;
				
				if(n->value < 32) {
					print("%c", 0b10000000 | n->value);
				}
				else {
					print("%c", 0b10100000 | 4);
					print_address(n->value);
				}
				
				break;
			}
			
			case PUSH_LINK_TOKEN: {
				Push_Link_Node* n = node;
				
				//if(n->link->address < 32) {
				//	print("%c", 0b10000000 | n->link->address);
				//}
				//else {
				//	print("%c", 0b10100000 | 4);
				//	print_address(n->link->address);
				//}
				
				print("%c", 0b10100000 | 4);
				print_address(n->link->address);
				
				break;
			}
			
			case POP_TOKEN: {
				Pop_Node* n = node;
				
				print("%c", 0b11000000 | n->number_of_bytes_to_pop);
				break;
			}

			case LOAD_TOKEN: print("%c", LOAD_COMMAND); break;
			case STORE_TOKEN: print("%c", STORE_COMMAND); break;
			case LOCAL_TO_GLOBAL_TOKEN: print("%c", LOCAL_TO_GLOBAL_COMMAND); break;

			case SYSTEM_CALL_TOKEN: print("%c", SYSTEM_CALL_COMMAND); break;
			case BEGIN_CALL_TOKEN: print("%c", BEGIN_CALL_COMMAND); break;
			case CALL_TOKEN: print("%c", CALL_COMMAND); break;
			case RETURN_TOKEN: print("%c", RETURN_COMMAND); break;
			case RETURN_LINK_TOKEN: print("%c", RETURN_LINK_COMMAND); break;

			case JUMP_TOKEN: print("%c", JUMP_COMMAND); break;
			case JUMP_IF_ZERO_TOKEN: print("%c", JUMP_IF_ZERO_COMMAND); break;
			case JUMP_IF_NOT_ZERO_TOKEN: print("%c", JUMP_IF_NOT_ZERO_COMMAND); break;
			
			case NOT_TOKEN: print("%c", NOT_COMMAND); break;
			
			case OR_TOKEN: print("%c", OR_COMMAND); break;
			case AND_TOKEN: print("%c", AND_COMMAND); break;
			case SHIFT_LEFT_TOKEN: print("%c", SHIFT_LEFT_COMMAND); break;
			case SHIFT_RIGHT_TOKEN: print("%c", SHIFT_RIGHT_COMMAND); break;
			
			case NEG_TOKEN: print("%c", NEG_COMMAND); break;
			
			case ADD_TOKEN: print("%c", ADD_COMMAND); break;
			case SUB_TOKEN: print("%c", SUB_COMMAND); break;
			case MUL_TOKEN: print("%c", MUL_COMMAND); break;
			case DIV_TOKEN: print("%c", DIV_COMMAND); break;
			
			case EQUAL_TOKEN: print("%c", EQUAL_COMMAND); break;
			case NOT_EQUAL_TOKEN: print("%c", NOT_EQUAL_COMMAND); break;
			case LESSER_TOKEN: print("%c", LESSER_COMMAND); break;
			case LESSER_OR_EQUAL_TOKEN: print("%c", LESSER_OR_EQUAL_COMMAND); break;
			case GREATER_TOKEN: print("%c", GREATER_COMMAND); break;
			case GREATER_OR_EQUAL_TOKEN: print("%c", GREATER_OR_EQUAL_COMMAND); break;
		}
	}
}