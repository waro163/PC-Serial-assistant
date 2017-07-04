#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <vector>
#include <QtCore/QtGlobal>
#include <QTimer>
#include <QMainWindow>
#include <QMessageBox>
#include <QInputDialog>
#include <fstream>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

using namespace std;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    struct Settings {
        QString name;
        qint32 baudRate;
        QString stringBaudRate;
        QSerialPort::DataBits dataBits;
        QString stringDataBits;
        QSerialPort::Parity parity;
        QString stringParity;
        QSerialPort::StopBits stopBits;
        QString stringStopBits;
        QSerialPort::FlowControl flowControl;
        QString stringFlowControl;
    };
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
//    QTimer *timer;
    QSerialPort *serial;
    ofstream fout;
    QString fileName;
//    QByteArray serialData;
    Settings updataSettings();
    void fillPortsParameters();
    void enableConfigPortsParameters();
    void disableConfigPortsParameters();

    Ui::MainWindow *ui;

private slots:
    void fillPortsInfo();
    void openSerialPort();
    void readData();
    void sendData();
    void handleTimeout();
    void clearRecvMsg();
    void clearSendMsg();
    void saveFileName(bool);
    void handleError(QSerialPort::SerialPortError error);
};

#endif // MAINWINDOW_H
