cmake_minimum_required(VERSION 3.19)

project(Boost_Demo)

set(Boost_ROOT D:/SDK/boost_1_75_0)

set(Boost_USE_STATIC_LIBS ON)

find_package(Boost REQUIRED COMPONENTS thread regex date_time)

add_definitions(-D_WIN32_WINNT=0x0A00)

include_directories(${Boost_INCLUDE_DIRS})

add_executable(server server.cpp tcp_session.cpp chat_server.cpp)

target_link_libraries(server Boost::thread Boost::regex)

add_executable(client client.cpp)

target_link_libraries(client Boost::date_time Boost::regex Boost::thread)