cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(cartcontroller LANGUAGES CXX C)

execute_process(COMMAND ${CMAKE_COMMAND} -DNAMESPACE=epi -P ${CMAKE_CURRENT_SOURCE_DIR}/version.cmake)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

message(${PROJECT_SOURCE_DIR})

add_executable(controller "../common/src/epiwriter.cpp"
 "../common/src/inimanager.cpp"
 "../common/src/inisection.cpp"
 "../common/src/jsonutils.cpp"
 "../controller/src/constants.cpp"
 "../controller/src/dlgError.cpp"
 "../controller/src/dlgInfo.cpp"
 "src/version.cpp"
 "src/cartcontrollerApp.cpp"
 "src/cartcontrollerMain.cpp"
 "src/pnlResource.cpp"
 "../controller/src/dlgEdit.cpp"
 "../controller/src/dlgOptions.cpp"
 "../controller/src/dlgUpload.cpp"
 "../controller/src/dlgUsb.cpp"
 "../controller/src/progress.cpp"
 "../controller/src/uirect.cpp"
 "../controller/src/usbchecker.cpp"
 "../controller/src/wmbutton.cpp"
 "../controller/src/wmedit.cpp"
 "../controller/src/wmfocusmanager.cpp"
 "../controller/src/wmkeyboard.cpp"
 "../controller/src/wmlabel.cpp"
 "../controller/src/wmlist.cpp"
 "../controller/src/wxhttpclientmanager.cpp"
 "../controller/src/wxwebsocketclient.cpp")

 include_directories(${DIR_JSON}/dist)
 include_directories(${DIR_LOG}/include)
 include_directories(${DIR_RESTGOOSE}/include)
include_directories(${PROJECT_SOURCE_DIR}/../common/include)
include_directories(${PROJECT_SOURCE_DIR}/icons)
include_directories(${PROJECT_SOURCE_DIR}/include)

list(APPEND flags "-fPIC" "-Wall" "-fpermissive" "-O3" "-pthread")
target_compile_options(cartcontroller PRIVATE ${flags})
target_compile_definitions(cartcontroller PUBLIC NDEBUG WXUSINGDLL WXUSE_UNICODE)

#find external libraries
find_package(wxWidgets REQUIRED)
include(${wxWidgets_USE_FILE})


target_link_libraries(cartcontroller ${wxWidgets_LIBRARIES} pthread cap restgoose pml_log)

set_target_properties(cartcontroller PROPERTIES RUNTIME_OUTPUT_DIRECTORY "../../bin/")


#install
install(TARGETS cartcontroller RUNTIME DESTINATION /usr/local/bin)

 
