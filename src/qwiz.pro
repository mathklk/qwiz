QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += \
    c++17 \
    lrelease \
    embed_translations

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    buzzer/keyboardbuzzer.cpp \
    widgets/rememberingfiledialog.cpp \
    translation.cpp \
    model/board.cpp \
    model/category.cpp \
    model/game.cpp \
    model/jsonBoard.cpp \
    model/player.cpp \
    main.cpp \
    model/question.cpp \
    moderatorwindow.cpp \
    playerdialog.cpp \
    viewerwindow.cpp \
    widgets/imagelabel.cpp

HEADERS += \
    buzzer/buzzerbase.h \
    buzzer/keyboardbuzzer.h \
    translation.h \
    widgets/rememberingfiledialog.h \
    model/board.h \
    model/category.h \
    model/game.h \
    model/jsonBoard.h \
    model/player.h \
    model/question.h \
    moderatorwindow.h \
    playerdialog.h \
    viewerwindow.h \
    widgets/imagelabel.h

FORMS += \
    moderatorwindow.ui \
    playerdialog.ui \
    viewerwindow.ui

RESOURCES += \
    res.qrc

TRANSLATIONS += \
    ../rsrc/translation/qwiz_de.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
