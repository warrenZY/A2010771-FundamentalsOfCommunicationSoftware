# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.18

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Disable VCS-based implicit rules.
% : %,v


# Disable VCS-based implicit rules.
% : RCS/%


# Disable VCS-based implicit rules.
% : RCS/%,v


# Disable VCS-based implicit rules.
% : SCCS/s.%


# Disable VCS-based implicit rules.
% : s.%


.SUFFIXES: .hpux_make_needs_suffix_list


# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "C:\Program Files\CMake\bin\cmake.exe"

# The command to remove a file.
RM = "C:\Program Files\CMake\bin\cmake.exe" -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = E:\WORKPLACE\LiteCV

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = E:\WORKPLACE\LiteCV\BuildWin32

# Include any dependencies generated for this target.
include CMakeFiles/conv.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/conv.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/conv.dir/flags.make

CMakeFiles/conv.dir/example/conv/conv.c.obj: CMakeFiles/conv.dir/flags.make
CMakeFiles/conv.dir/example/conv/conv.c.obj: CMakeFiles/conv.dir/includes_C.rsp
CMakeFiles/conv.dir/example/conv/conv.c.obj: ../example/conv/conv.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=E:\WORKPLACE\LiteCV\BuildWin32\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/conv.dir/example/conv/conv.c.obj"
	C:\MinGW\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles\conv.dir\example\conv\conv.c.obj -c E:\WORKPLACE\LiteCV\example\conv\conv.c

CMakeFiles/conv.dir/example/conv/conv.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/conv.dir/example/conv/conv.c.i"
	C:\MinGW\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E E:\WORKPLACE\LiteCV\example\conv\conv.c > CMakeFiles\conv.dir\example\conv\conv.c.i

CMakeFiles/conv.dir/example/conv/conv.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/conv.dir/example/conv/conv.c.s"
	C:\MinGW\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S E:\WORKPLACE\LiteCV\example\conv\conv.c -o CMakeFiles\conv.dir\example\conv\conv.c.s

# Object files for target conv
conv_OBJECTS = \
"CMakeFiles/conv.dir/example/conv/conv.c.obj"

# External object files for target conv
conv_EXTERNAL_OBJECTS =

conv.exe: CMakeFiles/conv.dir/example/conv/conv.c.obj
conv.exe: CMakeFiles/conv.dir/build.make
conv.exe: Litecv/liblicvcore.a
conv.exe: Litecv/liblicvimgproc.a
conv.exe: ../Litecv/3rd/win_3rd_lib/jpeg_win_install/lib/libjpeg.a
conv.exe: ../Litecv/3rd/win_3rd_lib/png_win_install/lib/libpng.a
conv.exe: ../Litecv/3rd/win_3rd_lib/zlib_win_install/lib/libz.a
conv.exe: ../Litecv/3rd/win_3rd_lib/qr_win_install/lib/libqrencode.a
conv.exe: CMakeFiles/conv.dir/linklibs.rsp
conv.exe: CMakeFiles/conv.dir/objects1.rsp
conv.exe: CMakeFiles/conv.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=E:\WORKPLACE\LiteCV\BuildWin32\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable conv.exe"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\conv.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/conv.dir/build: conv.exe

.PHONY : CMakeFiles/conv.dir/build

CMakeFiles/conv.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\conv.dir\cmake_clean.cmake
.PHONY : CMakeFiles/conv.dir/clean

CMakeFiles/conv.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" E:\WORKPLACE\LiteCV E:\WORKPLACE\LiteCV E:\WORKPLACE\LiteCV\BuildWin32 E:\WORKPLACE\LiteCV\BuildWin32 E:\WORKPLACE\LiteCV\BuildWin32\CMakeFiles\conv.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/conv.dir/depend

