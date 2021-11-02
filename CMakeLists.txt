cmake_minimum_required(VERSION 3.0)
project(px-first-login-welcome-screen
        DESCRIPTION "PantherX First Login Welcome Screen Application")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

find_package(CapnProto CONFIG REQUIRED)
set(INTERFACE_DIR src/px-settings-service/interface)
set(CAPNPC_SRC_PREFIX "${INTERFACE_DIR}" CACHE STRING "" FORCE)
set(CAPNPC_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/interface" CACHE STRING "" FORCE)
message(${CAPNPC_OUTPUT_DIR})
file(MAKE_DIRECTORY "${CAPNPC_OUTPUT_DIR}")

capnp_generate_cpp(CapnpSources CapnpHeaders
       ${INTERFACE_DIR}/Settings.capnp)

include_directories(src
                ${CAPNPC_OUTPUT_DIR}
                ${CMAKE_CURRENT_BINARY_DIR})

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
find_package(Qt5Core REQUIRED)
find_package(Qt5DBus REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5LinguistTools REQUIRED)

set(QT5_LIBRARIES Qt5::Core Qt5::Widgets Qt5::DBus)

FILE(GLOB SOURCES
        src/main.cpp 
        src/MainWindow.cpp
        src/px-settings-service/RPCclient.h
        src/px-settings-service/RpcSettingsClient.cpp src/px-settings-service/RpcSettingsClient.h
        src/px-settings-service/SettingsObjects.cpp src/px-settings-service/SettingsObjects.h
        )

add_executable(px-first-login-welcome-screen ${SOURCES} ${CapnpSources} ${CapnpHeaders})
target_link_libraries(px-first-login-welcome-screen ${QT5_LIBRARIES} CapnProto::capnp-rpc)
install(TARGETS px-first-login-welcome-screen DESTINATION lib)
