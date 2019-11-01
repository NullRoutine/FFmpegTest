# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.
include_directories(include)
set(path_project D:/ChuangDeProject/FFmpegTest/app/src/main/jniLibs)
add_library( # Sets the name of the library.
        native-lib

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        native-lib.cpp)

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.
add_library(
        avcodec-56
        SHARED
        IMPORTED)
set_target_properties(avcodec-56
        PROPERTIES
        IMPORTED_LOCATION ${path_project}/${ANDROID_ABI}/libavcodec-56.so)

add_library(
        avdevice-56
        SHARED
        IMPORTED)
set_target_properties(avdevice-56
        PROPERTIES
        IMPORTED_LOCATION ${path_project}/${ANDROID_ABI}/libavdevice-56.so)

add_library(
        avfilter-5
        SHARED
        IMPORTED)
set_target_properties(avfilter-5
        PROPERTIES
        IMPORTED_LOCATION ${path_project}/${ANDROID_ABI}/libavfilter-5.so)

add_library(
        avformat-56
        SHARED
        IMPORTED)
set_target_properties(avformat-56
        PROPERTIES
        IMPORTED_LOCATION ${path_project}/${ANDROID_ABI}/libavformat-56.so)

add_library(
        avutil-54
        SHARED
        IMPORTED)
set_target_properties(avutil-54
        PROPERTIES
        IMPORTED_LOCATION ${path_project}/${ANDROID_ABI}/libavutil-54.so)

add_library(
        postproc-53
        SHARED
        IMPORTED)
set_target_properties(postproc-53
        PROPERTIES
        IMPORTED_LOCATION ${path_project}/${ANDROID_ABI}/libpostproc-53.so)

add_library(
        swresample-1
        SHARED
        IMPORTED)
set_target_properties(swresample-1
        PROPERTIES
        IMPORTED_LOCATION ${path_project}/${ANDROID_ABI}/libswresample-1.so)

add_library(
        swscale-3
        SHARED
        IMPORTED)
set_target_properties(swscale-3
        PROPERTIES
        IMPORTED_LOCATION ${path_project}/${ANDROID_ABI}/libswscale-3.so)

add_library(
        yuv
        SHARED
        IMPORTED )
set_target_properties(yuv
        PROPERTIES
        IMPORTED_LOCATION ${path_project}/${ANDROID_ABI}/libyuv.so)


find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)
find_library(
        android-lib
        android)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
        native-lib
        avcodec-56
        avdevice-56
        avformat-56
        avfilter-5
        avutil-54
        postproc-53
        swresample-1
        swscale-3
        yuv
        # Links the target library to the log library
        # included in the NDK.
        ${log-lib}
        ${android-lib}
        )