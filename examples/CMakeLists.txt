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
		file(GLOB_RECURSE SUBDIRECTORY_FILES CONFIGURE_DEPENDS
			"${SUBDIRECTORY}/*.cpp"
		)
		add_executable(${SUBDIRECTORY_NAME})

		if(DEFINED EMSCRIPTEN)
			set_target_properties(${SUBDIRECTORY_NAME} PROPERTIES SUFFIX ".html")
			target_link_options(${SUBDIRECTORY_NAME}
				PRIVATE
				"-sUSE_WEBGPU=1"
				"--use-port=sdl2"
				"--use-port=sdl2_image:formats=bmp,png,xpm,jpg"
			)
	  	endif()

		target_sources(${SUBDIRECTORY_NAME}
			PRIVATE
				FILE_SET CXX_MODULES
				FILES
					${SUBDIRECTORY_FILES}
		)
		target_link_libraries(${SUBDIRECTORY_NAME} PRIVATE ${PROJECT_NAME}::${PROJECT_NAME})
	endif()
endforeach()
