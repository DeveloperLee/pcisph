# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.4

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/Cellar/cmake/3.4.1/bin/cmake

# The command to remove a file.
RM = /usr/local/Cellar/cmake/3.4.1/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/LOVEME/Desktop/copy

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/LOVEME/Desktop/copy/build

# Include any dependencies generated for this target.
include ext_build/glfw/src/CMakeFiles/glfw.dir/depend.make

# Include the progress variables for this target.
include ext_build/glfw/src/CMakeFiles/glfw.dir/progress.make

# Include the compile flags for this target's objects.
include ext_build/glfw/src/CMakeFiles/glfw.dir/flags.make

# Object files for target glfw
glfw_OBJECTS =

# External object files for target glfw
glfw_EXTERNAL_OBJECTS = \
"/Users/LOVEME/Desktop/copy/build/ext_build/glfw/src/CMakeFiles/glfw_objects.dir/context.c.o" \
"/Users/LOVEME/Desktop/copy/build/ext_build/glfw/src/CMakeFiles/glfw_objects.dir/init.c.o" \
"/Users/LOVEME/Desktop/copy/build/ext_build/glfw/src/CMakeFiles/glfw_objects.dir/input.c.o" \
"/Users/LOVEME/Desktop/copy/build/ext_build/glfw/src/CMakeFiles/glfw_objects.dir/monitor.c.o" \
"/Users/LOVEME/Desktop/copy/build/ext_build/glfw/src/CMakeFiles/glfw_objects.dir/window.c.o" \
"/Users/LOVEME/Desktop/copy/build/ext_build/glfw/src/CMakeFiles/glfw_objects.dir/cocoa_init.m.o" \
"/Users/LOVEME/Desktop/copy/build/ext_build/glfw/src/CMakeFiles/glfw_objects.dir/cocoa_monitor.m.o" \
"/Users/LOVEME/Desktop/copy/build/ext_build/glfw/src/CMakeFiles/glfw_objects.dir/cocoa_window.m.o" \
"/Users/LOVEME/Desktop/copy/build/ext_build/glfw/src/CMakeFiles/glfw_objects.dir/iokit_joystick.m.o" \
"/Users/LOVEME/Desktop/copy/build/ext_build/glfw/src/CMakeFiles/glfw_objects.dir/mach_time.c.o" \
"/Users/LOVEME/Desktop/copy/build/ext_build/glfw/src/CMakeFiles/glfw_objects.dir/posix_tls.c.o" \
"/Users/LOVEME/Desktop/copy/build/ext_build/glfw/src/CMakeFiles/glfw_objects.dir/nsgl_context.m.o"

ext_build/glfw/src/libglfw3.a: ext_build/glfw/src/CMakeFiles/glfw_objects.dir/context.c.o
ext_build/glfw/src/libglfw3.a: ext_build/glfw/src/CMakeFiles/glfw_objects.dir/init.c.o
ext_build/glfw/src/libglfw3.a: ext_build/glfw/src/CMakeFiles/glfw_objects.dir/input.c.o
ext_build/glfw/src/libglfw3.a: ext_build/glfw/src/CMakeFiles/glfw_objects.dir/monitor.c.o
ext_build/glfw/src/libglfw3.a: ext_build/glfw/src/CMakeFiles/glfw_objects.dir/window.c.o
ext_build/glfw/src/libglfw3.a: ext_build/glfw/src/CMakeFiles/glfw_objects.dir/cocoa_init.m.o
ext_build/glfw/src/libglfw3.a: ext_build/glfw/src/CMakeFiles/glfw_objects.dir/cocoa_monitor.m.o
ext_build/glfw/src/libglfw3.a: ext_build/glfw/src/CMakeFiles/glfw_objects.dir/cocoa_window.m.o
ext_build/glfw/src/libglfw3.a: ext_build/glfw/src/CMakeFiles/glfw_objects.dir/iokit_joystick.m.o
ext_build/glfw/src/libglfw3.a: ext_build/glfw/src/CMakeFiles/glfw_objects.dir/mach_time.c.o
ext_build/glfw/src/libglfw3.a: ext_build/glfw/src/CMakeFiles/glfw_objects.dir/posix_tls.c.o
ext_build/glfw/src/libglfw3.a: ext_build/glfw/src/CMakeFiles/glfw_objects.dir/nsgl_context.m.o
ext_build/glfw/src/libglfw3.a: ext_build/glfw/src/CMakeFiles/glfw.dir/build.make
ext_build/glfw/src/libglfw3.a: ext_build/glfw/src/CMakeFiles/glfw.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/LOVEME/Desktop/copy/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Linking C static library libglfw3.a"
	cd /Users/LOVEME/Desktop/copy/build/ext_build/glfw/src && $(CMAKE_COMMAND) -P CMakeFiles/glfw.dir/cmake_clean_target.cmake
	cd /Users/LOVEME/Desktop/copy/build/ext_build/glfw/src && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/glfw.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
ext_build/glfw/src/CMakeFiles/glfw.dir/build: ext_build/glfw/src/libglfw3.a

.PHONY : ext_build/glfw/src/CMakeFiles/glfw.dir/build

ext_build/glfw/src/CMakeFiles/glfw.dir/requires:

.PHONY : ext_build/glfw/src/CMakeFiles/glfw.dir/requires

ext_build/glfw/src/CMakeFiles/glfw.dir/clean:
	cd /Users/LOVEME/Desktop/copy/build/ext_build/glfw/src && $(CMAKE_COMMAND) -P CMakeFiles/glfw.dir/cmake_clean.cmake
.PHONY : ext_build/glfw/src/CMakeFiles/glfw.dir/clean

ext_build/glfw/src/CMakeFiles/glfw.dir/depend:
	cd /Users/LOVEME/Desktop/copy/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/LOVEME/Desktop/copy /Users/LOVEME/Desktop/copy/packages/glfw/src /Users/LOVEME/Desktop/copy/build /Users/LOVEME/Desktop/copy/build/ext_build/glfw/src /Users/LOVEME/Desktop/copy/build/ext_build/glfw/src/CMakeFiles/glfw.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : ext_build/glfw/src/CMakeFiles/glfw.dir/depend

