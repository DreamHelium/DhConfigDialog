find_package(ECM REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})
find_package(KF6 REQUIRED COMPONENTS ConfigWidgets I18n WidgetsAddons)

set(CMAKE_AUTOMOC ON)

qt_add_library(dhconfigdialog SHARED
            dhconfigdialog.cpp dhconfigdialog.h
        dhconfigtemplates.cpp
        dhconfigtemplates.h)

target_link_libraries(dhconfigdialog PUBLIC KF6::ConfigWidgets KF6::I18n KF6::WidgetsAddons)
