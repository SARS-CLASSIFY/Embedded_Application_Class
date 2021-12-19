#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

/*------------------------用户代码头文件---------------------------*/
#include <QtSerialPort/QSerialPort>//串口
#include <QtSerialPort/QSerialPortInfo>//串口
#include <QDebug>//用于在控制台输出调试信息
#include <QTime>//定时器
#include <QPainter>//坐标系绘图

#include <QTimer>


namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private slots:
    void on_btn_open_port_clicked();

    void on_btn_search_port_clicked();

    void AnalyzeData();//数据读取

    void setupPlot();//初始化


    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void start_serialport_read();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

private:
    Ui::Widget *ui;

    QSerialPort *myserial;//声明串口类，myserial是QSerialPort的实例
    bool serial_flag,start_flag;//定义两个标志位
    QByteArray alldata;//接收串口数据
    //绘图函数
    QDateTime mycurrenttime;//系统当前时间
    QDateTime mystarttime;//系统开始时间

    QTimer myTimer;// 定时器对象

    QByteArray mytemp;//串口接收缓存

    double xzb = 0;

};

#endif // WIDGET_H
