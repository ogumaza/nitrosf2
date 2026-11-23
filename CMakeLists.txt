# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.20)
# nitrosf2 --version reports this version. The file properties of the Windows program and the Info.plist of the macOS
# app show it too.
project(nitrosf2 VERSION 0.3.0 LANGUAGES CXX)

# The submodules: seqlib writes the MIDI, SoundFont and SFZ files. ntdlib contains the conversion of the DS's sound
# archives (ntdlib::sequence_export_ntr) and of Procyon Studio's driver (ntdlib::sequence_export_dse), the games'
# profiles as the conversion's options (ntdlib::sequence_export_ntr_profiles) and the converters' command-line frame
# (ntdlib::sequence_export). xsflib contains the common code that ntdlib builds on, and the CMake helpers. xsflib's
# XsflibProject.cmake sets the C++ standard, a release build by default, the static C runtime and the compiler options
# that keep the output the same on every platform. xsflib_project turns on NTDLIB_SEQUENCE_EXPORT, ntdlib's conversion
# of sequences, then adds the libraries. It also turns off clang-cl's -Wmissing-designated-field-initializers from
# clang 19 on, for every target and the libraries. A designated initializer names only the fields that differ from
# their defaults.
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/externals/xsflib/CMakeLists.txt")
  message(FATAL_ERROR "externals/xsflib is missing. Run: git submodule update --init")
endif()
include(externals/xsflib/cmake/XsflibProject.cmake)
xsflib_project(WARNINGS_ON_EVERY_TARGET ENABLE NTDLIB_SEQUENCE_EXPORT LIBRARIES seqlib xsflib ntdlib)

add_executable(nitrosf2 src/main.cpp)
target_link_libraries(nitrosf2 PRIVATE ntdlib::sequence_export_ntr_profiles)
xsflib_add_version(nitrosf2 VERSION ${PROJECT_VERSION} DEFINE NITROSF2_VERSION
                   DESCRIPTION "Nintendo DS music converter" PRODUCT nitrosf2
                   COPYRIGHT "Copyright (c) 2026 the nitrosf2 authors. MIT licence.")

# Windows: UTF-8 as the code page. An image or a folder whose name the system's code page can't represent then opens.
xsflib_add_utf8_manifest(nitrosf2)

# Finder cannot drop files onto a command-line executable. Package it in a macOS droplet app, with the version in its
# Info.plist.
xsflib_add_droplet_tool(nitrosf2 APP_NAME Nitrosf2 VERSION ${PROJECT_VERSION}
                        DESCRIPTION "Drop DS or DSi cartridge images (.nds) or sound archives (.sdat) here to convert their music to MIDI, SoundFont and SFZ. Output goes in a folder beside each one."
                        PROMPT "Choose DS or DSi cartridge images (.nds) or sound archives (.sdat) to convert:")

include(CTest)
if(BUILD_TESTING)
  # The program reports the version it was built as, and refuses a variable outside the driver's 32.
  xsflib_add_version_test(nitrosf2 VERSION ${PROJECT_VERSION})
  xsflib_add_var_range_test(nitrosf2 EXTENSION .sdat)

  # Load local_tests.cmake from each immediate subdirectory to register tests that use locally supplied files.
  xsflib_include_local_tests()
endif()
