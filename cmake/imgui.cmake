add_library(imgui STATIC)
add_library(imgui::imgui ALIAS imgui)

target_sources(imgui
    PRIVATE
        ${imgui_SOURCE_DIR}/imconfig.h
        ${imgui_SOURCE_DIR}/imgui.h
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_demo.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_internal.h
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/imstb_rectpack.h
        ${imgui_SOURCE_DIR}/imstb_textedit.h
        ${imgui_SOURCE_DIR}/imstb_truetype.h

        ${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.h
        ${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp

        ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.h
        ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_wgpu.h
        ${imgui_SOURCE_DIR}/backends/imgui_impl_wgpu.cpp
)

target_compile_definitions(imgui
	PUBLIC
		IMGUI_IMPL_WEBGPU_BACKEND_DAWN
)

target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR})
target_link_libraries(imgui PUBLIC
    SDL3::SDL3
    webgpu_dawn
    webgpu_cpp
)
