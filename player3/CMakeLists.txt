cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(player3 LANGUAGES CXX C VERSION 1.1.0)


set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

execute_process(COMMAND ${CMAKE_COMMAND} -DNAMESPACE=epi -DMAJOR=${PROJECT_VERSION_MAJOR} -DMINOR=${PROJECT_VERSION_MINOR} -DPATCH=${PROJECT_VERSION_PATCH} -P ${CMAKE_CURRENT_SOURCE_DIR}/version.cmake)

add_executable(player3 "../common/src/epicron.cpp"
					   "../common/src/inimanager.cpp"
						"../common/src/inisection.cpp"
						"../common/src/epiutils.cpp"
                        "../common/src/jsonutils.cpp"
						"../common/src/logtofile.cpp"
						${DIR_JSON}/dist/jsoncpp.cpp
						"src/main.cpp"
						"src/audiofile.cpp"
						"../common/src/epiwriter.cpp"
						"src/filesource.cpp"
						"src/mp3file.cpp"
						"src/playlist.cpp"
						"src/playout.cpp"
						"src/resampler.cpp"
						"src/resources.cpp"
						"src/schedule.cpp"
						"src/soundfile.cpp"
                        "src/version.cpp")

include_directories(${DIR_JSON}/dist)
include_directories(${DIR_LOG}/include)
						
include_directories(${PROJECT_SOURCE_DIR}/../common/include)
include_directories(${PROJECT_SOURCE_DIR}/include)

list(APPEND flags "-fPIC" "-Wall" "-fpermissive" "-O3" "-pthread" )
target_compile_options(player3 PRIVATE ${flags})
target_compile_definitions(player3 PUBLIC NDEBUG)


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

target_link_libraries(player3 pthread sndfile portaudio samplerate pml_log)

set_target_properties(player3 PROPERTIES RUNTIME_OUTPUT_DIRECTORY "../../bin/")

#install
install(TARGETS player3 RUNTIME DESTINATION /usr/local/bin)

