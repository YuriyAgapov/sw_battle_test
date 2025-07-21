file(GLOB_RECURSE VIEWER_SOURCES CONFIGURE_DEPENDS *.cpp *.hpp)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
qt_standard_project_setup()

qt_add_resources(VIEWER_SOURCES resources/resources.qrc)
qt_add_executable(sw_battle_tests_viewer ${VIEWER_SOURCES})

target_link_libraries(sw_battle_tests_viewer PRIVATE Qt6::Widgets sw_battle)

set_target_properties(sw_battle_tests_viewer PROPERTIES
    WIN32_EXECUTABLE ON
)
