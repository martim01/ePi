cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(controller LANGUAGES CXX C)

#add_custom_command(TARGET controller PRE_BUILD COMMAND ${PROJECT_SOURCE_DIR}/../autoversion ARGS ${PROJECT_SOURCE_DIR}/include/version.h)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

message(${PROJECT_SOURCE_DIR})

add_executable(controller "../common/src/epiwriter.cpp"
 "../common/src/inimanager.cpp"
 "../common/src/inisection.cpp"
 "../common/src/jsonutils.cpp"
 "../common/src/mongoose.c"
 "../external/jsoncpp/dist/jsoncpp.cpp"
 "../external/log/src/log.cpp"
 "src/constants.cpp"
 "src/dlgError.cpp"
 "src/version.cpp"
 "src/controllerApp.cpp"
 "src/controllerMain.cpp"
 "src/dlgEdit.cpp"
 "src/dlgOptions.cpp"
 "src/dlgUpload.cpp"
 "src/dlgUsb.cpp"
 "src/multipartupload.cpp"
 "src/progress.cpp"
 "src/restfulclient.cpp"
 "src/uirect.cpp"
 "src/usbchecker.cpp"
 "src/websocketclient.cpp"
 "src/wmbutton.cpp"
 "src/wmedit.cpp"
 "src/wmfocusmanager.cpp"
 "src/wmkeyboard.cpp"
 "src/wmlabel.cpp"
 "src/wmlist.cpp")

include_directories(${PROJECT_SOURCE_DIR}/../external/jsoncpp/dist)
include_directories(${PROJECT_SOURCE_DIR}/../external/log/include)
include_directories(${PROJECT_SOURCE_DIR}/../common/include)
include_directories(${PROJECT_SOURCE_DIR}/icons)
include_directories(${PROJECT_SOURCE_DIR}/include)

list(APPEND flags "-fPIC" "-Wall" "-fpermissive" "-O3" "-pthread")
target_compile_options(controller PRIVATE ${flags})
target_compile_definitions(controller PUBLIC NDEBUG WXUSINGDLL WXUSE_UNICODE)

#find external libraries
find_package(wxWidgets REQUIRED)
include(${wxWidgets_USE_FILE})


target_link_libraries(controller ${wxWidgets_LIBRARIES} pthread cap)

set_target_properties(controller PROPERTIES RUNTIME_OUTPUT_DIRECTORY "../../bin/")


#install
install(TARGETS controller RUNTIME DESTINATION /usr/local/bin)

 
