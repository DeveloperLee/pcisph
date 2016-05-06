# Install script for directory: /gpfs/main/home/sj4/course/cs224final/pcisph/version0.1/packages/soil

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/gpfs/main/home/sj4/course/cs224final/pcisph/version0.1/packages/soil/build/libSOIL.a")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/SOIL" TYPE FILE FILES
    "/gpfs/main/home/sj4/course/cs224final/pcisph/version0.1/packages/soil/src/SOIL.h"
    "/gpfs/main/home/sj4/course/cs224final/pcisph/version0.1/packages/soil/src/image_DXT.h"
    "/gpfs/main/home/sj4/course/cs224final/pcisph/version0.1/packages/soil/src/image_helper.h"
    "/gpfs/main/home/sj4/course/cs224final/pcisph/version0.1/packages/soil/src/stb_image_aug.h"
    "/gpfs/main/home/sj4/course/cs224final/pcisph/version0.1/packages/soil/src/stbi_DDS_aug.h"
    "/gpfs/main/home/sj4/course/cs224final/pcisph/version0.1/packages/soil/src/stbi_DDS_aug_c.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

file(WRITE "/gpfs/main/home/sj4/course/cs224final/pcisph/version0.1/packages/soil/build/${CMAKE_INSTALL_MANIFEST}" "")
foreach(file ${CMAKE_INSTALL_MANIFEST_FILES})
  file(APPEND "/gpfs/main/home/sj4/course/cs224final/pcisph/version0.1/packages/soil/build/${CMAKE_INSTALL_MANIFEST}" "${file}\n")
endforeach()
