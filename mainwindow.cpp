#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QTextCodec>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    serial = new QSerialPort(this);
//    timer = new QTimer(this);
    connect(serial, SIGNAL(error(QSerialPort::SerialPortError)), this,
            SLOT(handleError(QSerialPort::SerialPortError)));
    connect(serial,SIGNAL(readyRead()),this,SLOT(readData()));
    connect(ui->refreshButton,SIGNAL(clicked(bool)),this,SLOT(fillPortsInfo()));
    connect(ui->openButton,SIGNAL(clicked(bool)),this,SLOT(openSerialPort()));
    connect(ui->sendButton,SIGNAL(clicked(bool)),this,SLOT(sendData()));
    connect(ui->clearRecvButton,SIGNAL(clicked(bool)),this,SLOT(clearRecvMsg()));
    connect(ui->clearSendButton,SIGNAL(clicked(bool)),this,SLOT(clearSendMsg()));
    connect(ui->saveFilecheckBox,SIGNAL(toggled(bool)),this,SLOT(saveFileName(bool)));
//    connect(timer,SIGNAL(timeout()),this,SLOT(handleTimeout()));
    fillPortsParameters();
    fillPortsInfo();
    updataSettings();
}

MainWindow::~MainWindow()
{
//    delete timer;
    fout.close();
    delete serial;
    delete ui;
}

MainWindow::Settings MainWindow::updataSettings()
{
    Settings currentSettings;
    currentSettings.name = ui->serialNumListBox->currentText();
    currentSettings.baudRate = static_cast<QSerialPort::BaudRate>(ui->baudRateBox->itemData(ui->baudRateBox->currentIndex()).toInt());
    currentSettings.stringBaudRate = QString::number(currentSettings.baudRate);
    currentSettings.dataBits =  static_cast<QSerialPort::DataBits>(ui->dataBitsBox->itemData(ui->dataBitsBox->currentIndex()).toInt());
    currentSettings.stringDataBits = ui->dataBitsBox->currentText();
    currentSettings.parity =  static_cast<QSerialPort::Parity>(ui->parityBox->itemData(ui->parityBox->currentIndex()).toInt());
    currentSettings.stringParity = ui->parityBox->currentText();
    currentSettings.stopBits =  static_cast<QSerialPort::StopBits>(ui->stopBitsBox->itemData(ui->stopBitsBox->currentIndex()).toInt());
    currentSettings.stringStopBits = ui->stopBitsBox->currentText();
    currentSettings.flowControl =  static_cast<QSerialPort::FlowControl>(ui->flowControlBox->itemData(ui->flowControlBox->currentIndex()).toInt());
    currentSettings.stringFlowControl = ui->flowControlBox->currentText();

    return currentSettings;
}

void MainWindow::fillPortsParameters()
{
    ui->baudRateBox->addItem(QStringLiteral("9600"), QSerialPort::Baud9600);
    ui->baudRateBox->addItem(QStringLiteral("19200"), QSerialPort::Baud19200);
    ui->baudRateBox->addItem(QStringLiteral("38400"), QSerialPort::Baud38400);
    ui->baudRateBox->addItem(QStringLiteral("115200"), QSerialPort::Baud115200);

    ui->dataBitsBox->addItem(QStringLiteral("5"), QSerialPort::Data5);
    ui->dataBitsBox->addItem(QStringLiteral("6"), QSerialPort::Data6);
    ui->dataBitsBox->addItem(QStringLiteral("7"), QSerialPort::Data7);
    ui->dataBitsBox->addItem(QStringLiteral("8"), QSerialPort::Data8);
    ui->dataBitsBox->setCurrentIndex(3);

    ui->parityBox->addItem(tr("None"), QSerialPort::NoParity);
    ui->parityBox->addItem(tr("Even"), QSerialPort::EvenParity);
    ui->parityBox->addItem(tr("Odd"), QSerialPort::OddParity);
    ui->parityBox->addItem(tr("Mark"), QSerialPort::MarkParity);
    ui->parityBox->addItem(tr("Space"), QSerialPort::SpaceParity);

    ui->stopBitsBox->addItem(QStringLiteral("1"), QSerialPort::OneStop);
#ifdef Q_OS_WIN
    ui->stopBitsBox->addItem(tr("1.5"), QSerialPort::OneAndHalfStop);
#endif
    ui->stopBitsBox->addItem(QStringLiteral("2"), QSerialPort::TwoStop);

    ui->flowControlBox->addItem(tr("None"), QSerialPort::NoFlowControl);
    ui->flowControlBox->addItem(tr("RTS/CTS"), QSerialPort::HardwareControl);
    ui->flowControlBox->addItem(tr("XON/XOFF"), QSerialPort::SoftwareControl);
}

void MainWindow::enableConfigPortsParameters()
{
    ui->serialNumListBox->setEnabled(true);
    ui->baudRateBox->setEnabled(true);
    ui->dataBitsBox->setEnabled(true);
    ui->parityBox->setEnabled(true);
    ui->stopBitsBox->setEnabled(true);
    ui->flowControlBox->setEnabled(true);
    ui->refreshButton->setEnabled(true);
}

void MainWindow::disableConfigPortsParameters()
{
    ui->serialNumListBox->setDisabled(true);
    ui->baudRateBox->setDisabled(true);
    ui->dataBitsBox->setDisabled(true);
    ui->parityBox->setDisabled(true);
    ui->stopBitsBox->setDisabled(true);
    ui->flowControlBox->setDisabled(true);
    ui->refreshButton->setDisabled(true);
}

void MainWindow::fillPortsInfo()
{
    ui->serialNumListBox->clear();
//    QString description;
//    QString manufacturer;
//    QString serialNumber;
    foreach(const QSerialPortInfo &info,QSerialPortInfo::availablePorts()){
        QStringList list;
//        description = info.description();
//        manufacturer = info.manufacturer();
//        serialNumber = info.serialNumber();
        list << info.portName();
//             << (!description.isEmpty() ? description : "N/A")
//             << (!manufacturer.isEmpty() ? manufacturer : "N/A")
//             << (!serialNumber.isEmpty() ? serialNumber : "N/A")
//             << info.systemLocation()
//             << (info.vendorIdentifier() ? QString::number(info.vendorIdentifier(), 16) : "N/A")
//             << (info.productIdentifier() ? QString::number(info.productIdentifier(), 16) : "N/A");
//        qDebug()<<list.first();
        ui->serialNumListBox->addItem(list.first(),list);
    }
}
void MainWindow::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        QMessageBox::critical(this, tr("Critical Error"), serial->errorString());
    }
}
void MainWindow::openSerialPort()
{
    Settings p = updataSettings();
    if(serial->isOpen()){
        serial->close();
        ui->openButton->setText(tr("打开"));
        ui->statusBar->showMessage(tr("Disconnected"));
        enableConfigPortsParameters();
        if(fout.is_open()){
            fout.close();
        }
    }
    else{
        serial->setPortName(p.name);
        serial->setBaudRate(p.baudRate);
        serial->setDataBits(p.dataBits);
        serial->setParity(p.parity);
        serial->setStopBits(p.stopBits);
        serial->setFlowControl(p.flowControl);
        if(serial->open(QIODevice::ReadWrite)){
            ui->openButton->setText(tr("关闭"));
            disableConfigPortsParameters();
            ui->statusBar->showMessage(tr("Connected to %1 : %2, %3, %4, %5, %6")
                                       .arg(p.name).arg(p.stringBaudRate).arg(p.stringDataBits)
                                       .arg(p.stringParity).arg(p.stringStopBits).arg(p.stringFlowControl));
        }
        else{
            QMessageBox::critical(this, tr("Error"), serial->errorString());
            ui->statusBar->showMessage(tr("Open error"));
        }
        if(ui->saveFilecheckBox->isChecked()){
            fout.open(fileName.toLocal8Bit(),iostream::app);
            if(!fout.is_open()){
                QMessageBox::warning(this,"warn","open file error",QMessageBox::Ok);
                fout.close();
            }
        }
    }
//        ui->serialGroupBox->setDisabled(true);
}
void MainWindow::readData()
{
    QByteArray data = serial->readAll();
//    while (serial->waitForReadyRead(900))
//        data.append(serial->readAll());
    if(ui->hexRecvcheckBox->isChecked()){
        data = data.toHex();
    }
    ui->recvPlainTextEdit->insertPlainText(QString(data));//;//->appendPlainText(QString(data));
    ui->recvPlainTextEdit->setTextCursor(ui->recvPlainTextEdit->textCursor());
    if(ui->saveFilecheckBox->isChecked()){
        fout<<data.toStdString();
        fout<<flush;
    }

//    qDebug()<<QString(data.toHex());
//    QTextCodec *tc = QTextCodec::codecForName("GBK");
//    QString s = tc->toUnicode(data);
//    qDebug()<<s;
//    ui->recvPlainTextEdit->insertPlainText(s);
}
void MainWindow::sendData()
{
    QString msg = ui->sendTextEdit->toPlainText();
    serial->write(msg.toLocal8Bit());
}

void MainWindow::handleTimeout()
{

}

void MainWindow::clearRecvMsg()
{
    ui->recvPlainTextEdit->clear();
}
void MainWindow::clearSendMsg()
{
    ui->sendTextEdit->clear();
}

void MainWindow::saveFileName(bool flag)
{
    if(flag){
        fileName = QInputDialog::getText(this,"保存文件名称","输入文件名称",QLineEdit::Normal,"log");
    }
    fileName += ".txt";
//    qDebug()<<fileName;
}
