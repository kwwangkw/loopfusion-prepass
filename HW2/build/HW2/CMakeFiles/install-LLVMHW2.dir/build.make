# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/wujames/HW2

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/wujames/HW2/build

# Utility rule file for install-LLVMHW2.

# Include the progress variables for this target.
include HW2/CMakeFiles/install-LLVMHW2.dir/progress.make

HW2/CMakeFiles/install-LLVMHW2:
	cd /home/wujames/HW2/build/HW2 && /usr/bin/cmake -DCMAKE_INSTALL_COMPONENT="LLVMHW2" -P /home/wujames/HW2/build/cmake_install.cmake

install-LLVMHW2: HW2/CMakeFiles/install-LLVMHW2
install-LLVMHW2: HW2/CMakeFiles/install-LLVMHW2.dir/build.make

.PHONY : install-LLVMHW2

# Rule to build all files generated by this target.
HW2/CMakeFiles/install-LLVMHW2.dir/build: install-LLVMHW2

.PHONY : HW2/CMakeFiles/install-LLVMHW2.dir/build

HW2/CMakeFiles/install-LLVMHW2.dir/clean:
	cd /home/wujames/HW2/build/HW2 && $(CMAKE_COMMAND) -P CMakeFiles/install-LLVMHW2.dir/cmake_clean.cmake
.PHONY : HW2/CMakeFiles/install-LLVMHW2.dir/clean

HW2/CMakeFiles/install-LLVMHW2.dir/depend:
	cd /home/wujames/HW2/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/wujames/HW2 /home/wujames/HW2/HW2 /home/wujames/HW2/build /home/wujames/HW2/build/HW2 /home/wujames/HW2/build/HW2/CMakeFiles/install-LLVMHW2.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : HW2/CMakeFiles/install-LLVMHW2.dir/depend

