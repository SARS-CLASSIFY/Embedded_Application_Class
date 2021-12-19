#ifndef SERIAL_H
#define SERIAL_H

#include <QMainWindow>

/*---------------- user --------------------*/
//port
#include <QSerialPort>
#include <QSerialPortInfo>
//debug
#include <QDebug>



#include <QTimer>
/*------------------------------------------*/
namespace Ui {
class serial;
}

class serial : public QMainWindow
{
    Q_OBJECT

public:
    explicit serial(QWidget *parent = 0);
    ~serial();

private slots:
    //button
    void btn_open_port(bool);//开启串口按钮响应
//    void btn_send_data(bool);
    void btn_close_port(bool);
    void btn_search_port(bool);
//    void btn_clear_text(bool);
//    void btn_send_newline(bool);
    //receive data
    void receive_data();


    void on_btn_send_clicked();


    void on_btn_send_newline_clicked();

    void start_serialport_read();

    void on_pushButton_clicked();

    void data_analyse();

private:
    Ui::serial *ui;
    /*-----------function-----------------------*/
    void system_init();
    /*-----------variable-----------------------*/
    QSerialPort global_port;
    uint8_t send_newline_flag = 0;

    uint8_t read_flag = 0;

    QByteArray array ;

    QTimer myTimer;// 定时器对象
    float dataI1;
    float dataI2;
    float dataI3;
    float dataI4;
};

#endif // SERIAL_H
