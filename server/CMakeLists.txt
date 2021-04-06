cmake_minimum_required(VERSION 3.19)

project(Boost_Demo)

set(Boost_ROOT D:/SDK/boost_1_75_0)

set(Boost_USE_STATIC_LIBS ON)

find_package(Boost REQUIRED COMPONENTS date_time regex thread log)

add_definitions(-D_WIN32_WINNT=0x0A00)

include_directories(${Boost_INCLUDE_DIRS})

add_executable(server server.cpp tcp_session.cpp)

target_link_libraries(server Boost::date_time Boost::regex Boost::thread Boost::log)

add_executable(client client.cpp)

target_link_libraries(client Boost::date_time Boost::regex)