cmake_minimum_required(VERSION 3.11)

project(rtl VERSION 0.3.0 LANGUAGES C CXX)

option(RTL_BUILD_EXAMPLES "Build examples" ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_LIST_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_LIST_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_CURRENT_LIST_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_CURRENT_LIST_DIR}/bin)

set(RTL_TARGET_NAME ${PROJECT_NAME})
set(RTL_TARGETS_EXPORT_NAME "${PROJECT_NAME}Targets")
set(RTL_INSTALL_CMAKE_DIR ${CMAKE_INSTALL_PREFIX}/cmake)
set(RTL_MSDOS_STUB lib/stub.bin)

add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${RTL_TARGET_NAME} ALIAS ${RTL_TARGET_NAME})

include(GNUInstallDirs)

target_compile_features(${RTL_TARGET_NAME} INTERFACE cxx_std_17)

target_compile_definitions(${PROJECT_NAME} 
    INTERFACE
        RTL_ENABLE_APP=$<BOOL:$<TARGET_PROPERTY:RTL_ENABLE_APP>>
        RTL_ENABLE_APP_AUDIO_OUTPUT=$<BOOL:$<TARGET_PROPERTY:RTL_ENABLE_APP_AUDIO_OUTPUT>>
        RTL_ENABLE_APP_CLOCK=$<BOOL:$<TARGET_PROPERTY:RTL_ENABLE_APP_CLOCK>>
        RTL_ENABLE_APP_CURSOR_HIDDEN=$<BOOL:$<TARGET_PROPERTY:RTL_ENABLE_APP_CURSOR_HIDDEN>>
        RTL_ENABLE_APP_FULLSCREEN=$<BOOL:$<TARGET_PROPERTY:RTL_ENABLE_APP_FULLSCREEN>>
        RTL_ENABLE_APP_KEYS=$<BOOL:$<TARGET_PROPERTY:RTL_ENABLE_APP_KEYS>>
        RTL_ENABLE_APP_OPENGL=$<BOOL:$<TARGET_PROPERTY:RTL_ENABLE_APP_OPENGL>>
        RTL_ENABLE_APP_OPENGL_VSYNC=$<BOOL:$<TARGET_PROPERTY:RTL_ENABLE_APP_OPENGL_VSYNC>>
        RTL_ENABLE_APP_OSD=$<BOOL:$<TARGET_PROPERTY:RTL_ENABLE_APP_OSD>>
        RTL_ENABLE_APP_RESET=$<BOOL:$<TARGET_PROPERTY:RTL_ENABLE_APP_RESET>>
        RTL_ENABLE_APP_RESIZE=$<BOOL:$<TARGET_PROPERTY:RTL_ENABLE_APP_RESIZE>>
        RTL_ENABLE_APP_RESOURCES=$<BOOL:$<TARGET_PROPERTY:RTL_ENABLE_APP_RESOURCES>>
        RTL_ENABLE_APP_SCREEN_BUFFER=$<BOOL:$<TARGET_PROPERTY:RTL_ENABLE_APP_SCREEN_BUFFER>>
        RTL_ENABLE_APP_SINGLETON=$<BOOL:$<TARGET_PROPERTY:RTL_ENABLE_APP_SINGLETON>>
        RTL_ENABLE_ASSERT=$<BOOL:$<TARGET_PROPERTY:RTL_ENABLE_ASSERT>>
        RTL_ENABLE_CHRONO_CLOCK=$<BOOL:$<TARGET_PROPERTY:RTL_ENABLE_CHRONO_CLOCK>>
        RTL_ENABLE_HEAP=$<BOOL:$<TARGET_PROPERTY:RTL_ENABLE_HEAP>>
        RTL_ENABLE_LOG=$<BOOL:$<TARGET_PROPERTY:RTL_ENABLE_LOG>>
        RTL_ENABLE_MEMSET=$<BOOL:$<TARGET_PROPERTY:RTL_ENABLE_MEMSET>>
        RTL_ENABLE_OPENCL=$<BOOL:$<TARGET_PROPERTY:RTL_ENABLE_OPENCL>>
        RTL_ENABLE_RUNTIME_CHECKS=$<BOOL:$<TARGET_PROPERTY:RTL_ENABLE_RUNTIME_CHECKS>>
        RTL_ENABLE_RUNTIME_TESTS=$<BOOL:$<TARGET_PROPERTY:RTL_ENABLE_RUNTIME_TESTS>>

        UNICODE
)

target_include_directories(${PROJECT_NAME}
    INTERFACE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
)

if(MSVC)
    target_compile_options(${RTL_TARGET_NAME}
        INTERFACE
            /EHa-       # Disable standard C++ stack unwinding. 
            /EHs-       # Disable C++ exception handling (no SEH exceptions).
            /FA         # Creates a listing file containing assembler code.
            /GF         # Enables string pooling
            /GL         # Enables whole program optimization.
            /GR-        # Disables run-time type information (RTTI).
            /Gs32768    # The number of bytes that local variables can occupy before a stack probe is initiated
            /GS-        # Disables checks buffer security.
            /Gw         # Enables whole-program global data optimization.
            /O1         # Creates small code
            /Oi-        # Generates intrinsic functions
            /Os         # Favors small code.
            /Oy         # Omits frame pointer. (x86 only)

            /WX         # Treat warnings as errors.
            /Wall       # Enable all warnings, including warnings that are disabled by default.

            # Disable specific warnings:
            /wd4505     # 'function' : unreferenced local function has been removed
            /wd4514     # 'function' : unreferenced inline function has been removed
            /wd4577     # 'noexcept' used with no exception handling mode specified; termination on exception is not guaranteed. Specify /EHsc
            /wd4625     # 'derived class' : copy constructor was implicitly defined as deleted because a base class copy constructor is inaccessible or deleted
            /wd4626     # 'derived class' : assignment operator was implicitly defined as deleted because a base class assignment operator is inaccessible or deleted
            /wd4710     # 'function' : function not inlined
            /wd4711     # function 'function' selected for inline expansion
            /wd5039     # pointer or reference to potentially throwing function passed to extern C function under -EHc. Undefined behavior may occur if this function throws an exception.
            /wd5045     # Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified
    )

    # MSVC compiler inserts call to memset when generates code.
    # Can't define your own version of the memset without dislabling whole program optimization.
    # So we link our program with VC runtime (statically) and linker will see memset implementation.
    # set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    target_link_libraries(${RTL_TARGET_NAME}
        INTERFACE
            libvcruntime
            libcmt
            $<$<BOOL:$<TARGET_PROPERTY:RTL_ENABLE_APP_AUDIO_OUTPUT>>:winmm>
            $<$<BOOL:$<TARGET_PROPERTY:RTL_ENABLE_APP_OPENGL>>:opengl32>
    )

    ################################################################################################
    # Typical segment names
    ################################################################################################
    #
    # .text: Code
    # .data: Initialized data
    # .bss: Uninitialized data
    # .rdata: Const/read-only (and initialized) data
    # .edata: Export descriptors
    # .idata: Import descriptors
    # .reloc: Relocation table (for code instructions with absolute addressing when
    #           the module could not be loaded at its preferred base address)
    # .rsrc: Resources (icon, bitmap, dialog, ...)
    # .tls: __declspec(thread) data (Fails with dynamically loaded DLLs -> hard to find bugs)
    target_link_options(${PROJECT_NAME} 
        INTERFACE
            $<$<CONFIG:RELWITHDEBINFO>:/DEBUG>          # Generate debug info (for RelWithDebInfo)
            $<$<CONFIG:DEBUG>:/DEBUG>                   # Generate debug info (for Debug)
            /DYNAMICBASE:NO                             # Specifies whether to generate an executable image that's rebased at load time by using the address space layout randomization (ASLR) feature.
            /FIXED                                      # Creates a program that can be loaded only at its preferred base address.
            /INCREMENTAL:NO                             # Controls incremental linking.
            /LTCG                                       # Specifies link-time code generation.
          # /MERGE:.data=.text                          # Combines sections.
          # /MERGE:.rdata=.text                         # Combines sections.
          # /MERGE:.edata=.text                         # Combines sections.
            /NOCOFFGRPINFO                              # Undocumented linker flag.
            /NODEFAULTLIB                               # Ignores all (or the specified) default libraries when external references are resolved.
            /MANIFEST:NO                                # Creates a side-by-side manifest file and optionally embeds it in the binary.
            /MAP                                        # Creates a mapfile.
            /SAFESEH:NO                                 # Specifies that the image will contain a table of safe exception handlers
          # /SECTION:.text,ER                           # Overrides the attributes of a section.
            /SUBSYSTEM:WINDOWS                          # Specify the Windows subsystem targeted by the executable.

            # Specify tiny MS-DOS Stub
            $<INSTALL_INTERFACE:/STUB:$<INSTALL_PREFIX>/${RTL_MSDOS_STUB}>
            $<BUILD_INTERFACE:/STUB:${PROJECT_SOURCE_DIR}/${RTL_MSDOS_STUB}>
    )
endif()

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
        ${RTL_INSTALL_CMAKE_DIR}
)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/rtl TYPE INCLUDE)
install(FILES ${PROJECT_SOURCE_DIR}/${RTL_MSDOS_STUB} TYPE LIB)

install(
    DIRECTORY 
        ${CMAKE_CURRENT_LIST_DIR}/bin
    DESTINATION 
        ${CMAKE_INSTALL_PREFIX}
    FILES_MATCHING 
        PATTERN "*.exe"
)

install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    DESTINATION
        ${RTL_INSTALL_CMAKE_DIR}
)

export(
    TARGETS
        ${RTL_TARGET_NAME}
    NAMESPACE
        ${PROJECT_NAME}::
    FILE
        ${CMAKE_CURRENT_BINARY_DIR}/${RTL_TARGETS_EXPORT_NAME}.cmake
)

install(
    TARGETS
        ${RTL_TARGET_NAME}
    EXPORT
        ${RTL_TARGETS_EXPORT_NAME}
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
        ${RTL_INSTALL_CMAKE_DIR}
)

if(RTL_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
