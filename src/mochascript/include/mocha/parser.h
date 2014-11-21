#ifndef mocha_parser_h
#define mocha_parser_h

#include <stddef.h>

#include <mocha/context.h>
#include <mocha/string.h>
#include <mocha/error.h>
#include <mocha/values.h>

typedef struct mocha_parser {
	const mocha_char* input;
	const mocha_char* input_end;
	mocha_char* input_buffer;
	mocha_context* context;
	mocha_error error;
	mocha_values values;
} mocha_parser;

void mocha_parser_init(mocha_parser* self, mocha_context* context, const mocha_char* input, size_t input_length);
const struct mocha_object* mocha_parser_parse(mocha_parser* self, mocha_error* error);

#endif
