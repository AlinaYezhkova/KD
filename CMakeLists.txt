cmake_minimum_required(VERSION 3.5)

project(KD VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

include(FetchContent)

FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
)
FetchContent_MakeAvailable(fmt)

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

        node.h          node.cpp
        packetcounter.h packetcounter.cpp
        swarm.h         swarm.cpp

        test.h          test.cpp
        kademlia.h      kademlia.cpp
        binbucket.h     binbucket.cpp
        utils.h         utils.cpp
        constants.h
        inode.h
        pool.h
    )
endif()
add_compile_definitions(DEBUG)

target_link_libraries(KD PRIVATE Qt${QT_VERSION_MAJOR}::Widgets fmt::fmt)

include(GNUInstallDirs)
install(TARGETS KD
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(KD)
endif()
