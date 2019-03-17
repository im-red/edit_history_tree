#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <qevent.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->textEdit2->setDocument(ui->textEdit->lastDocument());
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    switch (event->type())
    {
    case QEvent::DragEnter:
    case QEvent::DragLeave:
    case QEvent::DragMove:
    case QEvent::Drop:
    case QEvent::KeyPress:
    case QEvent::KeyRelease:
    case QEvent::MouseMove:
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::InputMethod:
    case QEvent::InputMethodQuery:
        //if (watched != this)
        {
            //qDebug() << watched << event;
        }
        break;
    default:
        break;
    }
    return QObject::eventFilter(watched, event);
}
