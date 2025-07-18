file(GLOB_RECURSE CLIENT_SOURCES CONFIGURE_DEPENDS *.cpp *.hpp)

add_executable(sw_battle_tests_client ${CLIENT_SOURCES})
target_link_libraries(sw_battle_tests_client
  sw_battle
)
