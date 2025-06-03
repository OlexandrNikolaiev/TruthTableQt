cmake_minimum_required(VERSION 3.19)
project(cursova LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets LinguistTools Concurrent)

qt_standard_project_setup()

qt_add_executable(cursova
    WIN32 MACOSX_BUNDLE
    app.rc
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui

    CustomLineEdit/customlineedit.h CustomLineEdit/customlineedit.cpp
    res.qrc
    IniManager/inimanager.h IniManager/inimanager.cpp
    IniManager/SettingsManager/settingsmanager.h IniManager/SettingsManager/settingsmanager.cpp
    IniManager/HistoryManager/historymanager.h IniManager/HistoryManager/historymanager.cpp
    form.ui

    TruthTableTab/truthtabletab.h TruthTableTab/truthtabletab.cpp TruthTableTab/truthtabletab.ui

    TruthTableBuilder/truthtablebuilder.h TruthTableBuilder/truthtablebuilder.cpp

)

#qt_add_translations(
#    TARGETS cursova
#    TS_FILES cursova_en_GB.ts
#)

target_link_libraries(cursova
    PRIVATE
        Qt::Core
        Qt::Widgets
)

include(GNUInstallDirs)

install(TARGETS cursova
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET cursova
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
