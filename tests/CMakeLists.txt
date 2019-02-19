cmake_minimum_required(VERSION 2.8.12)
project(GameSparksTests)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})
include(Coverage)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

if(APPLE)
    # ugly workaround for Version 7.0 (with compatibility for later xcode versions)
    if(EXISTS "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk")
        set(CMAKE_OSX_SYSROOT "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk")
    elseif(EXISTS "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.11.sdk")
        set(CMAKE_OSX_SYSROOT "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.11.sdk")
    elseif(EXISTS "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.12.sdk")
        set(CMAKE_OSX_SYSROOT "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.12.sdk")
    elseif(EXISTS "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.13.sdk")
        set(CMAKE_OSX_SYSROOT "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.13.sdk")
    endif()
endif()

add_subdirectory (gtest ${CMAKE_BINARY_DIR}/bin/gtest)
add_subdirectory (../base/build_scripts/ ${CMAKE_BINARY_DIR}/bin/gs)

file(
    GLOB_RECURSE
    TEST_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/base/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/rt/*.cpp"
)

get_directory_property(hasParent PARENT_DIRECTORY)
if(hasParent)
    set(TEST_SOURCES ${TEST_SOURCES} PARENT_SCOPE)
endif()

add_executable(
    ${PROJECT_NAME}
    ${TARGET_EXTRA_ARG}
    ${TEST_SOURCES}
    "${CMAKE_CURRENT_SOURCE_DIR}/../base/samples/extra/usleep.cpp"
)


if (UNIX AND NOT APPLE)
  find_package(Threads REQUIRED)
  if(THREADS_HAVE_PTHREAD_ARG)
    target_compile_options(${PROJECT_NAME} PUBLIC "-pthread")
  endif()
  if(CMAKE_THREAD_LIBS_INIT)
    target_link_libraries(${PROJECT_NAME} "${CMAKE_THREAD_LIBS_INIT}")
  endif()
endif (UNIX AND NOT APPLE)

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../base/samples/extra")
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../base/include")
target_link_libraries(${PROJECT_NAME} gtest GameSparks ${WS_LIBS})

# mkdir build
# cd build
# cmake -DCMAKE_BUILD_TYPE=Coverage ..
# RT_SESSION_0="..." RT_SESSION_1="..." GAMESPARKS_API_KEY=... GAMESPARKS_API_SECRET=... make GameSparksTestsCoverage

target_setup_coverage(${PROJECT_NAME})# --gtest_filter=Bugs.ParseNumber)

if (APPLE)
    if(IOS)
        set_target_properties(${PROJECT_NAME} PROPERTIES XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer")

        if(DEFINED ENV{DEVELOPMENT_TEAM})
            set_target_properties(${PROJECT_NAME} PROPERTIES XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "$ENV{DEVELOPMENT_TEAM}")
        else()
            set_target_properties(${PROJECT_NAME} PROPERTIES XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "7TFM6B4A93") # EJ6B39LVZL
        endif()

        set_xcode_property (${PROJECT_NAME} IPHONEOS_DEPLOYMENT_TARGET "7.0")
        TARGET_LINK_LIBRARIES (${PROJECT_NAME} "-framework UIKit")
    else()
        TARGET_LINK_LIBRARIES (${PROJECT_NAME} "-framework AppKit")
    endif()

    TARGET_LINK_LIBRARIES (${PROJECT_NAME} "-framework Foundation")
endif()
