cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(cartcontroller LANGUAGES CXX C)

#add_custom_command(TARGET controller PRE_BUILD COMMAND ${PROJECT_SOURCE_DIR}/../autoversion ARGS ${PROJECT_SOURCE_DIR}/include/version.h)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

message(${PROJECT_SOURCE_DIR})

add_executable(cartcontroller "../common/src/epiwriter.cpp"
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
include_directories(${PROJECT_SOURCE_DIR}/../common/include)
include_directories(${PROJECT_SOURCE_DIR}/../controller/icons)
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/../controller/include)

list(APPEND flags "-fPIC" "-Wall" "-fpermissive" "-O3" "-pthread")
target_compile_options(cartcontroller PRIVATE ${flags})
target_compile_definitions(cartcontroller PUBLIC NDEBUG WXUSINGDLL WXUSE_UNICODE)

#find external libraries
find_package(wxWidgets REQUIRED)
include(${wxWidgets_USE_FILE})


target_link_libraries(cartcontroller ${wxWidgets_LIBRARIES} pthread cap)

set_target_properties(cartcontroller PROPERTIES RUNTIME_OUTPUT_DIRECTORY "../../bin/")


#install
install(TARGETS cartcontroller RUNTIME DESTINATION /usr/local/bin)

 
