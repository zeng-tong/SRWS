cmake_minimum_required(VERSION 3.10)
project(Simple_Reactor_Web_Server)

include_directories("include")

set(CMAKE_CXX_STANDARD 11)

set(TCP_SERVER_FILE "src/tcpServer.cpp")
set(TCP_SERVER_TEST_FILE "test/tcpServerTest.cpp")

add_executable(tcp_server ${TCP_SERVER_FILE} "src/tcpServerMain.cpp")

add_executable(tstest ${TCP_SERVER_TEST_FILE} "test/testRunner.cpp")

enable_testing()
add_test(NAME "tcp_server_test" COMMAND "tstest")