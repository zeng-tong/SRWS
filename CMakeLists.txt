cmake_minimum_required(VERSION 3.5)
project(Simple_Reactor_Web_Server)
set(CMAKE_CXX_STANDARD 11)
include_directories("include")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -pthread -g -O0 -fprofile-arcs -ftest-coverage")
set(TCP_SERVER_FILE "src/tcpServer.cpp")
set(TCP_SERVER_TEST_FILE "test/tcpServerTest.cpp")
set(PROCESS_POOL_TEST_FILE "test/processPoolTest.cpp")

add_executable(tcp_server ${TCP_SERVER_FILE} "src/tcpServerMain.cpp")

add_executable(tstest ${TCP_SERVER_TEST_FILE} ${TCP_SERVER_FILE} "test/testRunner.cpp")

add_executable(ppt ${TCP_SERVER_FILE} ${PROCESS_POOL_TEST_FILE} "test/testRunner.cpp")

add_executable(main ${TCP_SERVER_FILE} "src/main.cpp")

enable_testing()
add_test(NAME "tcp_server_test" COMMAND "tstest")
add_test(NAME "process_pool_test" COMMAND "ppt")
