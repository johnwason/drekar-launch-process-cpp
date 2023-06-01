cmake_minimum_required(VERSION 3.3)
project(simple_launch_process_cpp)

set (CMAKE_CXX_STANDARD 11)

if (WIN32)
    add_definitions(-D_WIN32_WINNT=0x0601)
    set(SRCS src/simple_launch_process_cpp_win32.cpp)
else()
    set(SRCS src/simple_launch_process_cpp_posix.cpp)
endif()

add_library(${PROJECT_NAME} ${SRCS})
target_include_directories(${PROJECT_NAME} PUBLIC include)

if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

include(GNUInstallDirs)

install(TARGETS ${PROJECT_NAME}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

# include(CTest)
if (BUILD_TESTING)

    add_executable(${PROJECT_NAME}_wait_exit_test test/test_simple_launch_process_cpp_wait_exit.cpp)
    target_link_libraries(${PROJECT_NAME}_wait_exit_test ${PROJECT_NAME})

    add_executable(${PROJECT_NAME}_callback_exit_test test/test_simple_launch_process_cpp_callback_exit.cpp)
    target_link_libraries(${PROJECT_NAME}_callback_exit_test ${PROJECT_NAME})
endif()