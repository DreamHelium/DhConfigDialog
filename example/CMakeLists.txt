cmake_minimum_required(VERSION 3.17)

project(example_dhcd LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)

find_package(ECM REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(KF6 REQUIRED COMPONENTS ConfigWidgets)

qt_add_executable(example_dhcd main.cpp)

target_link_libraries(example_dhcd PRIVATE
  Qt6::Widgets
  KF6::ConfigWidgets
  dhconfigdialog
)
target_include_directories(example_dhcd PRIVATE
 ${CMAKE_CURRENT_BINARY_DIR}
)
kconfig_add_kcfg_files(example_dhcd config/settings.kcfgc)

qt_finalize_executable(example_dhcd)
