# Changelog

## Stretch Goals
- Change from `resource_manager` global store to a scoped context approach (like Svelte stores). So that instead of one big store of global singletons like in Bevy, there can be one top level global one & then nested scoped ones in a stack that can be pushed in & popped out

## 0.0.2 - In Progress
- Generalized `CmakePresets.json` a bit more (left more options up to the user)
- Created `web_dev.sh` shell script using emscripten (config, build & run) and added emscripten support for `CMakeLists.txt`
- Removed application_commands struct and flattened all events using it
- Created color module (still needs polishing up)
- Added `use_component` functions to entity
- Added [info] log level
- Created webgpu renderer implementation of `fae::renderer`
- Polished up sdl module and added boolean flags for other SDL initializations
- Added `to_string` and feature enumeration helpers to webgpu module
- Finished desktop platform implementations of `get_sdl_webgpu_surface` function
- Polished up webgpu module and moved rendering update logic to rendering module



## 0.0.1 - 4/16/24

- Created application module. Allows for:
    - Interaction with the ECS API (create entities, query components, etc.)
    - Setting & getting global resources
    - Hooking up systems (plain old functions) to events (e.g. predefined application "update" and "start" events or custom user-created events).
    - Extending applications with plugins (functions that promise to mutate the application in some helpful way).
- Created windowing module (SDL windowing).
- Started rendering module. Allows for creation of a renderer with a screen clear color operation.
- Created logging module.
- Created input module.
- Created SDL module.
- Created WebGPU module using Dawn implementation.
