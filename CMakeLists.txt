cmake_minimum_required(VERSION 3.29.3)

project(
    fae
    VERSION 0.0.2
    LANGUAGES CXX
    DESCRIPTION "fae game framework (core library of the fae game engine)"
)

# options
set(FAE_CPM_VERSION "v0.39.0" CACHE STRING "Which version of CPM to use (tag or \"master\")")
option(FAE_ENABLE_WARNINGS "Enable compilation warnings" OFF)
option(FAE_ENABLE_WARNINGS_AS_ERRORS "Treat warnings as errors" OFF)
option(FAE_BUILD_EXAMPLES "Build examples" ON)
# TODO option(FAE_BUILD_TESTS "Build tests" OFF)
# TODO option(FAE_BUILD_BENCHMARKS "Build benchmarks" OFF))
# TODO option(FAE_BUILD_DOCS "Build documentation" OFF)

# dependencies
include(cmake/get_cpm.cmake)
get_cpm(VERSION ${FAE_CPM_VERSION})
CPMUsePackageLock(package-lock.cmake)
CPMAddPackage("gh:g-truc/glm#1.0.1")
CPMAddPackage("gh:skypjack/entt#v3.13.2")
set(CPM_SDL_OPTIONS
	"EXCLUDE_FROM_ALL ON"
	"SYSTEM ON"
	# "SDL_DISABLE_INSTALL OFF"
)

if(${BUILD_SHARED_LIBS})
	set(CPM_SDL_OPTIONS
		${CPM_SDL_OPTIONS}
		"SDL_SHARED ON"
		"SDL_STATIC OFF"
	)
else()
	set(CPM_SDL_OPTIONS
		${CPM_SDL_OPTIONS}
		"SDL_SHARED OFF"
		"SDL_STATIC ON"
	)
endif()
CPMAddPackage(
	NAME SDL
	GITHUB_REPOSITORY libsdl-org/SDL
	GIT_TAG prerelease-3.1.2
	OPTIONS
		${CPM_SDL_OPTIONS}
)
if(NOT DEFINED EMSCRIPTEN)
	CPMAddPackage(
		NAME dawn
		GITHUB_REPOSITORY google/dawn
	 	GIT_TAG main
	 	OPTIONS
		  	"EXCLUDE_FROM_ALL ON"
		  	"SYSTEM ON"
			"DAWN_FETCH_DEPENDENCIES ON"
	)
endif()

configure_file(config.hpp.in "include/${PROJECT_NAME}/config.hpp")

add_library(${PROJECT_NAME})
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

set_target_properties(${PROJECT_NAME}
    PROPERTIES
        CXX_STANDARD 23
        CXX_STANDARD_REQUIRED ON
		CXX_EXTENSIONS OFF
        COMPILE_WARNING_AS_ERROR ${FAE_ENABLE_WARNINGS_AS_ERRORS}
        LINKER_LANGUAGE CXX
)

if(DEFINED EMSCRIPTEN)
	target_compile_definitions(${PROJECT_NAME}
		PUBLIC
			FAE_PLATFORM_WEB
	)
elseif(WIN32)
	target_compile_definitions(${PROJECT_NAME}
		PUBLIC
			FAE_PLATFORM_WINDOWS
	)
elseif(APPLE)
	target_compile_definitions(${PROJECT_NAME}
		PUBLIC
			FAE_PLATFORM_MACOS
	)
elseif(UNIX)
	target_compile_definitions(${PROJECT_NAME}
		PUBLIC
			FAE_PLATFORM_LINUX
	)
endif()

target_compile_features(${PROJECT_NAME}
	PUBLIC
		cxx_std_23
)

if(FAE_ENABLE_WARNINGS)
	if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
		target_compile_options(${PROJECT_NAME}
			PRIVATE
				"/W4"
		)
	elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
		target_compile_options(${PROJECT_NAME}
			PRIVATE
				"-Wall"
				"-Wextra"
				"-Wpedantic"
		)
	endif()
endif()

target_include_directories(${PROJECT_NAME}
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
		$<INSTALL_INTERFACE:include>
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/src
)

file(GLOB_RECURSE FAE_SOURCES CONFIGURE_DEPENDS
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

target_sources(${PROJECT_NAME}
	PRIVATE
		${FAE_SOURCES}
)

target_link_libraries(${PROJECT_NAME}
	PUBLIC
		glm::glm
		EnTT::EnTT
		SDL3::SDL3
)
if(NOT DEFINED EMSCRIPTEN)
	target_link_libraries(${PROJECT_NAME}
		PUBLIC
			webgpu_cpp
			webgpu_dawn
	)
endif()

if(FAE_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
