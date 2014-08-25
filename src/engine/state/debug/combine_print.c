#include <tyran_engine/state/debug/combine_print.h>
#include <tyran_engine/state/debug/component_print.h>
#include <tyran_engine/state/combine.h>

void nimbus_combine_print(const nimbus_combine* combine, const struct tyran_symbol_table* symbol_table)
{
        for (int i=0; i<combine->components_count; ++i) {
		  const struct nimbus_component* component = &combine->components[i];
		  nimbus_component_print(component, symbol_table);
        }
}
