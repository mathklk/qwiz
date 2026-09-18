#include "viewerwindow.h"
#include "ui_viewerwindow.h"

#include "widgets/imagelabel.h"

ViewerWindow::ViewerWindow(Game* game, QWidget *parent):
    QWidget(parent),
    ui(new Ui::ViewerWindow),
    _game(game)
{
    ui->setupUi(this);
    _playerBoxes << PlayerBox{ui->frame_1, ui->p1_name, ui->p1_points};
    _playerBoxes << PlayerBox{ui->frame_2, ui->p2_name, ui->p2_points};
    _playerBoxes << PlayerBox{ui->frame_3, ui->p3_name, ui->p3_points};
    _playerBoxes << PlayerBox{ui->frame_4, ui->p4_name, ui->p4_points};

    for (int i = 0; i < Game::nPlayers; ++i) {
        updatePlayerBox(i);
        connect(game->players()[i], &Player::changed, this, [this, i](){
            updatePlayerBox(i);
            updateGame(); // Box colors may need to be updated
        });
    }

    connect(_game, &Game::changed, this, &ViewerWindow::updateGame);
    updateGame();
}

ViewerWindow::~ViewerWindow()
{
    delete ui;
}

void ViewerWindow::changeEvent(QEvent* ev) {
    QWidget::changeEvent(ev);
    if (ev->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
}

void ViewerWindow::updateGame() {
    setWindowTitle(_game->board().title());

    for (int i = 0; i < Game::nPlayers; ++i) {
        updatePlayerBox(i);
    }

    // Clear Raster
    QLayout* oldLayout = ui->raster->layout();
    if (oldLayout != nullptr) {
    QLayoutItem* item;
        while ((item = oldLayout->takeAt(0))) {
            if (item->widget()) {
                item->widget()->deleteLater();
            }
            delete item;
        }
        delete oldLayout;
    }
    QGridLayout* rasterLayout = new QGridLayout;
    ui->raster->setLayout(rasterLayout);
    // Reset Layout size

    Board const& board = _game->board();
    Game::State const& state = _game->state();
    if (state == Game::State::idle or state == Game::State::finished or _alwaysShowCategories) {
        // Draw Board
        if (board.empty()) {
            rasterLayout->addWidget(new QLabel());
        }
        for (int iColumn = 0; iColumn < board.count(); ++iColumn) {
            // Add Categories
            Category const& category = board[iColumn];
            auto categoryNameLabel = new QLabel("<h3>" + category.name() + "</h3>");
            auto categoryFont = categoryNameLabel->font();
            categoryFont.setBold(true);
            categoryNameLabel->setFont(categoryFont);
            categoryNameLabel->setAlignment(Qt::AlignCenter);
            categoryNameLabel->setWordWrap(true);
            categoryNameLabel->setContentsMargins(16, 16, 16, 16);
            rasterLayout->addWidget(categoryNameLabel, 0, iColumn);

            // Add Question Boxes
            if (not (state == Game::State::idle or state == Game::State::finished)) {
                continue;
            }
            for (int iRow = 0; iRow < category.count(); ++iRow) {
                Question const& question = category[iRow];
                auto questionLabel = new QLabel("<h2>" + QString::number(question.points()) + "</h2>");
                questionLabel->setAlignment(Qt::AlignCenter);
                if (state == Game::State::idle and question.state() == Question::State::finished) {
                    questionLabel->setText("");
                }
                rasterLayout->addWidget(questionLabel, 1 + iRow, iColumn);
                if (state == Game::State::finished and question.solver() != nullptr) {
                    questionLabel->setStyleSheet(QString("QLabel { background-color: %1; }").arg(question.solver()->color().name()));
                }
            }
        }
        for (int iRow = 1; iRow < board.maxNumberOfQuestionsPerCategory() + 1; ++iRow) {
            rasterLayout->setRowStretch(iRow, 1);
        }
    }
    if (state == Game::State::activeQuestion or state == Game::State::judging or state == Game::State::answered) {
        Question const*const activeQuestion = _game->activeQuestion();

        QWidget *const questionLayoutWidget = new QWidget;
        QHBoxLayout *const questionLayout = new QHBoxLayout;
        questionLayoutWidget->setLayout(questionLayout);

        if (not activeQuestion->text().isEmpty()) {
            QLabel *const questionLabel = new QLabel(activeQuestion ? "<h1>" + activeQuestion->text() + "</h1>" : "");
            questionLabel->setAlignment(Qt::AlignCenter);
            questionLabel->setWordWrap(true);
            questionLayout->addWidget(questionLabel, 1);
        }
        if (activeQuestion->hasImage()) {
            ImageLabel *const imageLabel = new ImageLabel(QPixmap::fromImage(activeQuestion->image()));
            questionLayout->addWidget(imageLabel, 1);
        }
        if (activeQuestion->hasSolutionImage() and state == Game::State::answered) {
            ImageLabel *const solutionImageLabel = new ImageLabel(QPixmap::fromImage(activeQuestion->solutionImage()));
            questionLayout->addWidget(solutionImageLabel, 1);
        }
        if (_alwaysShowCategories) {
            rasterLayout->addWidget(questionLayoutWidget, 1, 0, board.maxNumberOfQuestionsPerCategory(), board.count());
            rasterLayout->setRowStretch(1, 1);
        } else {
            rasterLayout->addWidget(questionLayoutWidget, 0, 0);
            rasterLayout->setRowStretch(0, 1);
        }
    }
    mainLayout()->setStretchFactor(ui->horizontalLayout, 0);
}

void ViewerWindow::updatePlayerBox(int i) {
    PlayerBox& box = _playerBoxes[i];
    Player const*const player = _game->players().at(i);

    bool const isFailer = _game->activeQuestion() != nullptr and _game->activeQuestion()->failers().contains(player);
    QString boxStyleSheet;
    QString const backgroundColor = player->isActive() ? player->color().name() : "transparent";
    QString const borderColor = isFailer ? "transparent" : player->color().name();

    boxStyleSheet = QString("QFrame { background-color: %1; border: 6px solid %2; }").arg(backgroundColor, borderColor);
    box.frame->setStyleSheet(boxStyleSheet);
    box.nameLabel->setText("<b>" + player->name() + "</b>");
    box.nameLabel->setStyleSheet(QString("QFrame {border: 0px; background-color: %1;}").arg(backgroundColor));
    box.pointsLabel->setText(QString::number(player->points()));
    box.pointsLabel->setStyleSheet(QString("QFrame {border: 0px; background-color: %1;}").arg(backgroundColor));
}
