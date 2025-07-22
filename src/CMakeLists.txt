file(GLOB_RECURSE SRC_SOURCES CONFIGURE_DEPENDS *.cpp *.hpp)

add_library(sw_battle STATIC ${SRC_SOURCES})
target_include_directories(sw_battle PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
