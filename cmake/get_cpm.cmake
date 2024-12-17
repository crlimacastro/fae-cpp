# Downloads CPM.cmake and includes it.
# ARGS
# VERSION - Version of CPM.cmake to download (a git tag or "master" by default)
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

  # If the release is already downloaded, don't download again. Always download master as it may have changed since the last configuration
  if (NOT EXISTS ${GET_CPM_DOWNLOAD_LOCATION} OR GET_CPM_VERSION STREQUAL "master")
    file(DOWNLOAD "https://raw.githubusercontent.com/cpm-cmake/CPM.cmake/${GET_CPM_VERSION}/cmake/CPM.cmake" ${GET_CPM_DOWNLOAD_LOCATION})
  endif()

  include(${GET_CPM_DOWNLOAD_LOCATION})
endfunction()
