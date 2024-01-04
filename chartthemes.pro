QT += charts
QT += core gui multimedia multimediawidgets widgets

HEADERS += \
    qmychartview.h \
    themewidget.h

SOURCES += \
    main.cpp \
    qmychartview.cpp \
    themewidget.cpp

target.path = $$[QT_INSTALL_EXAMPLES]/charts/chartthemes
INSTALLS += target

FORMS += \
    themewidget.ui

#win32: LIBS += -LD:/Anaconda3/envs/gym/libs/ -lpython39

#INCLUDEPATH += D:/Anaconda3/envs/gym/libs
#DEPENDPATH += D:/Anaconda3/envs/gym/libs

#win32:!win32-g++: PRE_TARGETDEPS += D:/Anaconda3/envs/gym/libs/python39.lib
