#include <tyranscript/tyran_string.h>
#include <tyranscript/tyran_config.h>

static tyran_string* create_string(tyran_memory_pool* string_pool)
{
	tyran_string* duplicate = TYRAN_MALLOC_TYPE(string_pool, tyran_string);
	return duplicate;
}

static void tyran_string_reserve(tyran_string* self, tyran_memory* memory, int char_count)
{
	self->buf = TYRAN_MALLOC_NO_POOL_TYPE_COUNT(memory, tyran_string_char, char_count);
	self->len = char_count;
}

void tyran_string_init(tyran_string* self, tyran_memory* memory, const tyran_string_char* chars, int char_count)
{
	tyran_string_reserve(self, memory, char_count);
	tyran_memcpy_type(tyran_string_char, self->buf, chars, char_count);
}

void tyran_string_free(tyran_string* str)
{
	tyran_free(str->buf);
	tyran_memory_pool_free(str);
}

const tyran_string* tyran_string_strdup(tyran_memory_pool* string_pool, tyran_memory* memory, const tyran_string* str)
{
	int len = str->len;
	tyran_string* duplicate = create_string(string_pool);
	tyran_string_init(duplicate, memory, str->buf, len);
	return duplicate;
}

const tyran_string* tyran_string_strdup_str(tyran_memory_pool* string_pool, tyran_memory* memory, const char* str)
{
	int len = tyran_strlen(str);

	tyran_string* duplicate = create_string(string_pool);
	tyran_string_reserve(duplicate, memory, len);

	for (int i = 0; i < len; ++i) {
		duplicate->buf[i] = str[i];
	}

	return duplicate;
}

const tyran_string* tyran_string_substr(tyran_memory_pool* string_pool, tyran_memory* memory, const tyran_string* str, int start, int len)
{
	int source_string_length = str->len;

	if (start < 0) {
		start += source_string_length;
	} else if (start >= source_string_length) {
		return tyran_string_strdup_str(string_pool, memory, "");
	}

	int characters_to_copy = source_string_length - start;
	characters_to_copy = characters_to_copy < len ? characters_to_copy : len;


	tyran_string* rr = create_string(string_pool);
	tyran_string_init(rr, memory, str->buf + start, characters_to_copy);

	return rr;
}

const tyran_string* tyran_string_strcpy(tyran_memory_pool* string_pool, tyran_memory* memory, const tyran_string* from)
{
	int len = from->len;

	tyran_string* rr = create_string(string_pool);
	tyran_string_init(rr, memory, from->buf, len);

	return rr;
}

void tyran_string_to_c_str(char* buf, int tyran_string_max_length, const tyran_string* str)
{
	int i;
	int len = str->len;

	for (i = 0; i < len && i < tyran_string_max_length - 1; ++i) {
		buf[i] = (char)str->buf[i];
	}
	buf[i] = 0;
}

const tyran_string* tyran_string_from_c_str(tyran_memory_pool* string_pool, tyran_memory* memory, const char* str)
{
	int len = tyran_strlen(str);
	tyran_string* target = create_string(string_pool);
	tyran_string_reserve(target, memory, len);
	tyran_string_length_type i;

	for (i = 0; i < len; ++i) {
		target->buf[i] = str[i];
	}

	return target;
}

int tyran_string_strcmp(const tyran_string* str1, const tyran_string* str2)
{
	int len1 = str1->len;
	int len2 = str2->len;

	int i, r;

	if (len1 != len2) {
		return len1 - len2;
	}

	for (i = 0; i < len1; ++i) {
		r = str1->buf[i] - str2->buf[i];
		if (r) {
			return r;
		}
	}
	return 0;
}

const tyran_string* tyran_string_strcat(tyran_memory_pool* string_pool, tyran_memory* memory, const tyran_string* str1, const tyran_string* str2)
{
	int len = str1->len + str2->len;
	tyran_string* rr = create_string(string_pool);
	tyran_string_reserve(rr, memory, len);
	tyran_memcpy_type(tyran_string_char, rr->buf, str1->buf, str1->len);
	tyran_memcpy_type(tyran_string_char, rr->buf + str1->len, str2->buf, str2->len);
	return rr;
}
