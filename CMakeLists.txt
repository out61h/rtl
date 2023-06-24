cmake_minimum_required(VERSION 3.11)

project(rtl VERSION 0.3.0 LANGUAGES C CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_LIST_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_LIST_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_CURRENT_LIST_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_CURRENT_LIST_DIR}/bin)

set(RTL_TARGET_NAME ${PROJECT_NAME})
set(RTL_TARGETS_EXPORT_NAME "${PROJECT_NAME}Targets")

option(RTL_BUILD_EXAMPLES "Build examples" ON)

if(RTL_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${RTL_TARGET_NAME} ALIAS ${RTL_TARGET_NAME})

include(GNUInstallDirs)

target_include_directories(${RTL_TARGET_NAME}
    INTERFACE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_compile_features(${RTL_TARGET_NAME} 
    INTERFACE 
        cxx_std_17
)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    VERSION
        ${PROJECT_VERSION}
    COMPATIBILITY
        AnyNewerVersion
)

configure_package_config_file(
        ${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    INSTALL_DESTINATION
        ${CMAKE_INSTALL_PREFIX}/cmake
)

install(
    DIRECTORY
        ${PROJECT_SOURCE_DIR}/include/rtl
    DESTINATION
        ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    DESTINATION
        ${CMAKE_INSTALL_PREFIX}/cmake
)

export(
    TARGETS
        ${RTL_TARGET_NAME}
    NAMESPACE
        ${PROJECT_NAME}::
    FILE
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake
)

install(
    TARGETS
        ${RTL_TARGET_NAME}
    EXPORT
        ${PROJECT_NAME}Targets
    INCLUDES DESTINATION
        ${CMAKE_INSTALL_INCLUDEDIR}
    ARCHIVE DESTINATION
        ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION
        ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION
        ${CMAKE_INSTALL_BINDIR}
)

install(
    EXPORT
        ${RTL_TARGETS_EXPORT_NAME}
    NAMESPACE
        ${PROJECT_NAME}::
    DESTINATION
        ${CMAKE_INSTALL_PREFIX}/cmake
)
