#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <Windows.h>
#include <QMainWindow>
#include "log.h"
#include "ui_mainwindow.h"
#include "qblock.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    enum class State
    {
        IDLE = 0,
        VALIDATED,
        RUN,
        DEMO,
        AUTODEMO,
    };

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void findRoute();

    void autoDemo();

private:
    Ui::MainWindow *ui;

    MainWindow::State state{State::IDLE};    //current state

    std::vector<QBlock*> puzzleStart;
    std::vector<QBlock*> puzzleDestination;

    std::vector<std::vector<uint64_t>> route;

    uint32_t demoStateIdx{NULL};

    HANDLE hAutoDemoThread{nullptr};

    bool isAutoDemo{false};

private:
    void setBlock(QObject* block, const int num);

signals:
    void signal_transferState(const MainWindow::State state);

    void signal_autoDemo();

private slots:
   //log
   void slot_onReceiveLog(const QString str, const Log::LogType type);

   //button slots
   void slot_increaseNum();
   void slot_decreaseNum();
   void slot_validateStates();
   void slot_shuffleStates();
   void slot_findRoute();
   void slot_lastState();
   void slot_nextState();
   void slot_autoDemo();
   void slot_endDemo();
   void slot_autoNextState();

   //state transfer
   void slot_transferState(const MainWindow::State state);
};

#define TRANSFER_STATE(state) \
    emit signal_transferState(state);

#endif // MAINWINDOW_H
