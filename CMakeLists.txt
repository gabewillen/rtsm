cmake_minimum_required(VERSION 3.5)
project(rtsm VERSION 0.0.1 LANGUAGES CXX)


#enable_testing()
add_subdirectory(tests)
add_library(rtsm INTERFACE)

target_include_directories(rtsm INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        )

include(GNUInstallDirs)
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/rtsm)
install(TARGETS rtsm EXPORT rtsm-targets LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
set_target_properties(rtsm PROPERTIES EXPORT_NAME rtsm)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(EXPORT rtsm-targets
        FILE
        rtsmTargets.cmake
        NAMESPACE
        rtsm::
        DESTINATION
        ${INSTALL_CONFIGDIR}
        )
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/rtsmConfigVersion.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion
)
configure_package_config_file(${CMAKE_CURRENT_LIST_DIR}/cmake/rtsmConfig.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/rtsmConfig.cmake
        INSTALL_DESTINATION ${INSTALL_CONFIGDIR}
        )
install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/rtsmConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/rtsmConfigVersion.cmake
        DESTINATION ${INSTALL_CONFIGDIR}
        )

## Exporting from the build tree

export(EXPORT rtsm-targets FILE ${CMAKE_CURRENT_BINARY_DIR}/rtsmTargets.cmake NAMESPACE rtsm::)

#Register package in user's package registry
export(PACKAGE rtsm)
