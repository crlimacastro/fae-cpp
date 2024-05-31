function(contains_cmakelists SUBDIRECTORY RETURN)
	file(GLOB CMAKELISTS_FILE "${SUBDIRECTORY}/CMakeLists.txt")
	if(CMAKELISTS_FILE)
		set(${RETURN} TRUE PARENT_SCOPE)
	else()
		set(${RETURN} FALSE PARENT_SCOPE)
	endif()
endfunction()

# calls add_subdirectory only if the directory contains a CMakeLists.txt file
function(maybe_add_subdirectory SUBDIRECTORY)
	contains_cmakelists(${SUBDIRECTORY} DOES_SUBDIRECTORY_CONTAIN_CMAKELISTS)
	if (${DOES_SUBDIRECTORY_CONTAIN_CMAKELISTS})
		add_subdirectory(${SUBDIRECTORY})
	endif()
endfunction()

# iterate over subdirectories in the current folder
file(GLOB FAE_EXAMPLE_SUBDIRS LIST_DIRECTORIES true *)
foreach(SUBDIRECTORY ${FAE_EXAMPLE_SUBDIRS})
	if(IS_DIRECTORY ${SUBDIRECTORY})
		maybe_add_subdirectory(${SUBDIRECTORY})
		get_filename_component(SUBDIRECTORY_NAME ${SUBDIRECTORY} NAME)
		file(GLOB_RECURSE SUBDIRECTORY_SOURCES CONFIGURE_DEPENDS
			"${SUBDIRECTORY}/*.cpp"
		)
		add_executable(${SUBDIRECTORY_NAME})

		set_target_properties(${SUBDIRECTORY_NAME}
    		PROPERTIES
        		CXX_STANDARD 23
        		CXX_STANDARD_REQUIRED ON
				CXX_EXTENSIONS OFF
        		COMPILE_WARNING_AS_ERROR ${FAE_ENABLE_WARNINGS_AS_ERRORS}
        		LINKER_LANGUAGE CXX
		)

		target_compile_features(${PROJECT_NAME}
			PUBLIC
				cxx_std_23
		)

		if(DEFINED EMSCRIPTEN)
			set_target_properties(${SUBDIRECTORY_NAME} PROPERTIES SUFFIX ".html")
			target_link_options(${SUBDIRECTORY_NAME}
				PRIVATE
				"-sUSE_WEBGPU=1"
				"--use-port=sdl2"
				"--use-port=sdl2_image:formats=bmp,png,xpm,jpg"
				"--use-port=sdl2_ttf"
				"--use-port=sdl2_mixer"
				"--use-port=sdl2_net"
				"--use-port=bullet"
				"-sALLOW_MEMORY_GROWTH=1"
				"-sMAXIMUM_MEMORY=1gb"
				"-sASYNCIFY=1"
			)
	  	endif()

		target_sources(${SUBDIRECTORY_NAME}
			PRIVATE
				${SUBDIRECTORY_SOURCES}
		)

		if (NOT DEFINED EMSCRIPTEN)
			if(${BUILD_SHARED_LIBS})
				add_custom_command(
					TARGET ${PROJECT_NAME} POST_BUILD
					COMMAND ${CMAKE_COMMAND} -E copy
						$<TARGET_FILE:SDL3::SDL3-shared>
						$<TARGET_FILE_DIR:${SUBDIRECTORY_NAME}>
				)
			endif()
		endif()

		target_link_libraries(${SUBDIRECTORY_NAME} PRIVATE ${PROJECT_NAME}::${PROJECT_NAME})
	endif()
endforeach()
