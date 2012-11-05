project(RazorRandr)
cmake_minimum_required(VERSION 2.6)

if (NOT CMAKE_BUILD_TYPE)
    set ( CMAKE_BUILD_TYPE Release )
endif (NOT CMAKE_BUILD_TYPE)

if (CMAKE_BUILD_TYPE MATCHES [Dd]ebug)
    message(STATUS "Debug build")
    add_definitions(-DDEBUG)
else()
    message(STATUS "non-debug build")
    add_definitions(-DNDEBUG)
endif()

set(MAJOR_VERSION 0)
set(MINOR_VERSION 1)
set(PATCH_VERSION 0)

set(${PROJECTNAME}_VERSION ${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION})

subdirs(src)

set(DESKTOP_FILES
    razorxrandr.desktop
)

install(FILES ${DESKTOP_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications)
