cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(player3 LANGUAGES CXX C)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

message(${PROJECT_SOURCE_DIR})

add_executable(player3 "../common/src/epicron.cpp"
					   "../common/src/inimanager.cpp"
						"../common/src/inisection.cpp"
						"../common/src/utils.cpp"
						"../external/jsoncpp/dist/jsoncpp.cpp"
						"src/main.cpp"
						"src/audiofile.cpp"
						"src/epiwriter.cpp"
						"src/filesource.cpp"
						"src/mp3file.cpp"
						"src/playlist.cpp"
						"src/playout.cpp"
						"src/resampler.cpp"
						"src/resources.cpp"
						"src/schedule.cpp"
						"src/soundfile.cpp")

include_directories(${PROJECT_SOURCE_DIR}/../external/jsoncpp/dist)
include_directories(${PROJECT_SOURCE_DIR}/../external/log/include)
include_directories(${PROJECT_SOURCE_DIR}/../common/include)
include_directories(${PROJECT_SOURCE_DIR}/include)

list(APPEND flags "-fPIC" "-Wall" "-fpermissive" "-O3" "-pthread")
target_compile_options(player3 PRIVATE ${flags})
target_compile_definitions(player3 PUBLIC NDEBUG)




#log
find_path(LOG_LIB_DIR NAMES "include/log.h" PATHS ${PROJECT_SOURCE_DIR}/../external/log)
include_directories(${LOG_LIB_DIR}/include)

#use pkgconfig to find the required libs
find_package(PkgConfig REQUIRED QUIET)
pkg_search_module(samplerate REQUIRED samplerate IMPORTED_TARGET)
if(TARGET PkgConfig::samplerate)
	message(STATUS "Found libsamplerate")
	target_link_libraries(player3 PkgConfig::samplerate)
endif()
	
pkg_search_module(portaudio REQUIRED portaudio-2.0 IMPORTED_TARGET)
if(TARGET PkgConfig::portaudio)
	message(STATUS "Found portaudio")
	target_link_libraries(player3 PkgConfig::portaudio)
endif()

pkg_search_module(sndfile REQUIRED sndfile IMPORTED_TARGET)
if(TARGET PkgConfig::sndfile)
	message(STATUS "Found sndfile")
	target_link_libraries(player3 PkgConfig::sndfile)
endif()

target_link_libraries(player3 pthread pml_log sndfile portaudio samplerate)

set_target_properties(player3 PROPERTIES RUNTIME_OUTPUT_DIRECTORY "../bin/")

#install
install(TARGETS player3 RUNTIME DESTINATION /usr/local/bin)

