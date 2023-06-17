cmake_minimum_required(VERSION 3.3)

FILE (STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/package.xml" lib_version1 REGEX "<version>[0-9]+\\.[0-9]+\\.[0-9]+</version>")
IF ("${lib_version1}" STREQUAL "" )
MESSAGE(FATAL_ERROR "Could not read drekar-launch-process-cpp version.")
ENDIF()
STRING (REGEX REPLACE "<version>([0-9]+\\.[0-9]+\\.[0-9]+)</version>" "\\1" lib_version2 ${lib_version1})
STRING (STRIP ${lib_version2} lib_version)
MESSAGE(STATUS "drekar-launch-process-cpp version: ${lib_version}")

project(drekar-launch-process-cpp VERSION ${lib_version} LANGUAGES CXX)

set (CMAKE_CXX_STANDARD 11)

option(BUILD_SHARED_LIBS "Build shared libraries" ON)

if (WIN32)
    add_definitions(-D_WIN32_WINNT=0x0601)
    set(SRCS src/drekar_launch_process_cpp_win32.cpp)
else()
    set(SRCS src/drekar_launch_process_cpp_posix.cpp)
endif()

add_library(${PROJECT_NAME} ${SRCS})
target_include_directories(${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                                            $<INSTALL_INTERFACE:include>)

if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

include(GNUInstallDirs)

install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}-targets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        )
        
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

include(CMakePackageConfigHelpers)
        
set(CONFIG_EXPORT_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})
install(EXPORT ${PROJECT_NAME}-targets
        DESTINATION "${CONFIG_EXPORT_DIR}")

configure_package_config_file(
  "${PROJECT_SOURCE_DIR}/cmake/drekar-launch-process-cpp-config.cmake.in"
  "${PROJECT_BINARY_DIR}/drekar-launch-process-cpp-config.cmake"
  INSTALL_DESTINATION "${CONFIG_EXPORT_DIR}"
  PATH_VARS CMAKE_INSTALL_INCLUDEDIR CMAKE_INSTALL_LIBDIR CONFIG_EXPORT_DIR BUILD_SHARED_LIBS)
unset(EXPORT_TARGETS)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-version.cmake"
  VERSION "${PROJECT_VERSION}"
  COMPATIBILITY AnyNewerVersion
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-version.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

# include(CTest)
if (BUILD_TESTING)

    add_executable(${PROJECT_NAME}_wait_exit_test test/test_drekar_launch_process_cpp_wait_exit.cpp)
    target_link_libraries(${PROJECT_NAME}_wait_exit_test ${PROJECT_NAME})

    add_executable(${PROJECT_NAME}_callback_exit_test test/test_drekar_launch_process_cpp_callback_exit.cpp)
    target_link_libraries(${PROJECT_NAME}_callback_exit_test ${PROJECT_NAME})
endif()