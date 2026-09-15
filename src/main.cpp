#include "moderatorwindow.h"
#include "playerdialog.h"
#include "viewerwindow.h"

#include "model/game.h"
#include "buzzer/keyboardbuzzer.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // For QSettings in RememberingFileDialog
    a.setApplicationName("qwiz");
    a.setOrganizationName("qwiz");
    a.setOrganizationDomain("qwiz");

    Game game;
    PlayerDialog playerDialog(&game);
    ViewerWindow viewerWindow(&game);
    ModeratorWindow moderatorWindow(&game, &playerDialog);
    QObject::connect(&moderatorWindow, &ModeratorWindow::closed, [&playerDialog, &viewerWindow](){
        playerDialog.close();
        viewerWindow.close();
    });
    QObject::connect(&viewerWindow, &ViewerWindow::closed, [&playerDialog, &moderatorWindow](){
        playerDialog.close();
        moderatorWindow.close();
    });
    QObject::connect(&moderatorWindow, &ModeratorWindow::signalIncreaseFont, [&viewerWindow](){
        QFont font = viewerWindow.font();
        font.setPointSize(font.pointSize() + 1);
        viewerWindow.setFont(font);
        viewerWindow.update();
    });
    QObject::connect(&moderatorWindow, &ModeratorWindow::signalDecreaseFont, [&viewerWindow](){
        QFont font = viewerWindow.font();
        font.setPointSize(font.pointSize() - 1);
        viewerWindow.setFont(font);
        viewerWindow.update();
    });
    QObject::connect(&moderatorWindow, &ModeratorWindow::signalAlwaysShowCategoriesChanged, &viewerWindow, &ViewerWindow::setAlwaysShowCategories);

    QList<QWidget*> windowsThatListenToKeyboardBuzzers;
    windowsThatListenToKeyboardBuzzers<< &playerDialog;
    windowsThatListenToKeyboardBuzzers<< &moderatorWindow;
    windowsThatListenToKeyboardBuzzers<< &viewerWindow;
    QList<QList<QKeySequence>> sequences = {
        {Qt::Key_F1,  Qt::Key_F2,  Qt::Key_F3,  Qt::Key_F4 },
        {Qt::Key_F21, Qt::Key_F22, Qt::Key_F23, Qt::Key_F24}
    };
    for (QWidget* window : windowsThatListenToKeyboardBuzzers) {
        for (QList<QKeySequence> const& sequence : sequences) {
            KeyboardBuzzer* buzzer = new KeyboardBuzzer(window, sequence);
            QObject::connect(buzzer, &BuzzerBase::buzz, &playerDialog, &PlayerDialog::triggerBuzzerVisualization);
            QObject::connect(buzzer, &BuzzerBase::buzz, &game, &Game::buzz);
        }
    }

    moderatorWindow.show();
    viewerWindow.show();
    return a.exec();
}
