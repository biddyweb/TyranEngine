#include <tyranscript/tyran_object_operator.h>
#include <tyranscript/tyran_value.h>

tyran_boolean tyran_object_operator_comparison(int comparison, tyran_value* a, tyran_value* b)
{
	tyran_boolean test;
	switch (comparison) {
		case 8:
			if (tyran_value_is_object(b)) {
				test = (a->data.object == b->data.object);
			} else {
				test = TYRAN_FALSE;
			}
			break;
		case 9:
			test = TYRAN_FALSE;
			break;
		case 10:
			test = TYRAN_FALSE;
			break;
		default:
			TYRAN_ERROR("Unknown comparison:%d", comparison);
			test = TYRAN_FALSE;
	}

	return test;
}
