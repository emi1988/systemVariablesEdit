#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>

typedef struct systemVariables
{
    QString key;
    QString value;

};


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QProcess m_Process;
    QList<systemVariables> m_systemVariables;

    int m_sorting;

private slots:

   void consoleOutput();
   void consoleOutput2();
   void consoleErrOutput();
   void cellCklicked(int row, int col);
   void headerClicked(int col);
   void buttonCklicked();

   void searchValueChanged();
   void on_pushButtonSetNewValue_clicked();
};

#endif // MAINWINDOW_H
