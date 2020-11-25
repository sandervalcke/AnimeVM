#
# Top-level CMakeLists.txt file for the flowshow project
#
project(ViewManager)

# Set the version number.
set (ViewManager_VERSION_MAJOR 0)
set (ViewManager_VERSION_MINOR 2)

add_definitions( -Wall)

####### Set some generic stuff #######

cmake_minimum_required(VERSION 3.14)

# This is a C++ project
enable_language(CXX)
set (CMAKE_CXX_STANDARD 17)

# Location where cmake build system first looks for cmake modules.
set(CMAKE_MODULE_PATH ${ViewManager_SOURCE_DIR}/cmake)

####### Check for installed libraries/exes #######

#--> Qt

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Find the QtWidgets library
find_package(Qt5 COMPONENTS Core Quick Xml XmlPatterns Gui Qml Concurrent Sql Widgets REQUIRED)

####### Build the ViewManager project #######

# Set include dirs
include_directories( 
  ${QT_INCLUDES} 
  ${ViewManager_SOURCE_DIR}/inc
  ${ViewManager_BINARY_DIR}/src  # for the ui_* files
  )

#add_subdirectory( designer )

# Compile and link the program
add_subdirectory( src )
