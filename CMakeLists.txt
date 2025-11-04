cmake_minimum_required(VERSION 3.16)

project(
  ArduinoCodeConvertToRenesas
  VERSION 1.0.0
  LANGUAGES CXX
  DESCRIPTION " C++ application that fetches product data from a fake store API"
)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")


add_subdirectory(App)