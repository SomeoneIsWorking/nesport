cmake_minimum_required(VERSION 3.24)

project(nesport LANGUAGES C CXX)

include(cmake/ResolveMesenCE.cmake)

add_library(nesport
	src/machine.cpp
	src/native_overrides.cpp
)
target_compile_features(nesport PUBLIC cxx_std_17)
target_include_directories(nesport PUBLIC include)
target_link_libraries(nesport PUBLIC mesence_core)

option(NESPORT_BUILD_TESTS "Build nesport tests" ON)
if(NESPORT_BUILD_TESTS)
	enable_testing()
	add_executable(nesport_runtime_test tests/runtime_test.cpp)
	target_link_libraries(nesport_runtime_test PRIVATE nesport)
	add_test(NAME nesport_runtime_test COMMAND nesport_runtime_test)
endif()
