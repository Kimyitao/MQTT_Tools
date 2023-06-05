# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\MQTT_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\MQTT_autogen.dir\\ParseCache.txt"
  "MQTT_autogen"
  )
endif()
