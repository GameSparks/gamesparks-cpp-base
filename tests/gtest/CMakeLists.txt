file(GLOB_RECURSE GTEST_SRCS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc")

message(${GTEST_SRCS})

add_library (gtest ${GTEST_SRCS})

target_include_directories (gtest PRIVATE .)
target_include_directories (gtest PUBLIC ./include)

if (APPLE)
    if(IOS)
        set_xcode_property (gtest IPHONEOS_DEPLOYMENT_TARGET "7.0")
    endif()
endif()
