# returns true if the directory contains a CMakeLists.txt
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
	contains_cmakelists(${SUBDIRECTORY} SUBDIRECTORY_CONTAINS_CMAKELISTS)
	if (${SUBDIRECTORY_CONTAINS_CMAKELISTS})
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
			"${SUBDIRECTORY}/*.hpp"	
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

		target_compile_features(${SUBDIRECTORY_NAME}
			PUBLIC
				cxx_std_23
		)

		if(DEFINED EMSCRIPTEN)
			set_target_properties(${SUBDIRECTORY_NAME} PROPERTIES
				OUTPUT_NAME "index"
				SUFFIX ".html"
			)

			set_target_properties(${SUBDIRECTORY_NAME}
				PROPERTIES
					LINK_FLAGS
						"--use-preload-plugins --preload-file ${CMAKE_SOURCE_DIR}/assets@assets"
			)
	  	endif()

		target_sources(${SUBDIRECTORY_NAME}
			PRIVATE
				${SUBDIRECTORY_SOURCES}
		)

		if (NOT DEFINED EMSCRIPTEN AND ${BUILD_SHARED_LIBS})
			add_custom_target(${SUBDIRECTORY_NAME}_copy_dlls ALL
				COMMAND ${CMAKE_COMMAND} -E copy_if_different
    			$<TARGET_FILE:${PROJECT_NAME}::${PROJECT_NAME}>
				$<TARGET_FILE_DIR:${SUBDIRECTORY_NAME}>
			)
			add_dependencies(${SUBDIRECTORY_NAME} ${SUBDIRECTORY_NAME}_copy_dlls)
		endif()

		target_link_libraries(${SUBDIRECTORY_NAME} PRIVATE ${PROJECT_NAME}::${PROJECT_NAME})
	endif()
endforeach()
