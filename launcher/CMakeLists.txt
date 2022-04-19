cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(launcher LANGUAGES CXX C)

#add_custom_command(TARGET launcher PRE_BUILD COMMAND ${PROJECT_SOURCE_DIR}/../autoversion ARGS ${PROJECT_SOURCE_DIR}/include/version.h)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

message(${PROJECT_SOURCE_DIR})

add_executable(launcher "../common/src/inimanager.cpp"
			"../common/src/inisection.cpp"
                        "src/main.cpp"
                        "../common/src/epiutils.cpp" 
                        "src/launcher.cpp")

include_directories(${DIR_LOG}/include)
include_directories(${PROJECT_SOURCE_DIR}/../common/include)
include_directories(${PROJECT_SOURCE_DIR}/include)

list(APPEND flags "-fPIC" "-Wall" "-fpermissive" "-O3" "-pthread")
target_compile_options(launcher PRIVATE ${flags})
target_compile_definitions(launcher PUBLIC NDEBUG)



target_link_libraries(launcher pthread cap pml_log)

set_target_properties(launcher PROPERTIES RUNTIME_OUTPUT_DIRECTORY "../../bin/")


#install
install(TARGETS launcher RUNTIME DESTINATION /usr/local/bin)
install(CODE "execute_process(COMMAND setcap cap_sys_admin+ep /usr/local/bin/launcher)")
