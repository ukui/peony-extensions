#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "computer-view.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setCentralWidget(new ComputerView);
}

MainWindow::~MainWindow()
{
    delete ui;
}

