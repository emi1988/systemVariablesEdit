#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QString>
#include <QPushButton>
#include <QHeaderView>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //m_Process.setProcessChannelMode(QProcess::MergedChannels);
    //m_Process.setProcessChannelMode(QProcess::SeparateChannels);
    m_Process.setReadChannel(QProcess::StandardOutput);

    connect(&m_Process, SIGNAL(readyReadStandardOutput()), this, SLOT(consoleOutput()));

    connect(&m_Process, SIGNAL(readyReadStandardError()), this, SLOT(consoleErrOutput()));

    connect(ui->TextEditSearch, SIGNAL(textChanged()), this, SLOT(searchValueChanged()));

    connect(ui->tableWidget,SIGNAL(cellClicked(int,int)),this, SLOT(cellCklicked(int,int)));

    connect(ui->tableWidget->horizontalHeader(),SIGNAL(sectionClicked(int)),this,SLOT(headerClicked(int)));

    m_Process.start("cmd.exe" , QStringList() << "/C" << "set");

}

MainWindow::~MainWindow()
{
    delete ui;
    m_Process.close();
}


//receives the output from "set"-command --> receives the system variables and saves them
void MainWindow::consoleOutput()
{

    systemVariables currentVar;
    while(m_Process.canReadLine())
    {
        QByteArray line = m_Process.readLine();

        qDebug() << line;
        QString lineString = QString(line);


        QStringList splittedSysVar = lineString.split("=");
        currentVar.key = splittedSysVar.at(0);
        currentVar.value = splittedSysVar.at(1);

        //check if the value has ";" -> each should appear in a new line
        QStringList splittedValues = currentVar.value.simplified().split(";",QString::SkipEmptyParts);

        QString editedValue;
        if(splittedValues.count() == 1)
        {
            editedValue = currentVar.value;
        }
        else
        {
            for (int i = 0; i < splittedValues.count(); ++i)
            {
                editedValue.append(splittedValues.at(i) + ";\n");
            }

        }

        currentVar.value = editedValue;

        m_systemVariables.append(currentVar);

    }

    ui->tableWidget->clear();
    ui->tableWidget->setColumnCount(3);
    ui->tableWidget->setRowCount(m_systemVariables.count());

    //set table headers
    ui->tableWidget->setHorizontalHeaderItem(0, new QTableWidgetItem("Edit"));
    ui->tableWidget->setHorizontalHeaderItem(1, new QTableWidgetItem("Key"));
    ui->tableWidget->setHorizontalHeaderItem(2, new QTableWidgetItem("Value"));


    for (int i = 0; i < m_systemVariables.count(); ++i)
    {
//        QPushButton *newSetButton = new QPushButton("set value");
//        newSetButton->setProperty("row", QVariant(i));
//        ui->tableWidget->setCellWidget(i,0,newSetButton);

//        connect(newSetButton, SIGNAL(clicked()), this, SLOT(buttonCklicked()));

        QTableWidgetItem *newKeyItem = new QTableWidgetItem(m_systemVariables.at(i).key);
        ui->tableWidget->setItem(i,1,newKeyItem);

        QTableWidgetItem *newValueItem = new QTableWidgetItem(m_systemVariables.at(i).value);
        ui->tableWidget->setItem(i,2,newValueItem);
    }

    //sort by names
    m_sorting = Qt::AscendingOrder;
    ui->tableWidget->sortByColumn(1,(Qt::SortOrder)m_sorting);

    //after sorting add the buttons
    for (int i = 0; i < m_systemVariables.count(); ++i)
    {
        QPushButton *newSetButton = new QPushButton("set value");
        newSetButton->setProperty("row", QVariant(i));
        ui->tableWidget->setCellWidget(i,0,newSetButton);

        connect(newSetButton, SIGNAL(clicked()), this, SLOT(buttonCklicked()));
    }
    //resize the table to the contents
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget->resizeColumnsToContents();
    ui->tableWidget->resizeRowsToContents();

}

//receives the data from the "setx"-command output
void MainWindow::consoleOutput2()
{

    QByteArray response = m_Process.readAllStandardOutput();
    QString responseString = QString(response).simplified();
    qDebug() << QString(responseString).simplified();

    //ignore empty responses
    if(responseString.compare("") != 0)
    {
        ui->textEditOutput->setTextColor(QColor(0,200,0));
        ui->textEditOutput->setText(responseString);
    }
}

void MainWindow::consoleErrOutput()
{

    QByteArray response = m_Process.readAllStandardError();
    QString responseString = QString(response).simplified();
    qDebug() << QString(responseString).simplified();

    ui->textEditOutput->setTextColor(QColor(255,0,0));
    ui->textEditOutput->setText(responseString);

}

void MainWindow::cellCklicked(int row, int col)
{

    qDebug() << "cell clicked row:" <<row << "col:" << col;

    //resize the clicked row to the current content
    ui->tableWidget->verticalHeader()->setSectionResizeMode(row,QHeaderView::ResizeToContents);
}

void MainWindow::headerClicked(int col)
{
     qDebug() << "header clicked row:"  "col:" << col;

     //chenge the sort order
     //TODO: to use this we have to change the row-property of the button.
     //-->after sorting they don't correspond to the current rows

//     if(m_sorting == Qt::AscendingOrder)
//     {
//         m_sorting = Qt::DescendingOrder;
//     }
//     else
//     {
//        m_sorting = Qt::AscendingOrder ;
//     }

//     ui->tableWidget->sortByColumn(1,(Qt::SortOrder)m_sorting);
}

//one of the set-buttons was clicked
void MainWindow::buttonCklicked()
{
    //reconnect the signal to new slot for receiving the "setx" command output
    disconnect(&m_Process, SIGNAL(readyReadStandardOutput()), this, SLOT(consoleOutput()));
    connect(&m_Process, SIGNAL(readyReadStandardOutput()), this, SLOT(consoleOutput2()));

    QPushButton* button = qobject_cast<QPushButton*>(sender());

    int currentRow = button->property("row").toInt();

    qDebug() <<"button clicked" <<currentRow;


        //qDebug() <<"button clicked" << button->text();
//    QString buttonText = button->text();
//    QStringList split = buttonText.split("set");

   // QString currentKey = ui->tableWidget->item(split.at(1).toInt(),1)->text();
    //QString currentValue = ui->tableWidget->item(split.at(1).toInt(),2)->text().simplified();

    QString currentKey = ui->tableWidget->item(currentRow,1)->text();
    QString currentValue = ui->tableWidget->item(currentRow,2)->text().simplified();

    QString prefix;

    if(ui->checkBoxUserVar->isChecked())
    {
        prefix = " ";
    }
    else
    {
        //write value to the global-sys-variables

        prefix = "/m ";
    }
    //QStringList commandList =QStringList() << "/C" << "setx " + prefix + currentKey + " \"" + currentValue + "\" ";

   // QStringList commandList =QStringList() << "/C" << "setx "  << prefix << currentKey << currentValue;

    QStringList commandList =QStringList() << "cmd.exe" << "/C" << "setx " + prefix + currentKey + " \"" + currentValue + "\" ";

    QString commandString;

    commandString = commandList.join(" ");
    qDebug() << "commandString: " << commandString;

    if(commandString.length() >1000)
    {
        QString testString= "halloIchHeißeEmanuelHalberstadt";

        QString part =  testString.mid(0,10);
        QString part2 =  testString.mid(10,10);
    }
    else
    {


        m_Process.start(commandString);
    }

   // m_Process.start("cmd.exe" , commandList);


}

void MainWindow::searchValueChanged()
{

    ui->tableWidget->clearContents();

    QList<systemVariables> systemVariablesSearched;

    //first check if the search field is empty
    if(ui->TextEditSearch->toPlainText().compare("") == 0)
    {
        //use all SystemVariables
        systemVariablesSearched = m_systemVariables;
    }
    else
    {
        //check the systemvariables for the searched text
        for (int i = 0; i < m_systemVariables.count(); ++i)
        {

            //just use the systemvariables which contain the searched text
            if(m_systemVariables.at(i).key.contains(ui->TextEditSearch->toPlainText(),Qt::CaseInsensitive))
            {
                systemVariablesSearched.append(m_systemVariables.at(i));
            }
        }

    }
    for (int i = 0; i < systemVariablesSearched.count(); ++i)
    {
//        QPushButton *newSetButton = new QPushButton("set" + QString::number(i));
//        ui->tableWidget->setCellWidget(i,0,newSetButton);

//        connect(newSetButton, SIGNAL(clicked()), this, SLOT(buttonCklicked()));

        QTableWidgetItem *newKeyItem = new QTableWidgetItem(systemVariablesSearched.at(i).key);
        ui->tableWidget->setItem(i,1,newKeyItem);

        QTableWidgetItem *newValueItem = new QTableWidgetItem(systemVariablesSearched.at(i).value);
        ui->tableWidget->setItem(i,2,newValueItem);
    }

    //sort by names
    m_sorting = Qt::AscendingOrder;
    ui->tableWidget->sortByColumn(1,(Qt::SortOrder)m_sorting);

    //after sorting add the buttons
    for (int i = 0; i < systemVariablesSearched.count(); ++i)
    {
        QPushButton *newSetButton = new QPushButton("set value");
        newSetButton->setProperty("row", QVariant(i));
        ui->tableWidget->setCellWidget(i,0,newSetButton);

        connect(newSetButton, SIGNAL(clicked()), this, SLOT(buttonCklicked()));
    }

    //resize the table to the contents
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget->resizeColumnsToContents();
    ui->tableWidget->resizeRowsToContents();

    //scroll to the top
    ui->tableWidget->scrollToTop();



}

void MainWindow::on_pushButtonSetNewValue_clicked()
{

    //reconnect the signal to new slot for receiving the "setx" command output
    disconnect(&m_Process, SIGNAL(readyReadStandardOutput()), this, SLOT(consoleOutput()));
    connect(&m_Process, SIGNAL(readyReadStandardOutput()), this, SLOT(consoleOutput2()));

    QString currentKey = ui->TextEditNewKey->toPlainText();
    QString currentValue = ui->TextEditNewValue->toPlainText();

    QString prefix;

    if(ui->checkBoxUserVar->isChecked())
    {
        prefix = " ";
    }
    else
    {
        //write value to the global-sys-variables

        prefix = "/m ";
    }

    QStringList commandList =QStringList() << "cmd.exe" << "/C" << "setx " + prefix + currentKey + " \"" + currentValue + "\" ";

    QString commandString;

    commandString = commandList.join(" ");
    qDebug() << "commandString for new Pair: " << commandString;

    m_Process.start(commandString);

}
