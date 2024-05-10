# Changelog

## Stretch Goals
- Change from `resource_manager` global store to a scoped context approach (like Svelte stores). So that instead of one big store of global singletons like in Bevy, there can be one top level global one & then nested scoped ones in a stack that can be pushed in & popped.
- Create a Scriptable render pipeline-like thing (Like Unity). i.e. be able to describe different rendering pipelines with assets.
- Create Input Actions API. Be able to create actions (i.e. Jump, Shoot, Move, etc.) that map to any number inputs (keyboard, gamepad, etc.) but don't use string mappings. Something that is type safe or like Unity's new Input system/Unreal Enhanced Input. 

## 0.0.2 - In Progress
- Polished up build tools.
- Restructured and polished up APIs for `fae::application` & `fae::entity`.
- Created color module. Contains some predefined colors as well as RGB <-> HSV conversions.
- Created webgpu module.
    - Using Dawn (C++ implementation of WebGPU).
    - Created webgpu renderer implementation of a `fae::renderer`.
    - Created `get_sdl_webgpu_surface` function to extract a webgpu surface from an SDL window (only desktop platforms implemented).

## 0.0.1 - 4/16/24

- Created the core application module. Allows for:
    - Interaction with the ECS API (create entities, query components, etc.).
    - Setting & getting global resources.
    - Hooking up systems (plain old functions) to events (e.g. predefined application "start" and "update" events or custom user-created events).
    - Extending applications with plugins (functions that promise to mutate the application in some helpful way).
- Created SDL module (windowing, rendering, makes cross-platform development easier).
- Created windowing module (SDL windowing).
- Created rendering module. Abstracts the rendering pipeline and defines a render step where drawing can happen.
- Created logging module.
- Created input module (only keyboard input for now).
