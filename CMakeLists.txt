##################################################    Project     ##################################################
cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(rtsm VERSION 0.0.1 LANGUAGES C CXX)
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_CXX_STANDARD 11)

include(set_max_warning_level)
set_max_warning_level()

##################################################    Options     ##################################################
option(BUILD_TESTS "Build tests." OFF)

##################################################    Sources     ##################################################
file(GLOB_RECURSE PROJECT_HEADERS include/*.h include/*.hpp)
file(GLOB_RECURSE PROJECT_CMAKE_UTILS cmake/*.cmake)
file(GLOB_RECURSE PROJECT_MISC *.md *.txt)
set(PROJECT_FILES
        ${PROJECT_HEADERS}
        ${PROJECT_CMAKE_UTILS}
        ${PROJECT_MISC})

include(assign_source_group)
assign_source_group(${PROJECT_FILES})

##################################################  Dependencies  ##################################################
include(import_library)
find_package(Threads REQUIRED)
set(PROJECT_LIBRARIES ${CMAKE_THREAD_LIBS_INIT})
# ADD LIBRARIES HERE.
# Examples:
# - Header Only:
#   find_package  (GLM REQUIRED)
#   import_library(GLM_INCLUDE_DIRS)
# - Identical Debug and Release:
#   find_package  (OpenGL REQUIRED)
#   import_library(OPENGL_INCLUDE_DIR OPENGL_LIBRARIES)
# - Separate Debug and Release:
#   find_package  (Boost REQUIRED iostreams)
#   import_library(Boost_INCLUDE_DIRS Boost_IOSTREAMS_LIBRARY_DEBUG Boost_IOSTREAMS_LIBRARY_RELEASE)
# - You may also set the PROJECT_INCLUDE_DIRS and PROJECT_LIBRARIES instead of using import_library.

##################################################    Targets     ##################################################
add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        $<INSTALL_INTERFACE:include>)
target_include_directories(${PROJECT_NAME} INTERFACE ${PROJECT_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} INTERFACE ${PROJECT_LIBRARIES})
target_compile_definitions(${PROJECT_NAME} INTERFACE ${PROJECT_COMPILE_DEFINITIONS})

# Hack for header-only project to appear in the IDEs.
add_library(${PROJECT_NAME}_ STATIC ${PROJECT_FILES})
target_include_directories(${PROJECT_NAME}_ PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(${PROJECT_NAME}_ PUBLIC ${PROJECT_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME}_ PUBLIC ${PROJECT_LIBRARIES})
target_compile_definitions(${PROJECT_NAME}_ PUBLIC ${PROJECT_COMPILE_DEFINITIONS})
set_target_properties(${PROJECT_NAME}_ PROPERTIES LINKER_LANGUAGE CXX)

##################################################    Testing     ##################################################
#if(BUILD_TESTS)
enable_testing()
set(TEST_MAIN_NAME doctest_main)
set(TEST_MAIN_SOURCES tests/doctest/doctest.h tests/doctest/main.cpp)
add_library(${TEST_MAIN_NAME} OBJECT ${TEST_MAIN_SOURCES})
set_property(TARGET ${TEST_MAIN_NAME} PROPERTY FOLDER tests/doctest)
assign_source_group(${TEST_MAIN_SOURCES})

file(GLOB PROJECT_TEST_CPPS tests/*.cpp)
foreach (_SOURCE ${PROJECT_TEST_CPPS})
    get_filename_component(_NAME ${_SOURCE} NAME_WE)
    add_executable(${_NAME} ${_SOURCE} $<TARGET_OBJECTS:${TEST_MAIN_NAME}>)
    target_link_libraries(${_NAME} ${PROJECT_NAME})
    if (${CMAKE_BUILD_TYPE} STREQUAL Debug)
        target_compile_options(${_NAME} PRIVATE -g3 -O0)
    endif ()
    #    if(MSVC)
    #        # Trust me, Microsoft docs suck.
    #        target_compile_options(${_NAME} "/Fa${OUTPUT_PATH}/${ASSEMBLY_LISTING_FILE}" /FA)
    #    else()
    #        # The trick is that makefiles generator defines a [sourcefile].s target for each sourcefile of a target to generate the listing
    #        # of that file. We hook a command after build to invoke that target and copy the result file to our ourput directory:
    #        add_custom_command(TARGET ${_NAME}
    #                POST_BUILD
    #                COMMAND make ARGS ${_SOURCE}.s
    #                COMMAND ${CMAKE_COMMAND} -E copy
    #                "${CMAKE_BINARY_DIR}/CMakeFiles/${_NAME}.dir/${_SOURCE}.cpp.s"
    #                "${CMAKE_BINARY_DIR}/${_NAME}"
    #                WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
    #    endif()
    add_test(${_NAME} ${_NAME})
    set_property(TARGET ${_NAME} PROPERTY FOLDER tests)
    assign_source_group(${_SOURCE})
endforeach ()
#endif()

##################################################  Examples  ##################################################
file(GLOB PROJECT_EXAMPLES_CPPS examples/*.cpp)
foreach (_SOURCE ${PROJECT_EXAMPLES_CPPS})
    get_filename_component(_NAME ${_SOURCE} NAME_WE)
    add_executable(${_NAME} ${_SOURCE})
    target_link_libraries(${_NAME} ${PROJECT_NAME})
    if (${CMAKE_BUILD_TYPE} STREQUAL Debug)
        target_compile_options(${_NAME} PRIVATE -g3 -O0)
    endif ()
    target_compile_options(${_NAME} PRIVATE -save-temps=obj -fno-rtti -fno-exceptions -fverbose-asm)
    #    if(MSVC)
    #        # Trust me, Microsoft docs suck.
    #        target_compile_options(${_NAME} "/Fa${OUTPUT_PATH}/${ASSEMBLY_LISTING_FILE}" /FA)
    #    else()
    #        # The trick is that makefiles generator defines a [sourcefile].s target for each sourcefile of a target to generate the listing
    #        # of that file. We hook a command after build to invoke that target and copy the result file to our ourput directory:
    #        add_custom_command(TARGET ${_NAME}
    #                POST_BUILD
    #                COMMAND make ARGS ${_SOURCE}.s
    #                COMMAND ${CMAKE_COMMAND} -E copy
    #                "${CMAKE_BINARY_DIR}/CMakeFiles/${_NAME}.dir/${_SOURCE}.cpp.s"
    #                "${CMAKE_BINARY_DIR}/${_NAME}"
    #                WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
    #    endif()
#    set_property(TARGET ${_NAME} PROPERTY FOLDER tests)
    #    assign_source_group(${_SOURCE})
endforeach ()

add_subdirectory(doc)
##################################################  Installation  ##################################################
install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}-config)
install(DIRECTORY include/ DESTINATION include)
install(EXPORT ${PROJECT_NAME}-config DESTINATION cmake)
export(TARGETS ${PROJECT_NAME} FILE ${PROJECT_NAME}-config.cmake)