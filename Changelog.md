# Changelog

## Stretch Goals
- Change from `resource_manager` global store to a scoped context approach (like Svelte stores). So that instead of one big store of global singletons like in Bevy, there can be one top level global one & then nested scoped ones in a stack that can be pushed in & popped out

## 0.0.2 - In Progress

## 0.0.1 - 4/16/24

Created application module. Allows for:
- Interaction with the ECS API (create entities, query components, etc.)
- Setting & getting global resources
- Hooking up systems (plain old functions) to events (e.g. predefined application "update" and "start" events or custom user-created events).
- Extending applications with plugins (functions that promise to mutate the application in some helpful way).

Created windowing module (SDL windowing).

Started rendering module. Allows for creation of a renderer with a screen clear color operation.

Created logging module.

Created input module.

Created SDL module.

Created WebGPU module using Dawn implementation.
