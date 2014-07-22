# State

Keeps track of all Combines and a flat array of Components.
It also holds an array of all Component Definitions.

All combines in the same state uses the same component data array for components of the same type (tracks).

## Spawn
Spawning is the process of cloning the combines components in one state to another (the state that the engine is rendering).
The engine gets information of the complete state using `nimbus_state_update_send()`.

When cloning a component, all local references are rewritten, and all other references remain in place.
