cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(controller LANGUAGES CXX C VERSION 2.0.0)

execute_process(COMMAND ${CMAKE_COMMAND} -DNAMESPACE=epi -DMAJOR=${PROJECT_VERSION_MAJOR} -DMINOR=${PROJECT_VERSION_MINOR} -DPATCH=${PROJECT_VERSION_PATCH} -P ${CMAKE_CURRENT_SOURCE_DIR}/version.cmake)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

message(${PROJECT_SOURCE_DIR})

add_executable(controller "../common/src/epiwriter.cpp"
 "../common/src/inimanager.cpp"
 "../common/src/inisection.cpp"
 "../common/src/jsonutils.cpp"
 "src/constants.cpp"
 "src/controllerApp.cpp"
 "src/controllerMain.cpp"
 "src/dlgEdit.cpp"
 "src/dlgError.cpp"
 "src/dlgInfo.cpp"
 "src/dlgOptions.cpp"
 "src/dlgUpload.cpp"
 "src/dlgUsb.cpp"
 "src/progress.cpp"
 "src/uirect.cpp"
 "src/usbchecker.cpp"
 "src/version.cpp"
 "src/wmbutton.cpp"
 "src/wmedit.cpp"
 "src/wmfocusmanager.cpp"
 "src/wmkeyboard.cpp"
 "src/wmlabel.cpp"
 "src/wmlist.cpp"
 "src/wxhttpclientmanager.cpp"
 "src/wxwebsocketclient.cpp")

include_directories(${DIR_JSON}/dist)
include_directories(${DIR_LOG}/include)
include_directories(${DIR_RESTGOOSE}/include)
include_directories(${PROJECT_SOURCE_DIR}/../common/include)
include_directories(${PROJECT_SOURCE_DIR}/../controller/icons)
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/../controller/include)

list(APPEND flags "-fPIC" "-Wall" "-fpermissive" "-O3" "-pthread")
target_compile_options(controller PRIVATE ${flags})
target_compile_definitions(controller PUBLIC NDEBUG WXUSINGDLL WXUSE_UNICODE)

#find external libraries
find_package(wxWidgets REQUIRED)
include(${wxWidgets_USE_FILE})


target_link_libraries(controller ${wxWidgets_LIBRARIES} pthread cap restgoose pml_log)

set_target_properties(controller PROPERTIES RUNTIME_OUTPUT_DIRECTORY "../../bin/")


#install
install(TARGETS controller RUNTIME DESTINATION /usr/local/bin)

 
