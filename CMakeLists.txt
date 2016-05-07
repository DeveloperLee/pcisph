# PCISPH Project

cmake_minimum_required (VERSION 2.8.3)
project(pcisph)

# Environment configuration
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to 'Release' as none was specified.")
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()
string(TOUPPER "${CMAKE_BUILD_TYPE}" U_CMAKE_BUILD_TYPE)

# check Eigen library
if(NOT IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/packages/eigen")
    message(FATAL_ERROR "Eigen library not found, please add it into packages")
endif()

# check glew library
if(NOT IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/packages/glew")
    message(FATAL_ERROR "glew library not found, please add it into packages")
endif()

# check glfw library
if(NOT IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/packages/glfw")
    message(FATAL_ERROR "glfw library not found, please add it into packages")
endif()

# check soil library
if(NOT IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/packages/soil")
    message(FATAL_ERROR "soil library not found, please add it into packages")
endif()

# Sanitize build environment for static build with C++11
if (MSVC)
    add_definitions (/D "_CRT_SECURE_NO_WARNINGS")
    add_definitions (/D "__TBB_NO_IMPLICIT_LINKAGE")

    # Parallel build on MSVC (all targets)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")

    if (NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:SSE2")

        # Disable Eigen vectorization for Windows 32 bit builds (issues with unaligned access segfaults)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DDEIGEN_DONT_VECTORIZE /DEIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT")
    endif()

    # Static build
    set(CompilerFlags
        CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
        CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO
        CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
        CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO)
    
    foreach(CompilerFlag ${CompilerFlags})
        string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
    endforeach()

elseif(UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    if (APPLE)
        # Try to auto-detect a suitable SDK
        execute_process(COMMAND bash -c "xcodebuild -version -sdk | grep MacOSX | grep Path | head -n 1 | cut -f 2 -d ' '" OUTPUT_VARIABLE CMAKE_OSX_SYSROOT)
        string(REGEX REPLACE "(\r?\n)+$" "" CMAKE_OSX_SYSROOT "${CMAKE_OSX_SYSROOT}")
        string(REGEX REPLACE "^.*X([0-9.]*).sdk$" "\\1" CMAKE_OSX_DEPLOYMENT_TARGET "${CMAKE_OSX_SYSROOT}")
    endif()
endif()

# Platform-dependent files
set(PLATFORM_EXTRA_SOURCE "")
set(PLATFORM_EXTRA_LIBS "")

if(WIN32)
    # Build and include GLEW on Windows
    list(APPEND PLATFORM_EXTRA_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/packages/glew/src/glew.c")
    set_source_files_properties("${CMAKE_CURRENT_SOURCE_DIR}/packages/glew/src/glew.c" PROPERTIES COMPILE_DEFINITIONS GLEW_BUILD)
endif()

# Required core libraries on various platforms
if (WIN32)
    list(APPEND PLATFORM_EXTRA_LIBS opengl32)
elseif (APPLE)
    # Link with xcode core libs
    find_library(cocoa_library Cocoa)
    find_library(opengl_library OpenGL)
    find_library(corevideo_library CoreVideo)
    find_library(iokit_library IOKit)
    list(APPEND PLATFORM_EXTRA_LIBS ${cocoa_library} ${opengl_library} ${corevideo_library} ${iokit_library})
elseif("${CMAKE_SYSTEM}" MATCHES "Linux")
    list(APPEND PLATFORM_EXTRA_LIBS GL Xxf86vm Xrandr Xinerama Xcursor Xi X11 pthread dl rt)
endif()

# Build GLFW
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL " " FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL " " FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL " " FORCE)
set(GLFW_BUILD_INSTALL OFF CACHE BOOL " " FORCE)

# Get rid of annoying deprecation warnings when compiling GLFW on OSX
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-deprecated-declarations")
endif()

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/packages/glfw" "ext_build/glfw")

# Build TBB
set(TBB_BUILD_STATIC ON CACHE BOOL " " FORCE)
set(TBB_BUILD_SHARED OFF CACHE BOOL " " FORCE)
set(TBB_BUILD_TBBMALLOC OFF CACHE BOOL " " FORCE)
set(TBB_BUILD_TBBMALLOC_PROXY OFF CACHE BOOL " " FORCE)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/packages/tbb ext_build/tbb)

# Build SOIL
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/packages/soil ext_build/soil)

include_directories(
    # Source
    ${CMAKE_CURRENT_SOURCE_DIR}/src

    # Eigen
    ${CMAKE_CURRENT_SOURCE_DIR}/packages/eigen

    # GLFW
    ${CMAKE_CURRENT_SOURCE_DIR}/packages/glfw/include

    # GLEW
    ${CMAKE_CURRENT_SOURCE_DIR}/packages/glew/include

    # TBB
    ${CMAKE_CURRENT_SOURCE_DIR}/packages/tbb/include

    # json11
    ${CMAKE_CURRENT_SOURCE_DIR}/packages/json11

    # soil
    ${CMAKE_CURRENT_SOURCE_DIR}/packages/soil/src
    
    # Resource file
    ${CMAKE_CURRENT_BINARY_DIR}

    ${CMAKE_CURRENT_SOURCE_DIR}/shaders
)

# Create config file and add to include directories
configure_file("${PROJECT_SOURCE_DIR}/src/app/Config.h.in" "${PROJECT_BINARY_DIR}/Config.h")
include_directories(${PROJECT_BINARY_DIR})

add_library(core STATIC
  
    src/utils/Math.h
    src/utils/StringUtils.h
    src/utils/ConcurrentUtils.h
    src/utils/Def.h
    src/utils/Settings.h src/utils/Settings.cpp
    src/utils/ResourceLoader.h src/utils/ResourceLoader.cpp

    src/engine/camera/Camera.h src/engine/camera/Camera.cpp
    src/engine/Engine.h src/engine/Engine.cpp
  
    src/shading/PCIShader.h src/shading/PCIShader.cpp
    src/shading/FBO.h src/shading/FBO.cpp
    src/shading/shape.h src/shading/shape.cpp
    src/shading/DomainShader.h
    src/shading/ParticleShader.h
    src/shading/SSFRenderer.h src/shading/SSFRenderer.cpp
    src/shading/MeshShader.h

    shaders/quad.vert
    shaders/quad.frag
    shaders/blur.vert
    shaders/blur.frag

    src/gui/Window.h
    src/gui/GLWidget.h src/gui/GLWidget.cpp

    src/visualization/mesh/Mesh.h src/visualization/mesh/Mesh.cpp
    src/visualization/objLoader/ObjLoader.h 
    src/visualization/objLoader/ObjLoader.cpp
    src/visualization/grid/Grid.h
    src/visualization/particle/Particle.h src/visualization/particle/Particle.cpp
    src/visualization/scene/Scene.h src/visualization/scene/Scene.cpp
    src/visualization/scene/SceneWidgets.h

    src/visualization/geometry/SDF.h src/visualization/geometry/SDF.cpp
    src/visualization/geometry/Voxelizer.h src/visualization/geometry/Voxelizer.cpp
    src/visualization/geometry/VoxelGrid.h

    src/algorithm/Kernel.h
    src/algorithm/SPH.h src/algorithm/SPH.cpp

    packages/json11/json11.cpp
    
    ${PLATFORM_EXTRA_SOURCE}
)
target_link_libraries(core tbb_static glfw SOIL ${PLATFORM_EXTRA_LIBS})

add_executable(fluid_simulator #MACOSX_BUNDLE
    #resources.h resources.cpp
    src/app/main.cpp
)
target_link_libraries(fluid_simulator core)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}" ${CMAKE_MODULE_PATH})

