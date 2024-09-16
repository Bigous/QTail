cmake_minimum_required(VERSION 3.16)

project(QTail VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools Concurrent)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools Concurrent)

set(TS_FILES QTail_pt_BR.ts)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        ${TS_FILES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(QTail
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}

        tailfilewidget.h tailfilewidget.cpp
        filemonitor.h filemonitor.cpp
        highlighter.h highlighter.cpp
        highlightedtextedit.h highlightedtextedit.cpp
    )
    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(QTail SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(QTail
            ${PROJECT_SOURCES}
        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_link_libraries(QTail PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Concurrent)

set_target_properties(QTail PROPERTIES
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS QTail
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Adicionar os diretórios assets/fonts e assets/imgs
set(ASSETS_DIR "${CMAKE_SOURCE_DIR}/assets")
set(FONTS_DIR "${ASSETS_DIR}/fonts")
set(IMGS_DIR "${ASSETS_DIR}/imgs")
set(LICENSES_DIR "${ASSETS_DIR}/licenses")

# Certifique-se de que o diretório de destino dos assets seja criado
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/assets)

# Copiar as pastas assets para o diretório de build
add_custom_command(TARGET QTail POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${ASSETS_DIR} $<TARGET_FILE_DIR:QTail>/assets
)


if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(QTail)
endif()
