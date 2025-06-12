cmake_minimum_required(VERSION 3.19)
project(cursova LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets LinguistTools Concurrent AxContainer Test)
qt_standard_project_setup()

# 1) Бібліотека з основної логіки без main.cpp
qt_add_library(cursova_core STATIC
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    CustomLineEdit/customlineedit.h
    CustomLineEdit/customlineedit.cpp
    res.qrc
    IniManager/inimanager.h
    IniManager/inimanager.cpp
    IniManager/SettingsManager/settingsmanager.h
    IniManager/SettingsManager/settingsmanager.cpp
    IniManager/HistoryManager/historymanager.h
    IniManager/HistoryManager/historymanager.cpp
    form.ui
    TruthTableTab/truthtabletab.h
    TruthTableTab/truthtabletab.cpp
    TruthTableTab/truthtabletab.ui
    TruthTableBuilder/truthtablebuilder.h
    TruthTableBuilder/truthtablebuilder.cpp
    FileManager/filemanager.h
    FileManager/filemanager.cpp
    ExcelExporter/excelexporter.h
    ExcelExporter/excelexporter.cpp
)

target_link_libraries(cursova_core
    PUBLIC
        Qt::Core
        Qt::Widgets
        Qt6::AxContainer
)


# 2) Виконуване додаток
qt_add_executable(cursova
    WIN32 MACOSX_BUNDLE
    app.rc
    main.cpp
)
target_link_libraries(cursova
    PRIVATE
        cursova_core
)
qt_add_translations(cursova
    TS_FILES
    cursova_en_GB.ts
    QM_FILES_OUTPUT_VARIABLE qm_files
)

# Install .qm files to translations directory
install(FILES ${qm_files} DESTINATION ${CMAKE_INSTALL_BINDIR}/translations)

# 3) Тести в окремому виконуваному
qt_add_executable(validate_tests
    tests/test_validateexpression.cpp
)
target_link_libraries(validate_tests
    PRIVATE
        cursova_core
        Qt6::Test
)

# 4) Налагодження встановлення і тестування
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

enable_testing()

# Додаємо тест із вказанням шляху до бінаря та конфігурації
add_test(NAME ValidateExpressionTests
         COMMAND $<TARGET_FILE:validate_tests>)
# Для multi-config генераторів (MSVC) вкажемо, що тест в Debug конфігурації
set_tests_properties(ValidateExpressionTests PROPERTIES VS_CONFIGURATION Debug)

# Альтернатива запуску без вказання конфігурації:
# Використовуйте у консолі:
#   ctest --verbose -C Debug
