typedef enum {
	PROCESS_EXECUTE,
	PROCESS_WAITING
}
Process_State;

typedef struct {
	struct Process* next;
	Process_State   state;
	Byte*           program;
	Number          program_size;
	Number          allocated_program_size;
	Byte*           program_count;
	Byte*           base;
	Byte*           stack;
	Byte*           data_stack;
}
Process;


Process* processes       = 0;
Process* current_process = 0;


#include "program.c"


void execute_processes()
{
	Boolean  all_processes_waiting;
	
	current_process = processes;
	all_processes_waiting = 1;
	
	for(;;) {
		if(current_process->state != PROCESS_WAITING) {
			all_processes_waiting = 0;
			execute_command(current_process);
		}

		current_process = current_process->next;
		
		while(!current_process) {
			while(all_processes_waiting) {
				//asm("hlt");
				Sleep(1);
			}

			current_process = processes;
			all_processes_waiting = 1;
		}
	}
}


void execute(Byte* program, Number program_size, Number allocated_program_size)
{
	Process* process;
	
	process = allocate_memory(sizeof(Process));
	process->next                   = processes;
	process->state                  = PROCESS_EXECUTE;
	process->program                = program;
	process->program_size           = program_size;
	process->allocated_program_size = allocated_program_size;
	process->program_count          = process->program;
	process->stack                  = process->program + process->allocated_program_size;
	process->data_stack             = process->program + process->program_size;
	process->base                   = process->data_stack;

	processes = process;
}