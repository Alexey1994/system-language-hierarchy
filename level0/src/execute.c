#include <types.c>
#include <heap.c>
#include <file.c>
#include "process.c"


Number main()
{
	Byte*  program;
	Number program_size;
	Byte*  console;
	
	console = get_default_console_reader();
	program = allocate_memory(65536);

	for(program_size = 0; program_size < 65536; ++program_size) {
		if(!read_bytes_from_file(console, program + program_size, 1)) {
			break;
		}
	}

	execute(program, program_size, 65536);
	execute_processes();

	return 0;
}