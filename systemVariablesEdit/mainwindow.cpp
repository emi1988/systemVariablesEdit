#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QString>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //m_Process.setProcessChannelMode(QProcess::MergedChannels);
    //m_Process.setProcessChannelMode(QProcess::SeparateChannels);
    m_Process.setReadChannel(QProcess::StandardOutput);

    connect(&m_Process, SIGNAL(readyReadStandardOutput()), this, SLOT(consoleOutput()));

   // connect(&m_Process, SIGNAL(readyReadStandardOutput()), this, SLOT(consoleOutput()));


  //  m_Process.start("cmd.exe /C start set");
    m_Process.start("cmd.exe" , QStringList() << "/C" << "set");
  //  m_Process.start("cmd.exe" , QStringList() << "/C" << "help");



    //qDebug() << "wait finished";

 //   QByteArray response = m_Process.readAllStandardOutput();
  //  qDebug() << QString(response);

    //m_Process.waitForReadyRead();
    //QByteArray stdOutput = m_Process.readAllStandardOutput();
    //QByteArray response =  m_Process.readAll();

 // qDebug() << QString(stdOutput);
}

MainWindow::~MainWindow()
{
    delete ui;
    m_Process.close();
}

void MainWindow::consoleOutput()
{
    //QByteArray response = m_Process.readAllStandardOutput();
   // qDebug() << QString(response);

  //  QString line = QString::fromLocal8Bit(m_Process.readAll());
  //  qDebug() << QString(line);

    systemVariables currentVar;
    while(m_Process.canReadLine())
    {
        QByteArray line = m_Process.readLine();

        qDebug() << line;
        QString lineString = QString(line);


        QStringList splitedSysVar = lineString.split("=");
        currentVar.key = splitedSysVar.at(0);
        currentVar.value = splitedSysVar.at(1);
        m_systemVariables.append(currentVar);

    }

    ui->tableWidget->clear();
    ui->tableWidget->setColumnCount(3);
    ui->tableWidget->setRowCount(m_systemVariables.count());

    for (int i = 0; i < m_systemVariables.count(); ++i)
    {        
        QPushButton *newSetButton = new QPushButton("set" + QString::number(i));
        ui->tableWidget->setCellWidget(i,0,newSetButton);

        connect(newSetButton, SIGNAL(clicked()), this, SLOT(buttonCklicked()));

        QTableWidgetItem *newKeyItem = new QTableWidgetItem(m_systemVariables.at(i).key);
        ui->tableWidget->setItem(i,1,newKeyItem);

        QTableWidgetItem *newValueItem = new QTableWidgetItem(m_systemVariables.at(i).value);
        ui->tableWidget->setItem(i,2,newValueItem);
    }


    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget->resizeColumnsToContents();

    connect(ui->tableWidget, SIGNAL(cellClicked(int,int)), this, SLOT(cellCklicked(int,int)));

}

void MainWindow::cellCklicked(int row, int col)
{

    qDebug() << "cell clicked row:" <<row << "col:" << col;
}

void MainWindow::buttonCklicked()
{
    QPushButton* button = qobject_cast<QPushButton*>(sender());

    qDebug() <<"button clicked" << button->text();

    QString buttonText = button->text();
    QStringList split = buttonText.split("set");

    QString currentKey = ui->tableWidget->item(split.at(1).toInt(),1)->text();
    QString currentValue = ui->tableWidget->item(split.at(1).toInt(),2)->text();

    m_Process.start("cmd.exe" , QStringList() << "/C" << "setx /m " + currentKey + " " + currentValue);

}
