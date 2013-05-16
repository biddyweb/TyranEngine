#ifndef _TYRAN_OPCODE_IDS_H
#define _TYRAN_OPCODE_IDS_H

typedef enum {
	TYRAN_OPCODE_LD = 1,
	TYRAN_OPCODE_LDC,
	TYRAN_OPCODE_LDCU,
	TYRAN_OPCODE_ADD,
	TYRAN_OPCODE_DIV,
	TYRAN_OPCODE_MOD,
	TYRAN_OPCODE_MUL,
	TYRAN_OPCODE_POW,
	TYRAN_OPCODE_SUB,
	TYRAN_OPCODE_INDEX,
	TYRAN_OPCODE_INDEX_SET,
	TYRAN_OPCODE_ITER,
	TYRAN_OPCODE_NEXT,
	TYRAN_OPCODE_NEG,
	TYRAN_OPCODE_NOT,
	TYRAN_OPCODE_EQ,
	TYRAN_OPCODE_LT,
	TYRAN_OPCODE_LE,
	TYRAN_OPCODE_JB,
	TYRAN_OPCODE_JBLD,
	TYRAN_OPCODE_JMP,
	TYRAN_OPCODE_RET,
	TYRAN_OPCODE_CALL,
	TYRAN_OPCODE_NEW,
	TYRAN_OPCODE_SET,
	TYRAN_OPCODE_GET,
	TYRAN_OPCODE_FUNC,
	TYRAN_OPCODE_DEBUG,
	TYRAN_OPCODE_MAX_ID
} tyran_opcode_id;

#endif
