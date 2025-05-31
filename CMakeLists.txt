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
    truthtablebuilder.h truthtablebuilder.cpp
    CustomLineEdit/customlineedit.h CustomLineEdit/customlineedit.cpp
    res.qrc

)

#qt_add_translations(
#    TARGETS cursova
#    TS_FILES cursova_en_GB.ts
#)

target_link_libraries(cursova
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt6::Concurrent
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
