cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(episerver LANGUAGES CXX C VERSION 2.0.1)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

execute_process(COMMAND ${CMAKE_COMMAND} -DNAMESPACE=epi -DMAJOR=${PROJECT_VERSION_MAJOR} -DMINOR=${PROJECT_VERSION_MINOR} -DPATCH=${PROJECT_VERSION_PATCH} -P ${CMAKE_CURRENT_SOURCE_DIR}/version.cmake)

add_executable(episerver 
 "../common/src/epicron.cpp" 
 "../common/src/inimanager.cpp" 
 "../common/src/inisection.cpp" 
 "../common/src/epiutils.cpp" 
 "../common/src/jsonutils.cpp" 
 "../external/uuid/guid.cpp" 
 "../external/uuid/md5.cpp" 
 "src/audiofile.cpp"
 "src/core.cpp"
 "src/launcher.cpp"
 "src/main.cpp"
 "src/version.cpp"
 "src/mp3file.cpp" "src/proccheck.cpp" "src/resource.cpp" "src/ntpstatus.cpp"
 "src/resourcemanager.cpp" "src/sysinfomanager.cpp" "src/uidutils.cpp" "src/wavfile.cpp")


list(APPEND flags "-fPIC" "-Wall" "-fpermissive" "-O3" "-pthread")
target_compile_options(episerver PRIVATE ${flags})
target_compile_definitions(episerver PUBLIC NDEBUG MG_ENABLE_SSL MG_ENABLE_HTTP_STREAMING_MULTIPART MG_HIDE_SERVER_INFO)

find_package(PkgConfig REQUIRED QUIET)
pkg_search_module(sndfile REQUIRED sndfile IMPORTED_TARGET)
if(TARGET PkgConfig::sndfile)
	message(STATUS "Found sndfile")
	target_link_libraries(episerver PkgConfig::sndfile)
endif()

include_directories(${DIR_JSON}/dist)
include_directories(${DIR_LOG}/include)
include_directories(${DIR_RESTGOOSE}/include)
include_directories(${PROJECT_SOURCE_DIR}/../external/uuid)
include_directories(${PROJECT_SOURCE_DIR}/../common/include)
include_directories(${PROJECT_SOURCE_DIR}/include)

target_link_libraries(episerver pthread sndfile ssl crypto restgoose)

set_target_properties(episerver PROPERTIES RUNTIME_OUTPUT_DIRECTORY "../../bin/")

#install
install(TARGETS episerver RUNTIME DESTINATION /usr/local/bin)

