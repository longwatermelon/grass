cmake_minimum_required(VERSION 3.19.2)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(grass)

add_executable(grass
    src/main.cpp
    src/grass.h
    src/grass.cpp
    src/plugin_manager.h
    src/plugin_manager.cpp
)

add_subdirectory(src/gui)
add_subdirectory(src/plugin)

target_link_libraries(grass PRIVATE gui plugin)

if (MSVC)
    set_property(TARGET grass PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif()

install(TARGETS grass DESTINATION bin)

# hacky solution but it works
if (WIN32)
    install(DIRECTORY ${CMAKE_BINARY_DIR}/src/grass/Release/ DESTINATION bin FILES_MATCHING PATTERN "*.dll")
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/res/ DESTINATION bin/res)
else()
    file(MAKE_DIRECTORY /usr/share/grass)
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/res/ DESTINATION /usr/share/grass/res)
endif()

include(InstallRequiredSystemLibraries)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "3")
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}" ${CMAKE_MODULE_PATH})
set(CPACK_START_MENU_SHORTCUTS "${PROJECT_NAME}")
set(CPACK_PACKAGE_EXECUTABLES "${PROJECT_NAME}" "${PROJECT_NAME}.exe")
include(CPack)

