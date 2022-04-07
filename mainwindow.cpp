#include<ctime>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <algorithm>
#include <QLibrary>

static Astar::EstimateMethod usedEstimateMethod = Astar::EstimateMethod::DIF_BITS;
static bool shouldDraw = true;

#define MAX_DIF_NUM 10

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //set window name
    this->setWindowTitle("Astar Demo");

    this->puzzleStart.push_back(ui->block0);
    this->puzzleStart.push_back(ui->block1);
    this->puzzleStart.push_back(ui->block2);
    this->puzzleStart.push_back(ui->block3);
    this->puzzleStart.push_back(ui->block4);
    this->puzzleStart.push_back(ui->block5);
    this->puzzleStart.push_back(ui->block6);
    this->puzzleStart.push_back(ui->block7);
    this->puzzleStart.push_back(ui->block8);
    this->puzzleDestination.push_back(ui->block0_2);
    this->puzzleDestination.push_back(ui->block1_2);
    this->puzzleDestination.push_back(ui->block2_2);
    this->puzzleDestination.push_back(ui->block3_2);
    this->puzzleDestination.push_back(ui->block4_2);
    this->puzzleDestination.push_back(ui->block5_2);
    this->puzzleDestination.push_back(ui->block6_2);
    this->puzzleDestination.push_back(ui->block7_2);
    this->puzzleDestination.push_back(ui->block8_2);

    //log
    connect(Log::getInstance(), &Log::signal_log, this, &MainWindow::slot_onReceiveLog);

    //blocks
    for(auto btn: puzzleStart)
    {
        connect(btn, &QBlock::signal_blockRightPressed, this, &MainWindow::slot_decreaseNum);
        connect(btn, &QBlock::signal_blockLeftPressed, this, &MainWindow::slot_increaseNum);
    }
    for(auto btn: puzzleDestination)
    {
        connect(btn, &QBlock::signal_blockRightPressed, this, &MainWindow::slot_decreaseNum);
        connect(btn, &QBlock::signal_blockLeftPressed, this, &MainWindow::slot_increaseNum);
    }

    //controls
    connect(ui->btnValidate, &QPushButton::clicked, this, &MainWindow::slot_validateStates);
    connect(ui->btnShuffle, &QPushButton::clicked, this, &MainWindow::slot_shuffleStates);
    connect(ui->btnRun, &QPushButton::clicked, this, &MainWindow::slot_findRoute);
    connect(ui->btnNextState, &QPushButton::clicked, this, &MainWindow::slot_nextState);
    connect(ui->btnLastState, &QPushButton::clicked, this, &MainWindow::slot_lastState);
    connect(ui->btnEndDemo, &QPushButton::clicked, this, &MainWindow::slot_endDemo);
    connect(ui->btnAutoDemo, &QPushButton::clicked, this, &MainWindow::slot_autoDemo);

    //state transfer
    connect(this, &MainWindow::signal_transferState, this, &MainWindow::slot_transferState);

    //demonstration
    connect(this, &MainWindow::signal_autoDemo, this, &MainWindow::slot_autoNextState);

    //drawing
    this->drawWindow = new Drawing();
    connect(this, &MainWindow::signal_openDrawWindow, this, &MainWindow::slot_openDrawWindow);
    connect(this, &MainWindow::signal_drawSearchTree, this->drawWindow, &Drawing::slot_drawSearchTree);

    //initialize button actions
    std::vector<QAction*> actionArr;
    QAction* actionUseEstimate1 = new QAction("Use Estimate1(Different bits)",this);
    QAction* actionUseEstimate2 = new QAction("Use Estimate2(Different nums)",this);
    QAction* actionUseEstimate3 = new QAction("Use Estimate3(Manhattan Distance)",this);
    QAction* actionUseEstimate4 = new QAction("Use Estimate4(Horizontal Distance", this);
    QAction* actionShowSearchTree = new QAction("Draw search tree", this);
    actionArr.push_back(actionUseEstimate1);
    actionArr.push_back(actionUseEstimate2);
    actionArr.push_back(actionUseEstimate3);
    actionArr.push_back(actionUseEstimate4);
    for(auto elem: actionArr) elem->setCheckable(true);
    actionUseEstimate1->setChecked(true);
    actionShowSearchTree->setCheckable(true);
    actionShowSearchTree->setChecked(true);

    connect(actionShowSearchTree, &QAction::triggered, [=]()
    {
        shouldDraw = !shouldDraw;
        actionShowSearchTree->setChecked(shouldDraw);
    });

    connect(actionUseEstimate1, &QAction::triggered, [=]()
    {
        for(auto elem: actionArr) elem->setChecked(false);
        actionUseEstimate1->setChecked(true);
        usedEstimateMethod = Astar::EstimateMethod::DIF_BITS;
        LOG("Using estimate method:DIF_BITS.", Log::LogType::normal);
    });

    connect(actionUseEstimate2, &QAction::triggered, [=]()
    {
        for(auto elem: actionArr) elem->setChecked(false);
        actionUseEstimate2->setChecked(true);
        usedEstimateMethod = Astar::EstimateMethod::DIF_NUMS;
        LOG("Using estimate method:DIF_NUMS.", Log::LogType::normal);
    });

    connect(actionUseEstimate3, &QAction::triggered, [=]()
    {
        for(auto elem: actionArr) elem->setChecked(false);
        actionUseEstimate3->setChecked(true);
        usedEstimateMethod = Astar::EstimateMethod::MANHATTAN_DISTANCE;
        LOG("Using estimate method:MANHATTAN_DISTANCE.", Log::LogType::normal);
    });

    connect(actionUseEstimate4, &QAction::triggered, [=]()
    {
        for(auto elem: actionArr) elem->setChecked(false);
        actionUseEstimate4->setChecked(true);
        usedEstimateMethod = Astar::EstimateMethod::HORIZONTAL_DISTANCE;
        LOG("Using estimate method:HORIZONTAL_DISTANCE.", Log::LogType::normal);
    });

    //initialize submenu
    QMenu* subMenu = new QMenu(this);
    subMenu->addAction(actionUseEstimate1);
    subMenu->addAction(actionUseEstimate2);
    subMenu->addAction(actionUseEstimate3);
    subMenu->addAction(actionUseEstimate4);
    subMenu->addAction(actionShowSearchTree);

    ui->btnRun->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->btnRun, &QWidget::customContextMenuRequested, [=](const QPoint &pos)
    {
        subMenu->exec(QCursor::pos());
    });

    //initial state
    TRANSFER_STATE(State::IDLE);
}

void MainWindow::slot_transferState(const State state)
{
    this->state = state;
    switch(this->state)
    {
    case State::IDLE:
    {
        this->ui->btnValidate->setEnabled(true);
        this->ui->btnShuffle->setEnabled(true);
        this->ui->btnRun->setEnabled(false);
        this->ui->btnLastState->setEnabled(false);
        this->ui->btnNextState->setEnabled(false);
        this->ui->btnAutoDemo->setEnabled(false);
        this->ui->btnEndDemo->setEnabled(false);
        this->ui->puzzleStart->setEnabled(true);
        this->ui->puzzleDestination->setEnabled(true);
        this->ui->btnRun->setText("Run");
        this->demoStateIdx = 0;
        break;
    }
    case State::VALIDATED:
    {
        this->ui->btnValidate->setEnabled(false);
        this->ui->btnShuffle->setEnabled(true);
        this->ui->btnRun->setEnabled(true);
        this->ui->btnLastState->setEnabled(false);
        this->ui->btnNextState->setEnabled(false);
        this->ui->btnAutoDemo->setEnabled(false);
        this->ui->btnEndDemo->setEnabled(false);
        this->ui->puzzleStart->setEnabled(true);
        this->ui->puzzleDestination->setEnabled(true);
        this->ui->btnRun->setText("Run");
        break;
    }
    case State::RUN:
    {
        this->ui->btnValidate->setEnabled(false);
        this->ui->btnShuffle->setEnabled(false);
        this->ui->btnRun->setEnabled(false);
        this->ui->btnLastState->setEnabled(false);
        this->ui->btnNextState->setEnabled(false);
        this->ui->btnAutoDemo->setEnabled(false);
        this->ui->btnEndDemo->setEnabled(false);
        this->ui->puzzleStart->setEnabled(false);
        this->ui->puzzleDestination->setEnabled(false);
        this->ui->btnRun->setText("Running...");
        break;
    }
    case State::DEMO:
    {
        this->ui->btnValidate->setEnabled(false);
        this->ui->btnShuffle->setEnabled(false);
        this->ui->btnRun->setEnabled(false);
        this->ui->btnNextState->setEnabled(true);
        this->ui->btnLastState->setEnabled(true);
        this->ui->btnAutoDemo->setEnabled(true);
        this->ui->btnEndDemo->setEnabled(true);
        this->ui->puzzleStart->setEnabled(false);
        this->ui->puzzleDestination->setEnabled(false);
        this->ui->btnRun->setText("Run");
        this->ui->btnAutoDemo->setText("Auto Demonstrate");
        if(this->demoStateIdx == this->route.size() - 1)
            this->ui->btnNextState->setEnabled(false);
        if(this->demoStateIdx == 0)
            this->ui->btnLastState->setEnabled(false);
        break;
    }
    case State::AUTODEMO:
    {
        this->ui->btnValidate->setEnabled(false);
        this->ui->btnShuffle->setEnabled(false);
        this->ui->btnRun->setEnabled(false);
        this->ui->btnLastState->setEnabled(false);
        this->ui->btnNextState->setEnabled(false);
        this->ui->btnAutoDemo->setEnabled(true);
        this->ui->btnEndDemo->setEnabled(false);
        this->ui->puzzleStart->setEnabled(false);
        this->ui->puzzleDestination->setEnabled(false);
        this->ui->btnRun->setText("Run");
        this->ui->btnAutoDemo->setText("Stop");
        break;
    }
    }
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slot_increaseNum()
{
    QPushButton* btn = qobject_cast<QPushButton*>(sender());
    qint32 i = btn->text().toInt() + 1;
    if(i > 8) i = 0;
    setBlock(btn, i);

    TRANSFER_STATE(State::IDLE);
}

void MainWindow::slot_decreaseNum()
{
    QPushButton* btn = qobject_cast<QPushButton*>(sender());
    qint32 i = btn->text().toInt() - 1;
    if(i < 0) i = 8;
    setBlock(btn, i);

    TRANSFER_STATE(State::IDLE);
}

void MainWindow::setBlock(QObject *block, const int num)
{
    QPushButton* btn = qobject_cast<QPushButton*>(block);
    btn->setText(QString::number(num));
    switch(num)
    {
    case 0:
        btn->setStyleSheet("QPushButton{color:rgb(0, 0, 0);background-color:rgb(255, 255, 255)}");
        break;
    case 1:
        btn->setStyleSheet("QPushButton{color:rgb(10,255,255);background-color:rgb(1, 1, 1)}");
        break;
    case 2:
        btn->setStyleSheet("QPushButton{color:rgb(60,179,113);background-color:rgb(1, 1, 1)}");
        break;
    case 3:
        btn->setStyleSheet("QPushButton{color:rgb(255,140,0);background-color:rgb(1, 1, 1)}");
        break;
    case 4:
        btn->setStyleSheet("QPushButton{color:rgb(255,105,180);background-color:rgb(1, 1, 1)}");
        break;
    case 5:
        btn->setStyleSheet("QPushButton{color:rgb(148,0,211);background-color:rgb(1, 1, 1)}");
        break;
    case 6:
        btn->setStyleSheet("QPushButton{color:rgb(30,144,255);background-color:rgb(1, 1, 1)}");
        break;
    case 7:
        btn->setStyleSheet("QPushButton{color:rgb(255,69,0);background-color:rgb(1, 1, 1)}");
        break;
    case 8:
        btn->setStyleSheet("QPushButton{color:rgb(255,127,80);background-color:rgb(1, 1, 1)}");
        break;
    default:
        break;
    }

}

void MainWindow::slot_validateStates()
{
    std::vector<uint64_t> initial;
    for(auto num: puzzleStart) initial.push_back(num->text().toInt());
    std::vector<uint64_t> destination;
    for(auto num: puzzleDestination) destination.push_back(num->text().toInt());
    if(Astar::validate(Astar::toBits(initial), Astar::toBits(destination)))
    {
        LOG("States are valid.", Log::LogType::normal);
        LOG("Right click run button to select estimate method, left click run button to start running.", Log::LogType::normal);
        TRANSFER_STATE(State::VALIDATED);
    }
    else
    {
        LOG("States are invalid or destination can not be reached.", Log::LogType::error);
        TRANSFER_STATE(State::IDLE);
    }
}

void MainWindow::slot_onReceiveLog(const QString str, const Log::LogType type)
{
    QString s;
    switch(type)
    {
    case Log::LogType::normal:
        s = "[Log]";
        break;
    case Log::LogType::error:
        s = "[Error]";
        break;
    case Log::LogType::runtime:
        s = "[Runtime]";
        break;
    default:
        s = "[Log]";
        break;
    }

    this->ui->logOutput->append(s + str);
}


void MainWindow::slot_shuffleStates()
{
    std::vector<uint64_t> arr1 = {0, 1, 2, 3, 4, 5, 6, 7, 8};
    std::vector<uint64_t> arr2 = {0, 1, 2, 3, 4, 5, 6, 7, 8};
    do
    {
        std::random_shuffle(arr1.begin(), arr1.end());
        std::random_shuffle(arr2.begin(), arr2.end());
    }while(!Astar::validate(Astar::toBits(arr1), Astar::toBits(arr2)) || (Astar::estimate2(Astar::toBits(arr1), Astar::toBits(arr2)) > MAX_DIF_NUM));

    for(int i = 0; i < 9;i++)
    {
        setBlock(puzzleStart[i], arr1[i]);
    }

    for(int i = 0; i < 9;i++)
    {
        setBlock(puzzleDestination[i], arr2[i]);
    }

    LOG("States are shuffled.", Log::LogType::normal);
    TRANSFER_STATE(State::IDLE);
}

void MainWindow::slot_lastState()
{
    if(this->demoStateIdx > 0) this->demoStateIdx--;

    if(this->demoStateIdx == 0) this->ui->btnLastState->setEnabled(false);

    this->ui->btnNextState->setEnabled(true);

    if(this->demoStateIdx < this->route.size())
    {
        for(uint32_t i = 0;i < 9;i++)
        {
            setBlock(puzzleStart[i], this->route[demoStateIdx][i]);
        }
    }
}

void MainWindow::slot_nextState()
{
    if(this->demoStateIdx < this->route.size() - 1) this->demoStateIdx++;

    if(this->demoStateIdx == this->route.size() - 1) this->ui->btnNextState->setEnabled(false);

    this->ui->btnLastState->setEnabled(true);

    if(this->demoStateIdx < this->route.size())
    {
        for(uint32_t i = 0;i < 9;i++)
        {
            setBlock(puzzleStart[i], this->route[demoStateIdx][i]);
        }
    }
}

void MainWindow::slot_autoNextState()
{
    if(this->demoStateIdx < this->route.size() - 1) this->demoStateIdx++;

    if(this->demoStateIdx < this->route.size())
    {
        for(uint32_t i = 0;i < 9;i++)
        {
            setBlock(puzzleStart[i], this->route[demoStateIdx][i]);
        }
    }
}

DWORD WINAPI AutoDemoThread(LPVOID params)
{
    MainWindow* pThis = reinterpret_cast<MainWindow*>(params);
    pThis->autoDemo();
    return NULL;
}

void MainWindow::autoDemo()
{
    while(this->isAutoDemo)
    {
        emit this->signal_autoDemo();
        Sleep(800);
    }

    TRANSFER_STATE(State::DEMO);
}

void MainWindow::slot_autoDemo()
{
    if(this->hAutoDemoThread == nullptr)
    {
        TRANSFER_STATE(State::AUTODEMO);
        this->isAutoDemo = true;
        this->hAutoDemoThread = CreateThread(nullptr,NULL, AutoDemoThread, this, NULL, nullptr);
        CloseHandle(this->hAutoDemoThread);
    }
    else
    {
        this->isAutoDemo = false;
        this->hAutoDemoThread = nullptr;
        this->ui->btnAutoDemo->setEnabled(false);
        this->ui->btnAutoDemo->setText("Stopping...");
    }

}

DWORD WINAPI FindRouteThread(LPVOID params)
{
    MainWindow* pThis = reinterpret_cast<MainWindow*>(params);
    qRegisterMetaType<Log::LogType>("Log::LogType");
    qRegisterMetaType<MainWindow::State>("MainWindow::State");
    qRegisterMetaType<Astar::LPNode>("Astar::LPNode");

    pThis->findRoute();
    return NULL;
}

#define DRAW_COST_LIMIT 30

void MainWindow::findRoute()
{
    std::vector<uint64_t> initial;
    for(auto num: puzzleStart) initial.push_back(num->text().toInt());
    std::vector<uint64_t> destination;
    for(auto num: puzzleDestination) destination.push_back(num->text().toInt());

    DWORD startTime = clock();
    auto result = Astar::run(Astar::toBits(initial), Astar::toBits(destination), usedEstimateMethod);
    this->route = result.route;
    DWORD endTime = clock();

    QString str = "Algorithm returns after " + QString::number((endTime - startTime)) + " ms...";
    LOG(str, Log::LogType::runtime);

    if(route.empty())
    {
        LOG("Unable to find route.", Log::LogType::error);
        TRANSFER_STATE(State::IDLE);
    }
    else
    {
        LOG("Route has been found.", Log::LogType::normal);
        TRANSFER_STATE(State::DEMO);
        if(shouldDraw)
        {
            if(result.cost > DRAW_COST_LIMIT)
            {
                LOG("Cost exceeds drawable cost limit, can't draw search tree.", Log::LogType::error);
            }
            else
            {
                Astar::LPNode root = Astar::runDrawing(Astar::toBits(initial), Astar::toBits(destination), usedEstimateMethod);
                emit signal_openDrawWindow(root);
            }
        }
    }
}

void MainWindow::slot_openDrawWindow(Astar::LPNode root)
{
    this->drawWindow->show();
    emit signal_drawSearchTree(root);
}

void MainWindow::slot_findRoute()
{
    TRANSFER_STATE(MainWindow::State::RUN);
    HANDLE hThread = CreateThread(nullptr, NULL, FindRouteThread, (LPVOID)this, NULL, nullptr);
    LOG("Thread is created and running, hThread = 0d" + QString::number((int)hThread) + ", calculating routes...", Log::LogType::runtime);
    CloseHandle(hThread);
}

void MainWindow::slot_endDemo()
{
    TRANSFER_STATE(MainWindow::State::IDLE);
}
