#

# Try to find SimpleRenderEngine library and include path.
# Once done this will define
#
# SRE_FOUND
# SRE_INCLUDE_DIR
# SRE_LIBRARY
#
IF(WIN32)
    FIND_PATH( SRE_INCLUDE_DIR
            NAMES
                SRE/SimpleRenderEngine.hpp
            PATHS
                $ENV{PROGRAMFILES}/SRE
                ${SRE_ROOT_DIR}
                $ENV{SRE_ROOT_DIR}/include
                $ENV{SRE_ROOT_DIR}
                DOC "The directory where SRE/SimpleRenderEngine.hpp resides")
ELSE(WIN32)
    FIND_PATH( SRE_INCLUDE_DIR
            NAMES
                SRE/SimpleRenderEngine.hpp
            PATHS
                /usr/include
                /usr/local/include
                /sw/include
                /opt/local/include
                ${SRE_ROOT_DIR}
                $ENV{SRE_ROOT_DIR}/include
                $ENV{SRE_ROOT_DIR}
                DOC "The directory where SRE/SimpleRenderEngine.hpp resides"
            )
ENDIF(WIN32)

IF(WIN32)
    find_library( SRE_DEBUG_LIBRARY
        NAMES
            SRE.lib
        PATHS
            ${SRE_LOCATION}/lib
            DOC "The Debug libSRE.lib library")
    find_library( SRE_RELEASE_LIBRARY
        NAMES
            SRE.lib
        PATHS
            ${SRE_LOCATION}/lib
            DOC "The Release libSRE.lib library")
ELSE(WIN32)
    find_library( SRE_LIBRARY
            NAMES
            libSRE.a
            PATHS
            /usr/lib
            /usr/local/lib
            ${SRE_LOCATION}/lib
            DOC "The libSRE.a library")
ENDIF(WIN32)

IF(SRE_LIBRARY OR (SRE_DEBUG_LIBRARY OR SRE_RELEASE_LIBRARY))
    IF(SRE_INCLUDE_DIR)
        SET(SRE_FOUND "YES")
        MESSAGE(STATUS "Found SRE.")
    ENDIF(SRE_INCLUDE_DIR)
ENDIF()
