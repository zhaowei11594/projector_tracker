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
CMAKE_SOURCE_DIR = /home/duy/projector_tracker

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/duy/projector_tracker/release

# Include any dependencies generated for this target.
include CMakeFiles/test_node.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/test_node.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/test_node.dir/flags.make

CMakeFiles/test_node.dir/test/test.cpp.o: CMakeFiles/test_node.dir/flags.make
CMakeFiles/test_node.dir/test/test.cpp.o: ../test/test.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/duy/projector_tracker/release/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/test_node.dir/test/test.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/test_node.dir/test/test.cpp.o -c /home/duy/projector_tracker/test/test.cpp

CMakeFiles/test_node.dir/test/test.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/test_node.dir/test/test.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/duy/projector_tracker/test/test.cpp > CMakeFiles/test_node.dir/test/test.cpp.i

CMakeFiles/test_node.dir/test/test.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/test_node.dir/test/test.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/duy/projector_tracker/test/test.cpp -o CMakeFiles/test_node.dir/test/test.cpp.s

CMakeFiles/test_node.dir/test/test.cpp.o.requires:
.PHONY : CMakeFiles/test_node.dir/test/test.cpp.o.requires

CMakeFiles/test_node.dir/test/test.cpp.o.provides: CMakeFiles/test_node.dir/test/test.cpp.o.requires
	$(MAKE) -f CMakeFiles/test_node.dir/build.make CMakeFiles/test_node.dir/test/test.cpp.o.provides.build
.PHONY : CMakeFiles/test_node.dir/test/test.cpp.o.provides

CMakeFiles/test_node.dir/test/test.cpp.o.provides.build: CMakeFiles/test_node.dir/test/test.cpp.o

# Object files for target test_node
test_node_OBJECTS = \
"CMakeFiles/test_node.dir/test/test.cpp.o"

# External object files for target test_node
test_node_EXTERNAL_OBJECTS =

test_node: CMakeFiles/test_node.dir/test/test.cpp.o
test_node: CMakeFiles/test_node.dir/build.make
test_node: libhw_interface.a
test_node: libprojector_tracker.a
test_node: /usr/lib/x86_64-linux-gnu/libQtGui.so
test_node: /usr/lib/x86_64-linux-gnu/libQtCore.so
test_node: /usr/local/lib/libopencv_xphoto.so.3.1.0
test_node: /usr/local/lib/libopencv_xobjdetect.so.3.1.0
test_node: /usr/local/lib/libopencv_tracking.so.3.1.0
test_node: /usr/local/lib/libopencv_surface_matching.so.3.1.0
test_node: /usr/local/lib/libopencv_structured_light.so.3.1.0
test_node: /usr/local/lib/libopencv_stereo.so.3.1.0
test_node: /usr/local/lib/libopencv_saliency.so.3.1.0
test_node: /usr/local/lib/libopencv_rgbd.so.3.1.0
test_node: /usr/local/lib/libopencv_reg.so.3.1.0
test_node: /usr/local/lib/libopencv_plot.so.3.1.0
test_node: /usr/local/lib/libopencv_optflow.so.3.1.0
test_node: /usr/local/lib/libopencv_ximgproc.so.3.1.0
test_node: /usr/local/lib/libopencv_line_descriptor.so.3.1.0
test_node: /usr/local/lib/libopencv_fuzzy.so.3.1.0
test_node: /usr/local/lib/libopencv_dpm.so.3.1.0
test_node: /usr/local/lib/libopencv_dnn.so.3.1.0
test_node: /usr/local/lib/libopencv_datasets.so.3.1.0
test_node: /usr/local/lib/libopencv_text.so.3.1.0
test_node: /usr/local/lib/libopencv_face.so.3.1.0
test_node: /usr/local/lib/libopencv_ccalib.so.3.1.0
test_node: /usr/local/lib/libopencv_bioinspired.so.3.1.0
test_node: /usr/local/lib/libopencv_bgsegm.so.3.1.0
test_node: /usr/local/lib/libopencv_aruco.so.3.1.0
test_node: /usr/local/lib/libopencv_videostab.so.3.1.0
test_node: /usr/local/lib/libopencv_superres.so.3.1.0
test_node: /usr/local/lib/libopencv_stitching.so.3.1.0
test_node: /usr/local/lib/libopencv_xfeatures2d.so.3.1.0
test_node: /usr/local/lib/libopencv_shape.so.3.1.0
test_node: /usr/local/lib/libopencv_video.so.3.1.0
test_node: /usr/local/lib/libopencv_photo.so.3.1.0
test_node: /usr/local/lib/libopencv_objdetect.so.3.1.0
test_node: /usr/local/lib/libopencv_calib3d.so.3.1.0
test_node: /usr/local/lib/libopencv_features2d.so.3.1.0
test_node: /usr/local/lib/libopencv_ml.so.3.1.0
test_node: /usr/local/lib/libopencv_highgui.so.3.1.0
test_node: /usr/local/lib/libopencv_videoio.so.3.1.0
test_node: /usr/local/lib/libopencv_imgcodecs.so.3.1.0
test_node: /usr/local/lib/libopencv_imgproc.so.3.1.0
test_node: /usr/local/lib/libopencv_flann.so.3.1.0
test_node: /usr/local/lib/libopencv_core.so.3.1.0
test_node: CMakeFiles/test_node.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable test_node"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/test_node.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/test_node.dir/build: test_node
.PHONY : CMakeFiles/test_node.dir/build

CMakeFiles/test_node.dir/requires: CMakeFiles/test_node.dir/test/test.cpp.o.requires
.PHONY : CMakeFiles/test_node.dir/requires

CMakeFiles/test_node.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/test_node.dir/cmake_clean.cmake
.PHONY : CMakeFiles/test_node.dir/clean

CMakeFiles/test_node.dir/depend:
	cd /home/duy/projector_tracker/release && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/duy/projector_tracker /home/duy/projector_tracker /home/duy/projector_tracker/release /home/duy/projector_tracker/release /home/duy/projector_tracker/release/CMakeFiles/test_node.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/test_node.dir/depend

