#include "moderatorwindow.h"
#include "ui_moderatorwindow.h"

#include "filedialog/rememberingfiledialog.h"
#include "model/jsonBoard.h"

#include <QMessageBox>
//#include <QHBoxLayout>
#include <QActionGroup>

ModeratorWindow::ModeratorWindow(Game* game, PlayerDialog* playerDialog, QWidget* parent):
    QMainWindow(parent),
    ui(new Ui::ModeratorWindow),
    _playerDialog(playerDialog),
    _game(game)
{
    ui->setupUi(this);
    connect(ui->actionPlayers, &QAction::triggered, _playerDialog, &QWidget::show);
    connect(ui->actionNew,     &QAction::triggered, this, &ModeratorWindow::newGameClicked);
    connect(ui->actionIncrease_Font_Size, &QAction::triggered, this, &ModeratorWindow::signalIncreaseFont);
    connect(ui->actionDecrease_Font_Size, &QAction::triggered, this, &ModeratorWindow::signalDecreaseFont);
    connect(ui->actionAlways_Show_Categories, &QAction::toggled, this, &ModeratorWindow::signalAlwaysShowCategoriesChanged);

    // Rules Menus
    auto pointDeductionActionGroup = new QActionGroup(this);
    QVector<QPair<QAction*, float>> pointDeductionActions = {
        {ui->action0_None,   0.00f},
        {ui->action0_1,      0.10f},
        {ui->action0_25,     0.25f},
        {ui->action0_5,      0.50f},
        {ui->action1_0_Full, 1.00f}
    };
    for (auto const option : pointDeductionActions) {
        pointDeductionActionGroup->addAction(option.first);
        if (_game->rules().wrongAnswerPointDeductionRatio == option.second) {
            option.first->setChecked(true);
        }
        connect(option.first, &QAction::triggered, this, [this, option](){
            Game::Rules rules = _game->rules();
            rules.wrongAnswerPointDeductionRatio = option.second;
            _game->setRules(rules);
        });
    }

    auto nextPlayerPolicyActionGroup = new QActionGroup(this);
    QVector<QPair<QAction*, Game::Rules::NextPlayerPolicy>> nextPlayerPolicyActions = {
        {ui->actionRandom,   Game::Rules::NextPlayerPolicy::random},
        {ui->actionCyclic,   Game::Rules::NextPlayerPolicy::cyclic},
        {ui->actionWinner,   Game::Rules::NextPlayerPolicy::winner},
        {ui->actionUnderdog, Game::Rules::NextPlayerPolicy::underdog}
    };
    for (auto const option : nextPlayerPolicyActions) {
        nextPlayerPolicyActionGroup->addAction(option.first);
        if (_game->rules().nextPlayerPolicy == option.second) {
            option.first->setChecked(true);
        }
        connect(option.first, &QAction::triggered, this, [this, option](){
            Game::Rules rules = _game->rules();
            rules.nextPlayerPolicy = option.second;
            _game->setRules(rules);
        });
    }

    // Judge Buttons
    connect(ui->buttonRight,   &QPushButton::clicked, _game, &Game::judgeCorrect  );
    connect(ui->buttonWrong,   &QPushButton::clicked, _game, &Game::judgeWrong    );
    connect(ui->buttonPass,    &QPushButton::clicked, _game, &Game::judgePass     );
    connect(ui->buttonProceed, &QPushButton::clicked, _game, &Game::proceed       );
    connect(ui->buttonSkip,    &QPushButton::clicked, _game, &Game::proceed       );

    // Player Info in Status Bar
    ui->statusbar->setContentsMargins(0, 0, 0, 0);
    _statusBarLabels = {new QLabel(), new QLabel(), new QLabel(), new QLabel()};
    for (auto label : _statusBarLabels) {
        label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        ui->statusbar->addWidget(label, 1);
    }
    for (int i = 0; i < Game::nPlayers; ++i) {
        connect(_game->players().at(i), &Player::changed, this, [this, i](){ playerChanged(i); });
        playerChanged(i);
    }

    // Board raster
    ui->raster->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->raster->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    //ui->raster->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    ui->raster->setFocusPolicy(Qt::NoFocus);
    ui->raster->setFrameStyle(QFrame::NoFrame);

    connect(_game, &Game::changed, this, [this](){
        updateBoard();
        updateQuestion();
    });
    updateQuestion();
    updateBoard();
}

ModeratorWindow::~ModeratorWindow() {
    delete ui;
}

void ModeratorWindow::newGameClicked() {
    QString const filePath = RememberingFileDialog::getOpenFileName(
        this,
        "boardJson",
        "Load Board-JSON",
        "Board-JSON (*.json)"
    );
    if (filePath.isNull() or filePath.isEmpty()) {
        return;
    }

    try {
        Board const board = JsonBoard::fromJsonFile(QFile(filePath));
        _game->start(board);
        setWindowTitle("qwiz | " + board.title());
    } catch (JsonBoard::JsonException const& jsonException) {
        QMessageBox::critical(this, "File Error", jsonException.detail());
    }
}

void ModeratorWindow::playerChanged(int const i) {
    Player const*const player = _game->players().at(i);
    QLabel *const label = _statusBarLabels[i];
    label->setText(QString("<b>%1</b> : %2").arg(player->name()).arg(player->points()));

    bool const isFailer = _game->activeQuestion() != nullptr and _game->activeQuestion()->failers().contains(player);
    QString const backgroundColor = player->isActive() ? player->color().name() : "transparent";
    QString const borderColor = isFailer ? "transparent" : player->color().name();
    label->setStyleSheet(
        QString("QFrame { padding: 2px; background-color: %1; border: 6px solid %2; }")
        .arg(backgroundColor)
        .arg(borderColor)
    );
}

void ModeratorWindow::updateBoard() {
    auto const& board = _game->board();
    Game::State const& state = _game->state();
    ui->groupBoxBoard->setVisible(state == Game::State::idle or state == Game::State::finished);

    ui->raster->clear();
    ui->raster->setColumnCount(board.count());
    ui->raster->setRowCount(1 + board.maxNumberOfQuestionsPerCategory());
    for (int iCategory = 0; iCategory < board.count(); ++iCategory) {
        // Add Category Names / Header
        Category const& category = board[iCategory];
        auto categoryNameItem = new QTableWidgetItem(category.name());
        auto categoryFont = categoryNameItem->font();
        categoryFont.setBold(true);
        categoryNameItem->setFont(categoryFont);
        categoryNameItem->setTextAlignment(Qt::AlignCenter);
        ui->raster->setItem(0, iCategory, categoryNameItem);

        // Add Question Cells
        for (int iQuestion = 0; iQuestion < category.count(); ++iQuestion) {
            if (iQuestion > category.count()) {
                continue;
            }
            Question const& question = category[iQuestion];
            QWidget* cellWidget = nullptr;
            if (question.state() == Question::State::hidden) {
                QPushButton* cellButton = new QPushButton(QString::number(question.points()));
                connect(cellButton, &QPushButton::clicked, this, [this, iCategory, iQuestion](){
                    _game->activateQuestion(iCategory, iQuestion);
                });
                cellWidget = cellButton;
            } else if (question.state() == Question::State::finished) {
                QLabel* cellLabel = new QLabel();
                cellLabel->setAlignment(Qt::AlignCenter);
                if (state == Game::State::finished) {
                    cellLabel->setText(QString::number(question.points()));
                    if (question.solver() != nullptr) {
                        cellLabel->setStyleSheet(QString("QLabel { background-color: %1; }").arg(question.solver()->color().name()));
                    }
                }
                cellWidget = cellLabel;
            }
            if (cellWidget != nullptr) {
                ui->raster->setCellWidget(1 + iQuestion, iCategory, cellWidget);
            }
        }
    }
}

void ModeratorWindow::updateQuestion() {
    Question const*const question = _game->activeQuestion();
    auto const& state = _game->state();

    bool const questionIsVisible = state == Game::State::activeQuestion or state == Game::State::judging or state == Game::State::answered;
    ui->frameQuestion->setVisible(questionIsVisible);
    ui->frameJudge   ->setVisible(questionIsVisible);
    for (QSpacerItem *const spacer : {ui->questionSpacerTop, ui->questionSpacerBottom}) {
        if (questionIsVisible) {
            spacer->changeSize(0, 0, QSizePolicy::Fixed, QSizePolicy::Expanding);
        } else {
            spacer->changeSize(0, 0, QSizePolicy::Fixed, QSizePolicy::Fixed);
        }
        spacer->invalidate();
    }
    ui->buttonRight  ->setEnabled(state == Game::State::judging);
    ui->buttonWrong  ->setEnabled(state == Game::State::judging);
    ui->buttonPass   ->setEnabled(state == Game::State::judging);
    ui->buttonProceed->setEnabled(state == Game::State::answered);
    ui->buttonSkip   ->setEnabled(state == Game::State::activeQuestion);

    if (question == nullptr) {
        ui->labelQuestion->setText("");
        ui->labelSolution->setText("");
        return;
    }

    ui->labelQuestion->setText("<h1>" + question->text() + "</h1>");
    ui->labelSolution->setText("<h2>" + question->solution() + "</h2>");
}
