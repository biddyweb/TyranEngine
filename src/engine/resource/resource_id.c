#include <tyran_engine/resource/id.h>
#include <tyranscript/tyran_clib.h>

typedef struct debug_resource_info {
	char name[64];
	nimbus_resource_id resource_id;
} debug_resource_info;

int g_resource_names_count = 0;
debug_resource_info g_resource_names[128];

int nimbus_resource_id_debug_name_index(nimbus_resource_id id)
{
	for (int i = 0; i < g_resource_names_count; ++i) {
		if (g_resource_names[i].resource_id == id) {
			return i;
		}
	}
	return -1;
}

void nimbus_resource_id_debug_name_define(nimbus_resource_id id, const char* name)
{
	int index = nimbus_resource_id_debug_name_index(id);
	if (index == -1) {
		debug_resource_info* info = &g_resource_names[g_resource_names_count++];
		tyran_strcpy(info->name, 64, name);
		info->resource_id = id;
	}
}

const char* nimbus_resource_id_debug_name(nimbus_resource_id id)
{
	int index = nimbus_resource_id_debug_name_index(id);
	return g_resource_names[index].name;
}

nimbus_resource_id nimbus_resource_id_from_string(const char* name)
{
	const char* p;
	u32t h = 0;
	u32t g = 0;

	for (p = name; *p != '\0'; p = p + 1) {
		h = (h << 4) + *p;

		if ((g = h & 0xf0000000) != 0) {
			h = h ^ (g >> 24);
			h = h ^ g;
		}
	}

	nimbus_resource_id id = (nimbus_resource_id) h;

	nimbus_resource_id_debug_name_define(id, name);

	return id;
}
