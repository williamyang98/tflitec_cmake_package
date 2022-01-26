# validate the cmake version is met
if("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" LESS 2.5)
   message(FATAL_ERROR "CMake >= 2.6.0 required")
endif()

# push our cmake import file pollcy
cmake_policy(PUSH)
cmake_policy(VERSION 2.6...3.19)
set(CMAKE_IMPORT_FILE_VERSION 1)

# get our import path
get_filename_component(_IMPORT_PREFIX "${CMAKE_CURRENT_LIST_FILE}" PATH)

# create our library
add_library(tflitec SHARED IMPORTED GLOBAL)  
set_target_properties(tflitec PROPERTIES 
    INTERFACE_INCLUDE_DIRECTORIES "${_IMPORT_PREFIX}/include"
    IMPORTED_IMPLIB_RELEASE     "${_IMPORT_PREFIX}/lib/tensorflowlite_c.lib"
    IMPORTED_LOCATION_RELEASE   "${_IMPORT_PREFIX}/bin/tensorflowlite_c.dll"
    IMPORTED_IMPLIB_DEBUG       "${_IMPORT_PREFIX}/lib/tensorflowlite_c.lib"
    IMPORTED_LOCATION_DEBUG     "${_IMPORT_PREFIX}/bin/tensorflowlite_c.dll"
    )

set_property(TARGET tflitec APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE DEBUG)

# corresponding pop on policy scope
set(CMAKE_IMPORT_FILE_VERSION)
cmake_policy(POP)