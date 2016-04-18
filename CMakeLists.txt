# CMAKE -- project pcisph_solver
# some build environment configuration code is copied from other cmakelists.txt

cmake_minimum_required (VERSION 2.8.3 FATAL_ERROR)
project(pcisph)

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
 	message(STATUS "Setting build type to 'Release' as none was specified.")
 	set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
  	set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()
string(TOUPPER "${CMAKE_BUILD_TYPE}" U_CMAKE_BUILD_TYPE)

# Check if has NanoGUI library
if (NOT IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/packages/nanogui")
  	message(FATAL_ERROR "The dependency libraries (NanoGUI) are missing! ")
endif()

# Check if has TBB library
if (NOT IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/packages/tbb")
	message(FATAL_ERROR "The dependency libraries (TBB) are missing!")
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

elseif (UNIX)
  	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
  	if (APPLE)
    	# Try to auto-detect a suitable SDK
    	execute_process(COMMAND bash -c "xcodebuild -version -sdk | grep MacOSX | grep Path | head -n 1 | cut -f 2 -d ' '" OUTPUT_VARIABLE CMAKE_OSX_SYSROOT)
    	string(REGEX REPLACE "(\r?\n)+$" "" CMAKE_OSX_SYSROOT "${CMAKE_OSX_SYSROOT}")
    	string(REGEX REPLACE "^.*X([0-9.]*).sdk$" "\\1" CMAKE_OSX_DEPLOYMENT_TARGET "${CMAKE_OSX_SYSROOT}")
  	endif()
endif()

# Compile with compiler warnings
if (MSVC)
  	if (CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    	string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  	else()
    	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  	endif()
else()
  	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-unused-parameter -Wno-unused-function")
  	if (APPLE)
    	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-gnu-anonymous-struct -Wno-c99-extensions -Wno-nested-anon-types")
  	endif()
endif()

# Build NanoGUI
set(NANOGUI_BUILD_EXAMPLE OFF CACHE BOOL " " FORCE)
set(NANOGUI_BUILD_SHARED  OFF CACHE BOOL " " FORCE)
set(NANOGUI_BUILD_PYTHON  OFF CACHE BOOL " " FORCE)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/packages/nanogui ext_build/nanogui)

# Build TBB
set(TBB_BUILD_STATIC          ON  CACHE BOOL " " FORCE)
set(TBB_BUILD_SHARED          OFF CACHE BOOL " " FORCE)
set(TBB_BUILD_TBBMALLOC       OFF CACHE BOOL " " FORCE)
set(TBB_BUILD_TBBMALLOC_PROXY OFF CACHE BOOL " " FORCE)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/packages/tbb ext_build/tbb)

include_directories(
	# src
	${CMAKE_CURRENT_SOURCE_DIR}/src

	# EIGEN
	${CMAKE_CURRENT_SOURCE_DIR}/packages/nanogui/ext/eigen

	# GLFW
	${CMAKE_CURRENT_SOURCE_DIR}/packages/nanogui/ext/glfw/include

	# GLEW
	${CMAKE_CURRENT_SOURCE_DIR}/packages/nanogui/ext/glew/include

	# NanoVG
	${CMAKE_CURRENT_SOURCE_DIR}/packages/nanogui/ext/nanovg/src

	# NanoGUI
	${CMAKE_CURRENT_SOURCE_DIR}/packages/nanogui/include

	# TBB
	${CMAKE_CURRENT_SOURCE_DIR}/packages/tbb/include
)

add_executable(fluid_simulator
	src/app/App.cpp
	src/app/main.cpp
)
target_link_libraries(fluid_simulator nanogui tbb_static ${NANOGUI_EXTRA_LIBS})

