project( noise LANGUAGES C CXX )

add_executable( ${PROJECT_NAME} WIN32 noise.cpp)

add_dependencies(${PROJECT_NAME} rtl)

target_compile_features(${PROJECT_NAME} 
    PRIVATE 
        cxx_std_17
)

macro(RemoveDebugCXXFlag flag)
    string(REPLACE "${flag}" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
endmacro()

macro(RemoveCXXFlag flag)
    string(REPLACE "${flag}" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
endmacro()

target_compile_definitions(${PROJECT_NAME} 
    PRIVATE      
        RTL_ENABLE_ASSERT=0
        RTL_ENABLE_APP=1
        RTL_ENABLE_APP_AUDIO=0
        RTL_ENABLE_APP_CURSOR=1
        RTL_ENABLE_APP_CLOCK=0
        RTL_ENABLE_APP_FULLSCREEN=0
        RTL_ENABLE_APP_KEYS=1
        RTL_ENABLE_APP_OSD=0
        RTL_ENABLE_APP_OPENGL=0
        RTL_ENABLE_APP_OPENGL_VSYNC=0
        RTL_ENABLE_APP_RESET=0
        RTL_ENABLE_APP_RESIZE=0
        RTL_ENABLE_APP_RESOURCES=0
        RTL_ENABLE_APP_SCREEN_BUFFER=1
        RTL_ENABLE_APP_SINGLETON=1
        RTL_ENABLE_CHRONO_CLOCK=0
        RTL_ENABLE_HEAP=1
        RTL_ENABLE_LOG=0
        RTL_ENABLE_MEMSET=0
        RTL_ENABLE_OPENCL=0
        RTL_ENABLE_RUNTIME_CHECKS=0
        RTL_ENABLE_RUNTIME_TESTS=0

        UNICODE
)

target_include_directories(${PROJECT_NAME} 
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
)

if(MSVC)
    # MSVC compiler inserts call to memset when generates code.
    # Can't define your own version of the memset without dislabling whole program optimization.
    # So we link our program with VC runtime (statically) and linker will see memset implementation.
    target_link_libraries(${PROJECT_NAME} 
        libvcruntime
        libcmt
    )

    target_compile_options(${PROJECT_NAME} 
        PRIVATE
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
            /Wall       # Enable all warnings, including warnings that are disabled by default.
            /wd4505     # 'function' : unreferenced local function has been removed
            /wd4514     # 'function' : unreferenced inline function has been removed

            # only noise
            /wd4625     # 'derived class' : copy constructor was implicitly defined as deleted because a base class copy constructor is inaccessible or deleted
            /wd4626     # 'derived class' : assignment operator was implicitly defined as deleted because a base class assignment operator is inaccessible or deleted
            /wd4577     # 'noexcept' used with no exception handling mode specified; termination on exception is not guaranteed. Specify /EHsc
            /wd4710     # 'function' : function not inlined
            /wd4711     # function 'function' selected for inline expansion
            /wd5039     # pointer or reference to potentially throwing function passed to extern C function under -EHc. Undefined behavior may occur if this function throws an exception.
            /wd5045     # Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified
            /WX         # Treat warnings as errors.
    )

    RemoveDebugCXXFlag("/RTC1")
    RemoveCXXFlag("/EHsc")

    # TODO: Add for debug config /DEBUG # Creates debugging information.
    # TODO: Move stub.bin to RTL (and import linker command from)
    # TODO: Import some flags from RTL library
    target_link_options(${PROJECT_NAME} 
        PRIVATE
            /DYNAMICBASE:NO                         # Specifies whether to generate an executable image that's rebased at load time by using the address space layout randomization (ASLR) feature.
            /FIXED                                  # Creates a program that can be loaded only at its preferred base address.
            /INCREMENTAL:NO                         # Controls incremental linking.
            /LTCG                                   # Specifies link-time code generation.
            /MANIFEST:NO                            # Creates a side-by-side manifest file and optionally embeds it in the binary.
            /MAP                                    # Creates a mapfile.
            /NOCOFFGRPINFO
            /NODEFAULTLIB                           # Ignores all (or the specified) default libraries when external references are resolved.
            /SAFESEH:NO                             # Specifies that the image will contain a table of safe exception handlers
            /STUB:${CMAKE_SOURCE_DIR}/lib/stub.bin  # Attaches an MS-DOS stub program to a Win32 program.
    )
endif()
