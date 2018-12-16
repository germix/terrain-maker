#-------------------------------------------------
#
# Project created by QtCreator 2018-12-09T09:09:03
#
#-------------------------------------------------

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TerrainMaker
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#DEFINES += DEBUG

#---------------------------------------------------------------------------------------------------
# Versión
#---------------------------------------------------------------------------------------------------

DEFINES += MAJOR_VERSION=1
DEFINES += MINOR_VERSION=0

#---------------------------------------------------------------------------------------------------
# Directorio de destino
#---------------------------------------------------------------------------------------------------
DESTDIR = ..\bin

#---------------------------------------------------------------------------------------------------
# Archivos de código
#---------------------------------------------------------------------------------------------------

SOURCES += \
        src/main.cpp \
        src/MainWindow.cpp \
    src/Document.cpp \
    src/Canvas.cpp \
    src/Triangle.cpp \
    src/TriangleVertex.cpp \
    src/Vertex.cpp \
    src/Vector2d.cpp \
    src/Tool.cpp \
    src/DocumentShape.cpp \
    src/ToolSelect.cpp \
    src/ToolPoints.cpp \
    src/utils.cpp \
    src/AboutDialog.cpp \
    src/Languages.cpp \
    src/NewDocumentDialog.cpp \
    src/RecentFilesMenu.cpp \
    src/ShowInContainerFolder.cpp \
    src/EditSizeDialog.cpp

HEADERS += \
        src/MainWindow.h \
    src/Document.h \
    src/Canvas.h \
    src/Triangle.h \
    src/TriangleVertex.h \
    src/Vertex.h \
    src/Vector2d.h \
    src/Tool.h \
    src/DocumentShape.h \
    src/ToolSelect.h \
    src/ToolPoints.h \
    src/AboutDialog.h \
    src/Languages.h \
    src/NewDocumentDialog.h \
    src/RecentFilesMenu.h \
    src/EditSizeDialog.h

FORMS += \
        src/MainWindow.ui \
    src/AboutDialog.ui \
    src/NewDocumentDialog.ui \
    src/EditSizeDialog.ui

#---------------------------------------------------------------------------------------------------
# Archivos de recursos
#---------------------------------------------------------------------------------------------------

RESOURCES += res/resource.qrc

win32:RC_FILE = res/resource_win32.rc

#---------------------------------------------------------------------------------------------------
# Archivos de traducción
#---------------------------------------------------------------------------------------------------
TRANSLATIONS =	translate/terrainmaker_es.ts \
                translate/terrainmaker_en.ts

