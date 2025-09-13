cmake_minimum_required(VERSION 3.5)

project(KD VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE Debug)

list(APPEND CMAKE_PREFIX_PATH "/home/odal/Qt/6.7.0/gcc_64")
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets) 
find_package(magic_enum CONFIG REQUIRED)

include(FetchContent)
include(FindProtobuf)
find_package(Protobuf REQUIRED)
FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
)
FetchContent_MakeAvailable(fmt)

find_package(Protobuf REQUIRED)
set(PROTO_FILE ${CMAKE_CURRENT_SOURCE_DIR}/message.proto)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_FILE})

add_library(proto_lib ${PROTO_SRCS} ${PROTO_HDRS})
target_include_directories(proto_lib PUBLIC ${Protobuf_INCLUDE_DIRS}
                                            ${CMAKE_CURRENT_BINARY_DIR} 
                                            ${CMAKE_CURRENT_SOURCE_DIR} )
target_link_libraries(proto_lib PUBLIC protobuf::libprotobuf)

set(PROJECT_SOURCES
        main.cpp
        MainWin.cpp
        MainWin.h
        MainWin.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(KD
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        swarm.h          swarm.cpp
        peer.h           peer.cpp
        node.h           node.cpp
        lookupcontext.h  lookupcontext.cpp
        messageHandler.h messageHandler.cpp
        utils.h          utils.cpp
        message.pb.h     message.pb.cc
        constants.h
        inode.h 
        ipeer.h
        messageBuilder.h
        lookupStats.h
    )
    add_compile_definitions(DEBUG)
endif()

target_link_libraries(KD PRIVATE Qt${QT_VERSION_MAJOR}::Widgets fmt::fmt proto_lib magic_enum::magic_enum)

include(GNUInstallDirs)
install(TARGETS KD
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(KD)
endif()
