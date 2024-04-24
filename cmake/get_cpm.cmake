function (get_cpm)
    set(oneValueArgs
        VERSION
    )
    cmake_parse_arguments(
        GET_CPM
        "${options}"
        "${oneValueArgs}"
        "${multiValueArgs}"
        ${ARGN})

  if(NOT GET_CPM_VERSION)
    set(GET_CPM_VERSION "master")
  endif()

  if(CPM_SOURCE_CACHE)
    set(GET_CPM_DOWNLOAD_LOCATION "${CPM_SOURCE_CACHE}/cpm/CPM_${GET_CPM_VERSION}.cmake")
  elseif(DEFINED ENV{CPM_SOURCE_CACHE})
    set(GET_CPM_DOWNLOAD_LOCATION "$ENV{CPM_SOURCE_CACHE}/cpm/CPM_${GET_CPM_VERSION}.cmake")
  else()
    set(GET_CPM_DOWNLOAD_LOCATION "${CMAKE_BINARY_DIR}/cmake/CPM_${GET_CPM_VERSION}.cmake")
  endif()

  # Expand relative path. This is important if the provided path contains a tilde (~)
  get_filename_component(GET_CPM_DOWNLOAD_LOCATION ${GET_CPM_DOWNLOAD_LOCATION} ABSOLUTE)

  # If release already downloaded, don't download again. Always download master as it may have changed since last download
  if (NOT EXISTS ${GET_CPM_DOWNLOAD_LOCATION} OR GET_CPM_VERSION STREQUAL "master")
    if (GET_CPM_VERSION STREQUAL "master")
        set(GET_CPM_URL "https://raw.githubusercontent.com/cpm-cmake/CPM.cmake/master/cmake/CPM.cmake")
    else()
        set(GET_CPM_URL "https://github.com/cpm-cmake/CPM.cmake/releases/download/${GET_CPM_VERSION}/CPM.cmake")
    endif()
    file(DOWNLOAD ${GET_CPM_URL} ${GET_CPM_DOWNLOAD_LOCATION})
  endif()

  include(${GET_CPM_DOWNLOAD_LOCATION})
  if (GET_CPM_VERSION STREQUAL "master")
    message(WARNING "To use a specific version of CPM, set the VERSION parameter of this 'get_cpm' function.")
  endif()
endfunction()
