#include "resource_id.h"

nimbus_resource_id nimbus_resource_id_from_string(const char* name)
{
	const char* p;
	u32t h = 0;
	u32t g = 0;

	for(p = name; *p != '\0'; p = p + 1) {
		h = (h << 4) + *p;

		if ((g = h & 0xf0000000) != 0) {
			h = h ^ (g >> 24);
			h = h ^ g;
		}
	}

	return (nimbus_resource_id)h;
}
