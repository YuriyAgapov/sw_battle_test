file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS *.cpp *.hpp)

add_executable(sw_battle_tests ${TEST_SOURCES})
target_include_directories(sw_battle_tests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(sw_battle_tests
  GTest::gtest_main
  sw_battle
)

include(GoogleTest)
gtest_discover_tests(sw_battle_tests)