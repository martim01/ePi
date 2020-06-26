cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(epiworkspace)

#log
find_path(LOG_LIB_DIR NAMES "include/log.h" PATHS ${PROJECT_SOURCE_DIR}/external/log)
if((NOT LOG_LIB_DIR) OR (NOT EXISTS ${LOG_LIB_DIR}))
    # we couldn't find the header files for FOO or they don't exist
    message("Unable to find pml::log")
    
    # we have a submodule setup for pml_log
    # now we need to clone this submodule
    execute_process(COMMAND git submodule update --init -- external/log WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
    find_path(LOG_LIB_DIR NAMES "include/log.h" PATHS ${PROJECT_SOURCE_DIR}/external/log)
else()
    message("Found pml::log")
endif()

#json
find_path(JSON_DIR NAMES "amalgamate.py" PATHS ${PROJECT_SOURCE_DIR}/external/jsoncpp)
if((NOT JSON_DIR) OR (NOT EXISTS ${JSON_DIR}))
    # we couldn't find the header files for FOO or they don't exist
    message("Unable to find jsoncpp")
    
    # we have a submodule setup for pml_log
    # now we need to clone this submodule
    execute_process(COMMAND git submodule update --init -- external/jsoncpp WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
    find_path(JSON_DIR NAMES "amalgamate.py" PATHS ${PROJECT_SOURCE_DIR}/external/jsoncpp)
    execute_process(COMMAND ${JSON_DIR}/amalgamate.py WORKING_DIRECTORY ${JSON_DIR})
else()
    message("Found jsoncpp")
endif()


message(STATUS "Configure player3")
add_subdirectory(player3)
message(STATUS "Configure episerver")
add_subdirectory(server)
message(STATUS "Configure controller")
add_subdirectory(controller)