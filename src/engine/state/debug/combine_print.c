#include <tyran_engine/state/debug/combine_print.h>
#include <tyran_engine/state/debug/component_print.h>

void nimbus_combine_print(const nimbus_combine* combine)
{
        for (int i=0; i<combine->components_count; ++i) {
                nimbus_component* component = combine->components[i];
                nimbus_component_print(component);
        }
}
