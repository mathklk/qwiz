#include "moderatorwindow.h"
#include "ui_moderatorwindow.h"

#include "widgets/rememberingfiledialog.h"
#include "model/jsonBoard.h"
#include "translation.h"
#include "color.h"

#include <QMessageBox>
#include <QToolButton>
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

    // Locale Menu
    auto localeButton = new QPushButton();
    localeButton->setIcon(QIcon(":/icon/globe.svg"));
    localeButton->setFlat(true);
    auto localeMenu = new QMenu(localeButton);
    auto localeActionGroup = new QActionGroup(localeMenu);
    localeActionGroup->setExclusive(true);
    auto actionSystemLocale = localeMenu->addAction(tr("System language"));
    actionSystemLocale->setCheckable(true);
    if (QSettings().value("locale", Translation::SETTINGS_KEY_SYSTEM_LOCALE).toString() == Translation::SETTINGS_KEY_SYSTEM_LOCALE) {
        actionSystemLocale->setChecked(true);
    }
    connect(actionSystemLocale, &QAction::triggered, this, [this](){
        QSettings().setValue("locale", Translation::SETTINGS_KEY_SYSTEM_LOCALE);
        emit localeChanged();
    });
    localeActionGroup->addAction(actionSystemLocale);
    localeMenu->addSeparator();
    for (QString const& locale : Translation::availableLocales()) {
        auto action = localeMenu->addAction(
            QLocale::languageToString(QLocale(locale).language())
        );
        localeActionGroup->addAction(action);
        action->setCheckable(true);
        if (QSettings().value("locale").toString() == locale) {
            action->setChecked(true);
        }
        connect(action, &QAction::triggered, this, [this, locale](){
            QSettings().setValue("locale", locale);
            emit localeChanged();
        });
    }
    localeButton->setMenu(localeMenu);
    ui->menubar->setCornerWidget(localeButton);


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

    // Question Control Buttons
    _judgeButtons = {ui->buttonRight, ui->buttonWrong, ui->buttonPass};
    _estimationWinnerButtons = {ui->pushButtonP1, ui->pushButtonP2, ui->pushButtonP3, ui->pushButtonP4};
    connect(ui->buttonRight,   &QPushButton::clicked, _game, &Game::judgeCorrect  );
    connect(ui->buttonWrong,   &QPushButton::clicked, _game, &Game::judgeWrong    );
    connect(ui->buttonPass,    &QPushButton::clicked, _game, &Game::judgePass     );
    connect(ui->buttonSkip,    &QPushButton::clicked, _game, &Game::proceed       );
    connect(ui->buttonProceed, &QPushButton::clicked, this, [this](){
        if (_game->state() == Game::State::estimation) {
            // Reset player buttons for next question and collect estimation winner
            QList<int> winners;
            for (int i = 0; i < Game::nPlayers; ++i) {
                if (_estimationWinnerButtons[i]->isChecked()) {
                    winners.append(i);
                }
                _estimationWinnerButtons[i]->setChecked(false);
            }
            _game->chooseEsimationWinners(winners);
        }
        // Proceed, for either question type
        _game->proceed();
    });
    for (QPushButton *const estimationButton : _estimationWinnerButtons) {
        connect(estimationButton, &QPushButton::clicked, this, [this](){
            bool atLeastOneWinnerButtonIsChecked = false;
            for (QPushButton *const button : _estimationWinnerButtons) {
                atLeastOneWinnerButtonIsChecked |= button->isChecked();
            }
            ui->buttonProceed->setEnabled(atLeastOneWinnerButtonIsChecked);
        });
    }

    // Player Info in Status Bar
    ui->statusbar->setContentsMargins(0, 0, 0, 0);
    _statusBarLabels = {new QLabel(), new QLabel(), new QLabel(), new QLabel()};
    _statusBarLabels.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    for (auto label : _statusBarLabels) {
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

void ModeratorWindow::changeEvent(QEvent* ev) {
    QMainWindow::changeEvent(ev);
    if (ev->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
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
        QString const fileName = QFileInfo(filePath).fileName();
        QMessageBox::critical(this, "Error reading " + fileName, jsonException.detail());
    }
}

void ModeratorWindow::playerChanged(int const i) {
    Player const*const player = _game->players().at(i);

    // Status Bar Label
    QLabel *const statusBarLabel = _statusBarLabels[i];
    statusBarLabel->setText(QString("<b>%1</b> : %2").arg(player->name()).arg(player->points()));
    bool const isFailer = _game->activeQuestion() != nullptr and _game->activeQuestion()->failers().contains(player);
    QString const backgroundColor = player->isActive() ? player->color().name() : "transparent";
    QString const borderColor = isFailer ? "transparent" : player->color().name();
    statusBarLabel->setStyleSheet(
        QString("QFrame { padding: 2px; background-color: %1; border: 6px solid %2; }")
        .arg(backgroundColor)
        .arg(borderColor)
    );

    // EstimationButtons
    QPushButton *const estimationButton = _estimationWinnerButtons[i];
    estimationButton->setText(player->name());
}

void ModeratorWindow::updateBoard() {
    auto const& board = _game->board();
    Game::State const& state = _game->state();
    ui->groupBoxBoard->setVisible(state == Game::State::idle or state == Game::State::finished);

    int nEstimationQuestions = 0;
    for (Category const& category : board) {
        for (Question const& question : category) {
            nEstimationQuestions += question.state() == Question::State::hidden and question.type() == Question::Type::estimate;
        }
    }
    if (nEstimationQuestions > 0) {
        ui->groupBoxBoard->setTitle(tr("%1 estimation questions remaining").arg(nEstimationQuestions));
    }

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
                    if (not question.solvers().isEmpty()) {
                        QList<QColor> solverColors;
                        for (auto const& player : question.solvers()) {
                            solverColors.append(player->color());
                        }
                        cellLabel->setStyleSheet(QString("QLabel { background-color: %1; }").arg(
                            Color::mix(solverColors).name()
                        ));
                    }
                }
                cellWidget = cellLabel;
            }
            if (cellWidget != nullptr) {
                QString qText = question.text();
                if (question.hasImage()) {
                    qText = "[" + tr("Image") + "]\n" + qText;
                }
                QString qSolution = question.solution();
                if (question.hasSolutionImage()) {
                    qSolution = "[Solution Image]\n" + qSolution;
                }
                cellWidget->setToolTip(QString("%1\n---\n%2\n---\n%3").arg(question.points()).arg(qText, qSolution));
                ui->raster->setCellWidget(1 + iQuestion, iCategory, cellWidget);
            }
        }
    }
}

void ModeratorWindow::updateQuestion() {
    Category const*const category = _game->activeCategory();
    Question const*const question = _game->activeQuestion();
    auto const& state = _game->state();

    bool const questionIsVisible = state & (
        Game::State::waitForBuzz |
        Game::State::judging     |
        Game::State::answered    |
        Game::State::estimation
    );
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
    if (not questionIsVisible) {
        return;
    }

    _judgeButtons           .setVisible(question->type() == Question::Type::normal  );
    _estimationWinnerButtons.setVisible(question->type() == Question::Type::estimate);
    _judgeButtons           .setEnabled(state == Game::State::judging    );
    _estimationWinnerButtons.setEnabled(state == Game::State::estimation);
    ui->buttonProceed->setEnabled(state & (Game::State::answered    | Game::State::estimation));
    ui->buttonSkip   ->setEnabled(state & (Game::State::waitForBuzz | Game::State::estimation));

    ui->labelCategory->setText("<h2>" + category->name() + "</h2>");
    QString qText = question->text();
    if (qText.isEmpty() and question->hasImage()) {
        qText = "[" + tr("Image") + "]";
    }
    ui->labelQuestion->setText("<h1>" + qText + "</h1>");
    ui->labelSolution->setText("<h2>" + question->solution() + "</h2>");
}
