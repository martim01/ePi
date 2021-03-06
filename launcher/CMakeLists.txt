cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(launcher LANGUAGES CXX C)

#add_custom_command(TARGET launcher PRE_BUILD COMMAND ${PROJECT_SOURCE_DIR}/../autoversion ARGS ${PROJECT_SOURCE_DIR}/include/version.h)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

message(${PROJECT_SOURCE_DIR})

add_executable(launcher "../common/src/inimanager.cpp"
			"../common/src/inisection.cpp"
                        "../external/log/src/log.cpp"
			"src/main.cpp"
                        "../common/src/utils.cpp" 
                        "src/launcher.cpp")

include_directories(${PROJECT_SOURCE_DIR}/../external/log/include)
include_directories(${PROJECT_SOURCE_DIR}/../common/include)
include_directories(${PROJECT_SOURCE_DIR}/include)

list(APPEND flags "-fPIC" "-Wall" "-fpermissive" "-O3" "-pthread")
target_compile_options(launcher PRIVATE ${flags})
target_compile_definitions(launcher PUBLIC NDEBUG)



target_link_libraries(launcher pthread cap)

set_target_properties(launcher PROPERTIES RUNTIME_OUTPUT_DIRECTORY "../../bin/")

add_custom_command(TARGET launcher POST_BUILD COMMAND "setcap" ARGS " cap_sys_admin+ep" "../../bin/launcher")


#install
install(TARGETS launcher RUNTIME DESTINATION /usr/local/bin)

