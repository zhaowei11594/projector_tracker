# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

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

# The program to use to edit the cache.
CMAKE_EDIT_COMMAND = /usr/bin/ccmake

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/justin/projector_tracker

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/justin/projector_tracker/release

# Include any dependencies generated for this target.
include CMakeFiles/camera_calibration.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/camera_calibration.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/camera_calibration.dir/flags.make

CMakeFiles/camera_calibration.dir/src/CameraCalibration.cpp.o: CMakeFiles/camera_calibration.dir/flags.make
CMakeFiles/camera_calibration.dir/src/CameraCalibration.cpp.o: ../src/CameraCalibration.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/justin/projector_tracker/release/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/camera_calibration.dir/src/CameraCalibration.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/camera_calibration.dir/src/CameraCalibration.cpp.o -c /home/justin/projector_tracker/src/CameraCalibration.cpp

CMakeFiles/camera_calibration.dir/src/CameraCalibration.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/camera_calibration.dir/src/CameraCalibration.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/justin/projector_tracker/src/CameraCalibration.cpp > CMakeFiles/camera_calibration.dir/src/CameraCalibration.cpp.i

CMakeFiles/camera_calibration.dir/src/CameraCalibration.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/camera_calibration.dir/src/CameraCalibration.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/justin/projector_tracker/src/CameraCalibration.cpp -o CMakeFiles/camera_calibration.dir/src/CameraCalibration.cpp.s

CMakeFiles/camera_calibration.dir/src/CameraCalibration.cpp.o.requires:
.PHONY : CMakeFiles/camera_calibration.dir/src/CameraCalibration.cpp.o.requires

CMakeFiles/camera_calibration.dir/src/CameraCalibration.cpp.o.provides: CMakeFiles/camera_calibration.dir/src/CameraCalibration.cpp.o.requires
	$(MAKE) -f CMakeFiles/camera_calibration.dir/build.make CMakeFiles/camera_calibration.dir/src/CameraCalibration.cpp.o.provides.build
.PHONY : CMakeFiles/camera_calibration.dir/src/CameraCalibration.cpp.o.provides

CMakeFiles/camera_calibration.dir/src/CameraCalibration.cpp.o.provides.build: CMakeFiles/camera_calibration.dir/src/CameraCalibration.cpp.o

# Object files for target camera_calibration
camera_calibration_OBJECTS = \
"CMakeFiles/camera_calibration.dir/src/CameraCalibration.cpp.o"

# External object files for target camera_calibration
camera_calibration_EXTERNAL_OBJECTS =

libcamera_calibration.a: CMakeFiles/camera_calibration.dir/src/CameraCalibration.cpp.o
libcamera_calibration.a: CMakeFiles/camera_calibration.dir/build.make
libcamera_calibration.a: CMakeFiles/camera_calibration.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX static library libcamera_calibration.a"
	$(CMAKE_COMMAND) -P CMakeFiles/camera_calibration.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/camera_calibration.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/camera_calibration.dir/build: libcamera_calibration.a
.PHONY : CMakeFiles/camera_calibration.dir/build

CMakeFiles/camera_calibration.dir/requires: CMakeFiles/camera_calibration.dir/src/CameraCalibration.cpp.o.requires
.PHONY : CMakeFiles/camera_calibration.dir/requires

CMakeFiles/camera_calibration.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/camera_calibration.dir/cmake_clean.cmake
.PHONY : CMakeFiles/camera_calibration.dir/clean

CMakeFiles/camera_calibration.dir/depend:
	cd /home/justin/projector_tracker/release && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/justin/projector_tracker /home/justin/projector_tracker /home/justin/projector_tracker/release /home/justin/projector_tracker/release /home/justin/projector_tracker/release/CMakeFiles/camera_calibration.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/camera_calibration.dir/depend
