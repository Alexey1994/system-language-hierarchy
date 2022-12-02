// hgfedcba - byte from high to low bits
// 0 - command
//  xxxxxxx - command index
// 1 - push/pop
//  0 - push
//   0 - value, padded to Number32
//    xxxxx
//   1 - size of value in bytes
//    xxxxx
//  1 - pop
//   xxxxxx - number of bytes to pop


typedef enum {
	SET_DATA_ADDRESS_COMMAND = 1,
	
	LOAD_COMMAND,
	STORE_COMMAND,
	LOCAL_TO_GLOBAL_COMMAND,

	SYSTEM_CALL_COMMAND,
	BEGIN_CALL_COMMAND,
	CALL_COMMAND,
	RETURN_COMMAND,
	RETURN_LINK_COMMAND,
	
	JUMP_COMMAND,
	JUMP_IF_ZERO_COMMAND,
	JUMP_IF_NOT_ZERO_COMMAND,
	
	NOT_COMMAND,
	SHIFT_LEFT_COMMAND,
	SHIFT_RIGHT_COMMAND,
	OR_COMMAND,
	AND_COMMAND,
	
	NEG_COMMAND,
	ADD_COMMAND,
	SUB_COMMAND,
	MUL_COMMAND,
	DIV_COMMAND,
	
	EQUAL_COMMAND,
	NOT_EQUAL_COMMAND,
	LESSER_COMMAND,
	LESSER_OR_EQUAL_COMMAND,
	GREATER_COMMAND,
	GREATER_OR_EQUAL_COMMAND
}
Opcodes;