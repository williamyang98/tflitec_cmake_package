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

# detect if x86 or x64
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set_target_properties(tflitec PROPERTIES 
        INTERFACE_INCLUDE_DIRECTORIES "${_IMPORT_PREFIX}/include"
        IMPORTED_IMPLIB             "${_IMPORT_PREFIX}/lib/x64/Release/tensorflowlite_c.lib"
        IMPORTED_LOCATION           "${_IMPORT_PREFIX}/bin/x64/Release/tensorflowlite_c.dll")
    message(STATUS "Using x64 build of tflitec")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    set_target_properties(tflitec PROPERTIES 
        INTERFACE_INCLUDE_DIRECTORIES "${_IMPORT_PREFIX}/include"
        IMPORTED_IMPLIB             "${_IMPORT_PREFIX}/lib/x86/Release/tensorflowlite_c.lib"
        IMPORTED_LOCATION           "${_IMPORT_PREFIX}/bin/x86/Release/tensorflowlite_c.dll")
    message(STATUS "Using x86 build of tflitec")
endif()

set_property(TARGET tflitec APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)

# corresponding pop on policy scope
set(CMAKE_IMPORT_FILE_VERSION)
cmake_policy(POP)