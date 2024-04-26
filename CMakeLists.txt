cmake_minimum_required(VERSION 3.29.2)

project(
	fae
	VERSION 0.0.2
	LANGUAGES CXX
)

# includes
include(cmake/get_cpm.cmake)

# options
set(FAE_CPM_VERSION "v0.39.0" CACHE STRING "Which version of CPM to use (tag or \"master\")")
option(FAE_ENABLE_WARNINGS "Enable compilation warnings" OFF)
option(FAE_ENABLE_WARNINGS_AS_ERRORS "Treat warnings as errors" OFF)
option(FAE_BUILD_EXAMPLES "Build examples" OFF)
# # TODO option(FAE_BUILD_TESTS "Build tests" OFF)
# # TODO option(FAE_BUILD_BENCHMARKS "Build benchmarks" OFF))
# # TODO option(FAE_BUILD_DOCS "Build documentation" OFF)
# # TODO option(FAE_ENABLE_INSTALL "Enable to install." OFF)

# dependencies
get_cpm(VERSION ${FAE_CPM_VERSION})
CPMUsePackageLock(package-lock.cmake)
CPMAddPackage("gh:g-truc/glm#1.0.0")
CPMAddPackage("gh:skypjack/entt#v3.13.1")
CPMAddPackage(
	NAME SDL
	GITHUB_REPOSITORY libsdl-org/SDL
	GIT_TAG prerelease-3.1.1
	OPTIONS
		"SDL_DISABLE_INSTALL OFF"
		if(${BUILD_SHARED_LIBS})
			"SDL_SHARED ON"
			"SDL_STATIC OFF"
		else()
			"SDL_SHARED OFF"
			"SDL_STATIC ON"
		endif()
)
if(NOT EMSCRIPTEN)
	CPMAddPackage(
		NAME dawn
  		GIT_REPOSITORY https://dawn.googlesource.com/dawn
  		GIT_TAG chromium/6433
  		OPTIONS
			"DAWN_FETCH_DEPENDENCIES ON"
			"EXCLUDE_FROM_ALL ON"
)
endif()

# configure
configure_file(config.cpp.in src/config.cpp)

# library
add_library(${PROJECT_NAME})
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

set_target_properties(${PROJECT_NAME} PROPERTIES
	CXX_STANDARD 23
	CXX_STANDARD_REQUIRED ON
	COMPILE_WARNING_AS_ERROR ${FAE_ENABLE_WARNINGS_AS_ERRORS}
	LINKER_LANGUAGE CXX
)
target_compile_features(${PROJECT_NAME}
	PUBLIC
		cxx_std_23
)

# warnings
if(FAE_ENABLE_WARNINGS)
	if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
		set(FAE_WARNINGS "/W4")
	elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
		set(FAE_WARNINGS "-Wall -Wextra -Wpedantic")
	endif()
endif()

target_compile_options(${PROJECT_NAME}
	PRIVATE
		${FAE_WARNINGS}
)

#includes
target_include_directories(${PROJECT_NAME}
	PUBLIC
		${glm_SOURCE_DIR}
)

# files
file(GLOB_RECURSE FAE_MODULES CONFIGURE_DEPENDS
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

target_sources(${PROJECT_NAME}
	PUBLIC
		FILE_SET CXX_MODULES
		BASE_DIRS
			${CMAKE_CURRENT_SOURCE_DIR}/src
			${CMAKE_CURRENT_BINARY_DIR}/src
		FILES
			${FAE_MODULES}
			${CMAKE_CURRENT_BINARY_DIR}/src/config.cpp
	PRIVATE
)	

set(FAE_PUBLIC_LIBRARIES
	EnTT::EnTT
	SDL3::SDL3
)
if (NOT EMSCRIPTEN)
	list(APPEND FAE_PUBLIC_LIBRARIES
		webgpu_cpp
		webgpu_dawn
	)
endif()
target_link_libraries(${PROJECT_NAME}
	PUBLIC
		${FAE_PUBLIC_LIBRARIES}
	PRIVATE
)

if(FAE_BUILD_EXAMPLES)
	add_subdirectory(examples)
endif()
