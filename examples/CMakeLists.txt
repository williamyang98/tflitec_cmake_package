cmake_minimum_required(VERSION 3.0.0)
project(tflitec)

set(tflitec_DIR ${CMAKE_SOURCE_DIR}/../)
find_package(tflitec CONFIG REQUIRED)

add_executable(main main.c)
target_link_libraries(main PRIVATE tflitec) 

# vcpkg.cmake has internal stuff that autogenerates this
# we have to do this manually
add_custom_command(
    TARGET main 
    POST_BUILD 
    COMMAND ${CMAKE_COMMAND} -E copy 
    $<TARGET_RUNTIME_DLLS:main> 
    $<TARGET_FILE_DIR:main>
    COMMAND_EXPAND_LISTS
)
