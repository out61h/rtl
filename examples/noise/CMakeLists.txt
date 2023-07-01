project(noise LANGUAGES C CXX)

add_executable(${PROJECT_NAME} WIN32 noise.cpp)

set_target_properties(${PROJECT_NAME}
    PROPERTIES
        RTL_ENABLE_APP ON
        RTL_ENABLE_APP_AUDIO_OUTPUT ON
        RTL_ENABLE_APP_KEYS ON
        RTL_ENABLE_APP_SCREEN_BUFFER ON
        RTL_ENABLE_HEAP ON
)

add_dependencies(${PROJECT_NAME} ${RTL_TARGET_NAME})
target_link_libraries(${PROJECT_NAME} ${RTL_TARGET_NAME})

if(MSVC)
    string(REPLACE "/RTC1" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
    string(REPLACE "/EHsc" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
endif()
