# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\SortingAlgorithms_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\SortingAlgorithms_autogen.dir\\ParseCache.txt"
  "SortingAlgorithms_autogen"
  )
endif()
