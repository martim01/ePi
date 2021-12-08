cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(epiworkspace)

option(BUILD_SERVER "Build episerver and player3" ON)
option(BUILD_CONTROLLER "Build controller and launcher" OFF)

SET(DIR_BASE $ENV{HOME} CACHE STRING "base location for libraries")
SET(DIR_LOG ${DIR_BASE}/log CACHE STRING "location of pml log")
SET(DIR_RESTGOOSE ${DIR_BASE}/Restgoose CACHE STRING "location of Restgoose")
SET(DIR_JSON ${DIR_BASE}/jsoncpp CACHE STRING "location of jsoncpp")

SET(BUILD_LOG ON CACHE BOOL "set to ON as pml::log built by Restgoose")

message(STATUS "Find external libraries")

#Get log
message(STATUS "Find pml::log")
find_path(TEMP_DIR NAMES "CMakeLists.txt" PATHS ${DIR_LOG})
if((NOT TEMP_DIR) OR (NOT EXISTS ${TEMP_DIR}))
	message(STATUS "log not found - clone from GitHub")
	execute_process(COMMAND git clone https://github.com/martim01/log/ ${DIR_LOG})

    find_path(TEMP_DIR2 NAMES "CMakeLists.txt" PATHS ${DIR_LOG})
    if((NOT TEMP_DIR2) OR (NOT EXISTS ${TEMP_DIR2}))
        message(FATAL_ERROR "Failed to clone pml::Log")
    endif()
else()
    message(STATUS "log found - update to latest version")
    execute_process(COMMAND git -C ${DIR_LOG} pull)
endif()

#Get Restgoose
find_path(EPI_TEMP_DIR NAMES "CMakeLists.txt" PATHS ${DIR_RESTGOOSE})
if((NOT EPI_TEMP_DIR) OR (NOT EXISTS ${EPI_TEMP_DIR}))
	message(STATUS "Restgoose not found - clone from GitHub")
	execute_process(COMMAND git clone https://github.com/martim01/Restgoose ${DIR_RESTGOOSE})
endif()
unset(EPI_TEMP_DIR)

find_path(EPI_TEMP_DIR2 NAMES "CMakeLists.txt" PATHS ${DIR_RESTGOOSE})
if((NOT EPI_TEMP_DIR2) OR (NOT EXISTS ${EPI_TEMP_DIR2}))
	message(FATAL_ERROR "Failed to clone Restgoose")
endif()
unset(EPI_TEMP_DIR2)



if(BUILD_SERVER MATCHES ON)
    message(STATUS "Configure Restgoose")
    add_subdirectory(${DIR_RESTGOOSE} ${DIR_RESTGOOSE}/build)
  #  message(STATUS "Configure player3")
  #  add_subdirectory(player3)
    message(STATUS "Configure episerver")
    add_subdirectory(server)
endif() 

#if(BUILD_CONTROLLER MATCHES ON)
#    message(STATUS "Configure controller")
#    add_subdirectory(controller)
#    message(STATUS "Configure launcher")
#    add_subdirectory(launcher)
#    message(STATUS "Configure cartcontroller")
#    add_subdirectory(cartcontroller)
#endif()
