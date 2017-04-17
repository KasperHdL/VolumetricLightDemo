cmake_minimum_required(VERSION 3.6)
project(WalkingSim CXX)

IF(UNIX)
    set(CMAKE_BUILD_TYPE Debug)

    IF(CMAKE_BUILD_TYPE MATCHES Debug)
        set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
        set(DEBUG_MODE TRUE)
    ELSEIF(CMAKE_BUILD_TYPE MATCHES Release)
        set(DEBUG_MODE FALSE)
    ENDIF()
ELSE(UNIX)
    set(DEBUG_MODE TRUE CACHE BOOL "Build mode")
ENDIF(UNIX) 

IF(DEBUG_MODE)
    message("----- Debug Build -----")
    add_definitions(-DDEBUG)
ELSE()
    message("----- Release Build -----")
ENDIF()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

#########################################################
# FIND OPENGL
#########################################################
IF(APPLE)
    INCLUDE_DIRECTORIES ( /System/Library/Frameworks )
    FIND_LIBRARY(OpenGL_LIBRARY OpenGL )
    MARK_AS_ADVANCED (OpenGL_LIBRARY)
    SET(EXTRA_LIBS ${OpenGL_LIBRARY})
ELSE (APPLE)
    find_package(OpenGL REQUIRED)
    include_directories(${OPENGL_INCLUDE_DIR})
    IF(UNIX)
        find_package(GLEW REQUIRED)
    ELSE(UNIX)
        find_package(glew REQUIRED)
    ENDIF(UNIX)
    include_directories(${GLEW_INCLUDE_DIR})
    SET(EXTRA_LIBS ${EXTRA_LIBS} ${OPENGL_LIBRARY} ${GLEW_LIBRARY})
ENDIF (APPLE)
 
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})

find_package(SDL2_IMAGE REQUIRED)
include_directories(${SDL2_IMAGE_INCLUDE_DIRS})

find_package(SDL2_MIXER REQUIRED)
include_directories(${SDL2_MIXER_INCLUDE_DIRS})

include_directories(include)

SET(EXTRA_LIBS ${EXTRA_LIBS} ${SDL2_LIBRARY} ${SDL2_MIXER_LIBRARY} ${SDL2_IMAGE_LIBRARY})

IF(WIN32)
    if(DEBUG_MODE)
        SET(EXTRA_LIBS ${EXTRA_LIBS}) #DEBUG LIBS?
    else()
        SET(EXTRA_LIBS ${EXTRA_LIBS}) #RELEASE LIBS?
    endif()
ELSE(WIN32)
    SET(EXTRA_LIBS ${EXTRA_LIBS})
ENDIF(WIN32)

file(GLOB_RECURSE GAME_FILES
    "src/*.cpp"
    "src/*.hpp"
)

add_executable(Game ${GAME_FILES})
target_link_libraries(Game ${EXTRA_LIBS})

file(COPY data DESTINATION .)

IF (WIN32)
    SET(SDL2_dll "" CACHE STRING "Where the SDL2.dll can be found")
    SET(SDL2_image_lib_path "" CACHE PATH "Where the SDL2_image.dll can be found")
    SET(DLLFileList "${SDL2_image_lib_path}/SDL2_image.dll" "${SDL2_image_lib_path}/libjpeg-9.dll" "${SDL2_image_lib_path}/libpng16-16.dll" "${SDL2_image_lib_path}/libtiff-5.dll" "${SDL2_image_lib_path}/libwebp-4.dll" "${SDL2_image_lib_path}/zlib1.dll" ${SDL2_dll})
    file(COPY ${DLLFileList} DESTINATION Debug)
    file(COPY ${DLLFileList} DESTINATION Release)
ELSE(WIN32)
    file(COPY data DESTINATION .)
ENDIF(WIN32)
