#include "opcodes.c"
#include <Windows/msvcrt.c>


void system_call_print_character(Process* process)
{
	process->data_stack -= sizeof(Number32);
	//print("%d ", *(Number32*)process->data_stack);
	print("%c", *process->data_stack);
}

void system_call_read_character(Process* process)
{
	Number32 character = _getch();
	
	Number32* top_value = process->data_stack;
	top_value[0] = character;
	process->data_stack += sizeof(Number32);
	
	//print("stack: %d\n", process->data_stack - process->program);
}

void (*system_calls[])(Process* process) = {
	&system_call_print_character,
	&system_call_read_character
};


void execute_command(Process* process)
{
	Opcodes opcode;
	
	opcode = *process->program_count;
	++process->program_count;
	
	if(opcode & 0b10000000) { //push/pop
		if(opcode & 0b01000000) { //pop
			process->data_stack -= opcode & 0b00111111;
		}
		else { //push
			Number32 value = opcode & 0b00011111;
			
			if(opcode & 0b00100000) {
				while(value) {
					*process->data_stack = *process->program_count;
					++process->data_stack;
					++process->program_count;
					--value;
				}
			}
			else {
				*(Number32*)process->data_stack = value;
				process->data_stack += sizeof(Number32);
			}
		}
	}
	else {
		switch(opcode) {
			case SET_DATA_ADDRESS_COMMAND: {
				process->data_stack -= sizeof(Number32);
				process->data_stack = process->program + *(Number32*)process->data_stack;
				//print("clean to %d\n", *(Number32*)process->data_stack);
				break;
			}
			
			case LOAD_COMMAND: {
				Number32* top_value = process->data_stack - sizeof(Number32);
				top_value[0] = *(Number32*)(process->program + top_value[0]);
				
				//print("load %d = %d\n", address, top_value[0]);
				break;
			}

			case STORE_COMMAND: {
				process->data_stack -= 2 * sizeof(Number32);
				Number32* top_value = process->data_stack;
				*(Number32*)(process->program + top_value[1]) = top_value[0];
				
				//print("store %d = %d\n", address, *(Number32*)(process->program + address));
				break;
			}
			
			case LOCAL_TO_GLOBAL_COMMAND: {
				Number32* top_value = process->data_stack - sizeof(Number32);
				top_value[0] = process->base - process->program + top_value[0];
				
				//print("local = %d, global = %d\n", address, top_value[0]);
				break;
			}

			case SYSTEM_CALL_COMMAND: {
				process->data_stack -= sizeof(Number32);
				Number32* top_value = process->data_stack;
				Number32 address = top_value[0];
				
				if(address < sizeof(system_calls) / sizeof(system_calls[0])) {
					system_calls[address](process);
				}

				break;
			}
			
			case BEGIN_CALL_COMMAND: {
				process->stack -= sizeof(Number32);
				*(Number32*)process->stack = process->data_stack;
				break;
			}
			
			case CALL_COMMAND: {
				process->data_stack -= sizeof(Number32);
				Number32* top_value = process->data_stack;
				
				Byte* old_base = process->base;
				
				process->base = *(Number32*)process->stack;
				//process->stack += sizeof(Number32);
				
				//process->stack -= sizeof(Number32);
				*(Number32*)process->stack = process->program_count;
				
				process->stack -= sizeof(Number32);
				*(Number32*)process->stack = old_base;
				
				process->program_count = process->program + top_value[0];
				
				//print("call %d\n", top_value[0]);
				break;
			}
			
			case RETURN_COMMAND: {
				process->base = *(Number32*)process->stack;
				process->stack += sizeof(Number32);
				
				process->program_count = *(Number32*)process->stack;
				process->stack += sizeof(Number32);
				
				break;
			}
			
			case RETURN_LINK_COMMAND: {
				*(Number32*)process->data_stack = process->base - process->program;
				process->data_stack += sizeof(Number32);

				process->base = *(Number32*)process->stack;
				process->stack += sizeof(Number32);
				
				process->program_count = *(Number32*)process->stack;
				process->stack += sizeof(Number32);
				break;
			}
			
			case JUMP_COMMAND: {
				process->data_stack -= sizeof(Number32);
				Number32* top_value = process->data_stack;
				process->program_count = process->program + top_value[0];
				break;
			}
			
			case JUMP_IF_ZERO_COMMAND: {
				process->data_stack -= 2 * sizeof(Number32);
				Number32* top_value = process->data_stack;
				
				if(!top_value[0]) {
					process->program_count = process->program + top_value[1];
				}
				break;
			}
			
			case JUMP_IF_NOT_ZERO_COMMAND: {
				process->data_stack -= 2 * sizeof(Number32);
				Number32* top_value = process->data_stack;
				
				if(top_value[0]) {
					process->program_count = process->program + top_value[1];
				}
				break;
			}
			
			case NOT_COMMAND: {
				Number32* top_value = process->data_stack - sizeof(Number32);
				top_value[0] = ~top_value[0];
				break;
			}
			
			case SHIFT_LEFT_COMMAND: {
				process->data_stack -= sizeof(Number32);
				Number32* top_value = process->data_stack - sizeof(Number32);
				top_value[0] = top_value[0] << top_value[1];
				break;
			}
			
			case SHIFT_RIGHT_COMMAND: {
				process->data_stack -= sizeof(Number32);
				Number32* top_value = process->data_stack - sizeof(Number32);
				top_value[0] = top_value[0] >> top_value[1];
				break;
			}
			
			case OR_COMMAND: {
				process->data_stack -= sizeof(Number32);
				Number32* top_value = process->data_stack - sizeof(Number32);
				top_value[0] = top_value[0] | top_value[1];
				break;
			}
			
			case AND_COMMAND: {
				process->data_stack -= sizeof(Number32);
				Number32* top_value = process->data_stack - sizeof(Number32);
				top_value[0] = top_value[0] & top_value[1];
				break;
			}
			
			case NEG_COMMAND: {
				Number32* top_value = process->data_stack - sizeof(Number32);
				top_value[0] = -top_value[0];
				break;
			}
			
			case ADD_COMMAND: {
				process->data_stack -= sizeof(Number32);
				Number32* top_value = process->data_stack - sizeof(Number32);
				//print("%d + %d\n", top_value[0], top_value[1]);
				top_value[0] = top_value[0] + top_value[1];
				break;
			}
			
			case SUB_COMMAND: {
				process->data_stack -= sizeof(Number32);
				Number32* top_value = process->data_stack - sizeof(Number32);
				top_value[0] = top_value[0] - top_value[1];
				break;
			}
			
			case MUL_COMMAND: {
				process->data_stack -= sizeof(Number32);
				Number32* top_value = process->data_stack - sizeof(Number32);
				top_value[0] = top_value[0] * top_value[1];
				break;
			}
			
			case DIV_COMMAND: {
				Number32* top_value = process->data_stack - 2 * sizeof(Number32);

				Number32 a = top_value[0];
				Number32 b = top_value[1];
				
				top_value[0] = a / b;
				top_value[1] = a % b;
				break;
			}
			
			case EQUAL_COMMAND: {
				process->data_stack -= sizeof(Number32);
				Number32* top_value = process->data_stack - sizeof(Number32);
				top_value[0] = top_value[0] == top_value[1];
				break;
			}
			
			case NOT_EQUAL_COMMAND: {
				process->data_stack -= sizeof(Number32);
				Number32* top_value = process->data_stack - sizeof(Number32);
				top_value[0] = top_value[0] != top_value[1];
				break;
			}
			
			case LESSER_COMMAND: {
				process->data_stack -= sizeof(Number32);
				Number32* top_value = process->data_stack - sizeof(Number32);
				top_value[0] = top_value[0] < top_value[1];
				break;
			}
			
			case LESSER_OR_EQUAL_COMMAND: {
				process->data_stack -= sizeof(Number32);
				Number32* top_value = process->data_stack - sizeof(Number32);
				top_value[0] = top_value[0] <= top_value[1];
				break;
			}
			
			case GREATER_COMMAND: {
				process->data_stack -= sizeof(Number32);
				Number32* top_value = process->data_stack - sizeof(Number32);
				top_value[0] = top_value[0] > top_value[1];
				break;
			}
			
			case GREATER_OR_EQUAL_COMMAND: {
				process->data_stack -= sizeof(Number32);
				Number32* top_value = process->data_stack - sizeof(Number32);
				top_value[0] = top_value[0] >= top_value[1];
				break;
			}
			
			default: {
				process->state = PROCESS_WAITING;
			}
		}
	}
}