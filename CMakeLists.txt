cmake_minimum_required(VERSION 3.16)

project(QTail VERSION 0.1 LANGUAGES CXX)

# Informações sobre o autor
set(PROJECT_AUTHOR "Richard Natal")

# Se estiver compilando em Debug
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Building in Debug mode")
    add_definitions(-DQT_DEBUG)
    # Adiciona o diretório de QCoro ao CMAKE_MODULE_PATH
    set(QCoro6_DIR "R:/Richard/cmake/debug/qcoro/lib/cmake/QCoro6")
    set(QCoro6Core_DIR "R:/Richard/cmake/debug/qcoro/lib/cmake/QCoro6Core")
    set(QCoro6Coro_DIR "R:/Richard/cmake/debug/qcoro/lib/cmake/QCoro6Coro")
else()
    message(STATUS "Building in Release mode")
    # Adiciona o diretório de QCoro ao CMAKE_MODULE_PATH
    set(QCoro6_DIR "R:/Richard/cmake/release/qcoro/lib/cmake/QCoro6")
    set(QCoro6Core_DIR "R:/Richard/cmake/release/qcoro/lib/cmake/QCoro6Core")
    set(QCoro6Coro_DIR "R:/Richard/cmake/release/qcoro/lib/cmake/QCoro6Coro")
endif()

list(APPEND CMAKE_PREFIX_PATH ${QCoro6_DIR})
list(APPEND CMAKE_PREFIX_PATH ${QCoro6Core_DIR})
list(APPEND CMAKE_PREFIX_PATH ${QCoro6Coro_DIR})

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets LinguistTools Concurrent)
find_package(Qt6 REQUIRED COMPONENTS Widgets LinguistTools Concurrent)
find_package(QCoro6 REQUIRED COMPONENTS Core)

set(TS_FILES QTail_pt_BR.ts)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        ${TS_FILES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    if(WIN32)
        qt_add_executable(QTail
            MANUAL_FINALIZATION
            ${PROJECT_SOURCES}

            tailfilewidget.h tailfilewidget.cpp



            version.rc
            LogFileModel.hpp
            LogListView.hpp
            FixDictionary.hpp

        )
    else()
        qt_add_executable(QTail
            MANUAL_FINALIZATION
            ${PROJECT_SOURCES}

            tailfilewidget.h tailfilewidget.cpp
            filemonitor.h filemonitor.cpp
            highlighter.h highlighter.cpp
            highlightedtextedit.h highlightedtextedit.cpp
        )
    endif()
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

target_link_libraries(QTail PRIVATE Qt${QT_VERSION_MAJOR}::Widgets QCoro::Core)

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

# Define o nome do arquivo zip
set(OUTPUT_ZIP "${CMAKE_BINARY_DIR}/QTail-Release.zip")

# Comando para empacotar arquivos no Windows (usando windeployqt e PowerShell)
if(WIN32)
    set(QT_DEPLOY_TOOL "R:/Richard/tools/Qt/6.7.2/msvc2019_64/bin/windeployqt6.exe")
    set(RELEASE_DIR "$<TARGET_FILE_DIR:QTail>/release")
    set(RELEASE_ADD_DIR "$<TARGET_FILE_DIR:QTail>/release/*")
    add_custom_command(TARGET QTail POST_BUILD
        # Remove o diretório de release anterior
        COMMAND ${CMAKE_COMMAND} -E echo "Removing old release directory..."
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${RELEASE_DIR}

        # Ajuste o windeployqt para incluir apenas o necessário
        COMMAND ${CMAKE_COMMAND} -E echo "Running windeployqt..."
        COMMAND ${QT_DEPLOY_TOOL} --release --no-opengl --translations pt_BR --include-plugins qico $<TARGET_FILE:QTail> --dir ${RELEASE_DIR}

        COMMAND ${CMAKE_COMMAND} -E echo "Packaging release files for Windows..."
        COMMAND powershell -Command "Compress-Archive -Force -Path $<TARGET_FILE:QTail>, ${RELEASE_ADD_DIR}, ${ASSETS_DIR} -DestinationPath ${OUTPUT_ZIP}"
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Generating release zip package (Windows)"
        VERBATIM
    )
# Comando para empacotar arquivos no macOS (usando macdeployqt)
elseif(APPLE)
    add_custom_command(TARGET QTail POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Running macdeployqt..."
        COMMAND macdeployqt $<TARGET_FILE:QTail> -dmg
        COMMAND ${CMAKE_COMMAND} -E echo "Packaging release files for macOS..."
        COMMAND ${CMAKE_COMMAND} -E tar "cfv" ${OUTPUT_ZIP} --format=zip
            $<TARGET_FILE:QTail>               # Executável
            ${ASSETS_DIR}    # Pasta de assets
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Generating release zip package (macOS)"
        VERBATIM
    )
# Comando para empacotar arquivos no Linux (usando linuxdeployqt)
elseif(UNIX)
    add_custom_command(TARGET QTail POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Running linuxdeployqt..."
        COMMAND linuxdeployqt $<TARGET_FILE:QTail> -bundle -appimage
        COMMAND ${CMAKE_COMMAND} -E echo "Packaging release files for Linux..."
        COMMAND ${CMAKE_COMMAND} -E tar "cfv" ${OUTPUT_ZIP} --format=zip
            $<TARGET_FILE:QTail>               # Executável
            ${ASSETS_DIR}    # Pasta de assets
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Generating release zip package (Linux)"
        VERBATIM
    )
endif()

# Isso só será ativado em builds do tipo Release
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_custom_target(create_zip ALL DEPENDS QTail)
endif()
