cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(cartcontroller LANGUAGES CXX C)

#add_custom_command(TARGET controller PRE_BUILD COMMAND ${PROJECT_SOURCE_DIR}/../autoversion ARGS ${PROJECT_SOURCE_DIR}/include/version.h)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

message(${PROJECT_SOURCE_DIR})

add_executable(cartcontroller "../common/src/epiwriter.cpp"
 "../common/src/inimanager.cpp"
 "../common/src/inisection.cpp"
 "../common/src/jsonutils.cpp"
 "../common/src/mongoose.c"
 "../external/jsoncpp/dist/jsoncpp.cpp"
 "../external/log/src/log.cpp"
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
 "../controller/src/multipartupload.cpp"
 "../controller/src/progress.cpp"
 "../controller/src/restfulclient.cpp"
 "../controller/src/uirect.cpp"
 "../controller/src/usbchecker.cpp"
 "../controller/src/websocketclient.cpp"
 "../controller/src/wmbutton.cpp"
 "../controller/src/wmedit.cpp"
 "../controller/src/wmfocusmanager.cpp"
 "../controller/src/wmkeyboard.cpp"
 "../controller/src/wmlabel.cpp"
 "../controller/src/wmlist.cpp")

include_directories(${PROJECT_SOURCE_DIR}/../external/jsoncpp/dist)
include_directories(${PROJECT_SOURCE_DIR}/../external/log/include)
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

 
