#include "widget.h"
#include "ui_widget.h"

static int x_label = 0;//设置横坐标初始值
//数据接收 帧头'T'帧尾'P'
static float max_data = 0;//数据最大值
static int connected_flag = 0;

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    this->setWindowTitle(QString("数据实时监测"));  //设置标题


    myserial = new QSerialPort();
    serial_flag = true;
    start_flag = true;


    setupPlot();//图形界面初始化函数
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_btn_open_port_clicked()
{
    if(serial_flag)
    {
        ui->comboBox->setDisabled(true); //禁止修改串口
        myserial->setPortName(ui->comboBox->currentText()); //设置串口号
        myserial->setBaudRate(QSerialPort::Baud115200); //设置波特
        myserial->setDataBits(QSerialPort::Data8); //设置数据位数
        myserial->setParity(QSerialPort::NoParity);//设置奇偶校验
        myserial->setStopBits(QSerialPort::OneStop);//设置停止位
        myserial->setFlowControl(QSerialPort::NoFlowControl);//非流控制
        if(myserial->open(QIODevice::ReadWrite))
        {
            if(connected_flag == 0){//避免重复打开
//                connect(myserial,&QSerialPort::readyRead,this,&Widget::AnalyzeData);
                connect(myserial,&QSerialPort::readyRead,this,&Widget::start_serialport_read);
                connect(&myTimer,SIGNAL(timeout()),this,SLOT(AnalyzeData()));
                connected_flag = 1;
            }

            mystarttime = QDateTime::currentDateTime();//图像横坐标初始值参考点，读取初始时间
            qDebug()<<"串口打开成功";
        }
        else
        {
            qDebug()<<"串口打开失败";
        }
        ui->btn_open_port->setText("关闭串口");
        serial_flag = false;//串口标志位置失效
    }
    else
    {
        ui->comboBox->setEnabled(true);//串口号下拉按钮使能工作
        myserial->close();
        ui->btn_open_port->setText("打开串口");//按钮显示“打开串口”
        serial_flag = true;//串口标志位置工作
    }
}


void Widget::on_btn_search_port_clicked()
{
    ui->comboBox->clear();
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())//读取串口信息
    {
        myserial->setPort(info);//这里相当于自动识别串口号之后添加到了cmb，如果要手动选择可以用下面列表的方式添加进去
        if(myserial->open(QIODevice::ReadWrite))
         {
          ui->comboBox->addItem(myserial->portName());//将串口号添加到cmb
          myserial->close();//关闭串口等待人为(打开串口按钮)打开
         }
     }
}


void Widget::AnalyzeData()
{
    myTimer.stop();
//    qDebug()<<"data_end"<<endl;
    if(mytemp.length()!=0)
    {
        qDebug()<<mytemp<<endl;
            QString StrI1=tr(mytemp.mid(mytemp.indexOf("A")+1,mytemp.indexOf("B")-mytemp.indexOf("A")-1));//自定义了简单协议，通过前面字母读取需要的数据
            if(sizeof(StrI1)>0)
            {
                //剩余的5条曲线添加
                QString StrI2=tr(mytemp.mid(mytemp.indexOf("C")+1,mytemp.indexOf("D")-mytemp.indexOf("C")-1));
                QString StrI3=tr(mytemp.mid(mytemp.indexOf("E")+1,mytemp.indexOf("F")-mytemp.indexOf("E")-1));
                QString StrI4=tr(mytemp.mid(mytemp.indexOf("G")+1,mytemp.indexOf("H")-mytemp.indexOf("G")-1));
                QString StrI5=tr(mytemp.mid(mytemp.indexOf("I")+1,mytemp.indexOf("J")-mytemp.indexOf("I")-1));
                QString StrI6=tr(mytemp.mid(mytemp.indexOf("K")+1,mytemp.indexOf("L")-mytemp.indexOf("K")-1));

                //文本框显示
                ui->line_ax->setText(StrI1);
                ui->line_ay->setText(StrI2);
                ui->line_az->setText(StrI3);
                ui->line_gx->setText(StrI4);
                ui->line_gy->setText(StrI5);
                ui->line_gz->setText(StrI6);

        //        if(dataI1>=max_data)   //得到数据最大值
        //        {
        //           max_data = dataI1;
        //        }
                mycurrenttime = QDateTime::currentDateTime();//获取系统时间


                float dataI1=StrI1.toFloat();//将字符串转换成float类型进行数据处理
                float dataI2=StrI2.toFloat();//将字符串转换成float类型进行数据处理
                float dataI3=StrI3.toFloat();//将字符串转换成float类型进行数据处理
                float dataI4=StrI4.toFloat();//将字符串转换成float类型进行数据处理
                float dataI5=StrI5.toFloat();//将字符串转换成float类型进行数据处理
                float dataI6=StrI6.toFloat();//将字符串转换成float类型进行数据处理



                if(x_label == 6001)             //到上限范围则清除图表
                {
                   ui->widget_plot->graph(0)->data().data()->clear();
                   ui->widget_plot->graph(1)->data().data()->clear();
                   ui->widget_plot->graph(2)->data().data()->clear();
                   ui->widget_plot->graph(3)->data().data()->clear();
                   ui->widget_plot->graph(4)->data().data()->clear();
                   ui->widget_plot->graph(5)->data().data()->clear();

                   //分立图表清空
                   ui->widget_plot_2->graph(0)->data().data()->clear();
                   ui->widget_plot_3->graph(0)->data().data()->clear();
                   ui->widget_plot_4->graph(0)->data().data()->clear();
                   ui->widget_plot_5->graph(0)->data().data()->clear();
                   ui->widget_plot_6->graph(0)->data().data()->clear();
                   ui->widget_plot_7->graph(0)->data().data()->clear();

                   ui->widget_plot->replot();

                   //replot
                   ui->widget_plot_2->replot();
                   ui->widget_plot_3->replot();
                   ui->widget_plot_4->replot();
                   ui->widget_plot_5->replot();
                   ui->widget_plot_6->replot();
                   ui->widget_plot_7->replot();

                   x_label = 0;
                   xzb=0;
                }


                //x范围自动改变
                if(xzb>30)
                {
                    ui->widget_plot->xAxis->setRange((xzb-30),xzb);//设定x轴的范围
                    ui->widget_plot_2->xAxis->setRange((xzb-30),xzb);//设定x轴的范围
                    ui->widget_plot_3->xAxis->setRange((xzb-30),xzb);//设定x轴的范围
                    ui->widget_plot_4->xAxis->setRange((xzb-30),xzb);//设定x轴的范围
                    ui->widget_plot_5->xAxis->setRange((xzb-30),xzb);//设定x轴的范围
                    ui->widget_plot_6->xAxis->setRange((xzb-30),xzb);//设定x轴的范围
                    ui->widget_plot_7->xAxis->setRange((xzb-30),xzb);//设定x轴的范围



                    xzb++;
                }
                else
                {
                    xzb++;
                }

                ui->widget_plot->graph(0)->addData(x_label,dataI1);//添加数据1到曲线1
                ui->widget_plot->graph(1)->addData(x_label,dataI2);//添加数据2到曲线2
                ui->widget_plot->graph(2)->addData(x_label,dataI3);//添加数据3到曲线3
                ui->widget_plot->graph(3)->addData(x_label,dataI4);//添加数据4到曲线4
                ui->widget_plot->graph(4)->addData(x_label,dataI5);//添加数据5到曲线5
                ui->widget_plot->graph(5)->addData(x_label,dataI6);//添加数据6到曲线6

                //分立图表数据添加
                ui->widget_plot_2->graph(0)->addData(x_label,dataI1);//添加数据1到曲线1
                ui->widget_plot_3->graph(0)->addData(x_label,dataI2);//添加数据1到曲线1
                ui->widget_plot_4->graph(0)->addData(x_label,dataI3);//添加数据1到曲线1
                ui->widget_plot_5->graph(0)->addData(x_label,dataI4);//添加数据1到曲线1
                ui->widget_plot_6->graph(0)->addData(x_label,dataI5);//添加数据1到曲线1
                ui->widget_plot_7->graph(0)->addData(x_label,dataI6);//添加数据1到曲线1

                x_label++;


                ui->widget_plot->replot();//每次画完曲线一定要更新显示
                //replot
                ui->widget_plot_2->replot();
                ui->widget_plot_3->replot();
                ui->widget_plot_4->replot();
                ui->widget_plot_5->replot();
                ui->widget_plot_6->replot();
                ui->widget_plot_7->replot();

            }
    }
    mytemp.clear();
}

void Widget::setupPlot()
{
    //曲线1
    ui->widget_plot->addGraph();//添加一条曲线
    QPen pen;
    pen.setWidth(1);//设置画笔线条宽度
    pen.setColor(Qt::blue);


    ui->widget_plot->graph(0)->setPen(pen);//设置画笔颜色
    ui->widget_plot->graph(0)->setBrush(QBrush(QColor(0, 0, 255, 20))); //设置曲线画刷背景
    ui->widget_plot->graph(0)->setName("ax");                        //图例1名称
    ui->widget_plot->graph(0)->setAntialiasedFill(false);
    ui->widget_plot->graph(0)->setLineStyle((QCPGraph::LineStyle)1);//曲线画笔
    ui->widget_plot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssNone,5));//曲线形状

    //曲线2
    ui->widget_plot->addGraph();
    pen.setColor(Qt::red);
    ui->widget_plot->graph(1)->setPen(pen);//设置画笔颜色
    ui->widget_plot->graph(1)->setBrush(QBrush(QColor(0, 0, 255, 20))); //设置曲线画刷背景
    ui->widget_plot->graph(1)->setName("ay");                        //图例2名称
    ui->widget_plot->graph(1)->setAntialiasedFill(false);
    ui->widget_plot->graph(1)->setLineStyle((QCPGraph::LineStyle)1);//曲线画笔
    ui->widget_plot->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssNone,5));//曲线形状

    //曲线3
    ui->widget_plot->addGraph();//添加一条曲线
    pen.setColor(Qt::green);
    ui->widget_plot->graph(2)->setPen(pen);//设置画笔颜色
    ui->widget_plot->graph(2)->setBrush(QBrush(QColor(0, 0, 255, 20))); //设置曲线画刷背景
    ui->widget_plot->graph(2)->setName("az");                        //图例1名称
    ui->widget_plot->graph(2)->setAntialiasedFill(false);
    ui->widget_plot->graph(2)->setLineStyle((QCPGraph::LineStyle)1);//曲线画笔
    ui->widget_plot->graph(2)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssNone,5));//曲线形状

    //曲线4
    ui->widget_plot->addGraph();
    pen.setColor(Qt::gray);
    ui->widget_plot->graph(3)->setPen(pen);//设置画笔颜色
    ui->widget_plot->graph(3)->setBrush(QBrush(QColor(0, 0, 255, 20))); //设置曲线画刷背景
    ui->widget_plot->graph(3)->setName("gx");                        //图例2名称
    ui->widget_plot->graph(3)->setAntialiasedFill(false);
    ui->widget_plot->graph(3)->setLineStyle((QCPGraph::LineStyle)1);//曲线画笔
    ui->widget_plot->graph(3)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssNone,5));//曲线形状


    //曲线5
    ui->widget_plot->addGraph();//添加一条曲线
    pen.setColor(Qt::black);
    ui->widget_plot->graph(4)->setPen(pen);//设置画笔颜色
    ui->widget_plot->graph(4)->setBrush(QBrush(QColor(0, 0, 255, 20))); //设置曲线画刷背景
    ui->widget_plot->graph(4)->setName("gy");                        //图例1名称
    ui->widget_plot->graph(4)->setAntialiasedFill(false);
    ui->widget_plot->graph(4)->setLineStyle((QCPGraph::LineStyle)1);//曲线画笔
    ui->widget_plot->graph(4)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssNone,5));//曲线形状

    //曲线6
    ui->widget_plot->addGraph();
    pen.setColor(Qt::yellow);
    ui->widget_plot->graph(5)->setPen(pen);//设置画笔颜色
    ui->widget_plot->graph(5)->setBrush(QBrush(QColor(0, 0, 255, 20))); //设置曲线画刷背景
    ui->widget_plot->graph(5)->setName("gz");                        //图例2名称
    ui->widget_plot->graph(5)->setAntialiasedFill(false);
    ui->widget_plot->graph(5)->setLineStyle((QCPGraph::LineStyle)1);//曲线画笔
    ui->widget_plot->graph(5)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssNone,5));//曲线形状


    //设置图表
    ui->widget_plot->xAxis->setLabel(QStringLiteral("时间/50ms"));//设置x坐标轴名称
    ui->widget_plot->xAxis->setLabelColor(QColor(20,20,20));//设置x坐标轴名称颜色

    ui->widget_plot->xAxis->setRange(0,30);//设定x轴的范围

    ui->widget_plot->yAxis->setLabel(QStringLiteral("fax/fay/faz gx/gy/gz(*10)"));//设置y坐标轴名称
    ui->widget_plot->yAxis->setLabelColor(QColor(20,20,20));//设置y坐标轴名称颜色

    ui->widget_plot->yAxis->setRange(-12000,12000);//设定y轴范围

    ui->widget_plot->axisRect()->setupFullAxesBox(true);//设置缩放，拖拽，设置图表的分类图标显示位置
    ui->widget_plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom| QCP::iSelectAxes);
    ui->widget_plot->axisRect()->insetLayout()->setInsetAlignment(0,Qt::AlignTop | Qt::AlignRight);//图例显示位置右上
    ui->widget_plot->legend->setVisible(true);//显示图例



    ui->widget_plot->replot();

    /*------------设置分立图表-------------------*/

    pen.setWidth(3);//设置画笔线条宽度

    ui->widget_plot_2->addGraph();//添加一条曲线
    ui->widget_plot_3->addGraph();//添加一条曲线
    ui->widget_plot_4->addGraph();//添加一条曲线
    ui->widget_plot_5->addGraph();//添加一条曲线
    ui->widget_plot_6->addGraph();//添加一条曲线
    ui->widget_plot_7->addGraph();//添加一条曲线

    pen.setColor(Qt::blue);
    ui->widget_plot_2->graph(0)->setPen(pen);//设置画笔颜色
    ui->widget_plot_2->graph(0)->setBrush(QBrush(QColor(0, 0, 255, 20))); //设置曲线画刷背景
    ui->widget_plot_2->graph(0)->setName("ax");                        //图例1名称
    ui->widget_plot_2->graph(0)->setAntialiasedFill(false);
    ui->widget_plot_2->graph(0)->setLineStyle((QCPGraph::LineStyle)1);//曲线画笔
    ui->widget_plot_2->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssNone,5));//曲线形状

    pen.setColor(Qt::red);
    ui->widget_plot_3->graph(0)->setPen(pen);//设置画笔颜色
    ui->widget_plot_3->graph(0)->setBrush(QBrush(QColor(0, 0, 255, 20))); //设置曲线画刷背景
    ui->widget_plot_3->graph(0)->setName("ay");                        //图例1名称
    ui->widget_plot_3->graph(0)->setAntialiasedFill(false);
    ui->widget_plot_3->graph(0)->setLineStyle((QCPGraph::LineStyle)1);//曲线画笔
    ui->widget_plot_3->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssNone,5));//曲线形状

    pen.setColor(Qt::green);
    ui->widget_plot_4->graph(0)->setPen(pen);//设置画笔颜色
    ui->widget_plot_4->graph(0)->setBrush(QBrush(QColor(0, 0, 255, 20))); //设置曲线画刷背景
    ui->widget_plot_4->graph(0)->setName("az");                        //图例1名称
    ui->widget_plot_4->graph(0)->setAntialiasedFill(false);
    ui->widget_plot_4->graph(0)->setLineStyle((QCPGraph::LineStyle)1);//曲线画笔
    ui->widget_plot_4->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssNone,5));//曲线形状

    pen.setColor(Qt::gray);
    ui->widget_plot_5->graph(0)->setPen(pen);//设置画笔颜色
    ui->widget_plot_5->graph(0)->setBrush(QBrush(QColor(0, 0, 255, 20))); //设置曲线画刷背景
    ui->widget_plot_5->graph(0)->setName("gx");                        //图例1名称
    ui->widget_plot_5->graph(0)->setAntialiasedFill(false);
    ui->widget_plot_5->graph(0)->setLineStyle((QCPGraph::LineStyle)1);//曲线画笔
    ui->widget_plot_5->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssNone,5));//曲线形状

    pen.setColor(Qt::black);
    ui->widget_plot_6->graph(0)->setPen(pen);//设置画笔颜色
    ui->widget_plot_6->graph(0)->setBrush(QBrush(QColor(0, 0, 255, 20))); //设置曲线画刷背景
    ui->widget_plot_6->graph(0)->setName("gy");                        //图例1名称
    ui->widget_plot_6->graph(0)->setAntialiasedFill(false);
    ui->widget_plot_6->graph(0)->setLineStyle((QCPGraph::LineStyle)1);//曲线画笔
    ui->widget_plot_6->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssNone,5));//曲线形状


    pen.setColor(Qt::yellow);
    ui->widget_plot_7->graph(0)->setPen(pen);//设置画笔颜色
    ui->widget_plot_7->graph(0)->setBrush(QBrush(QColor(0, 0, 255, 20))); //设置曲线画刷背景
    ui->widget_plot_7->graph(0)->setName("gz");                        //图例1名称
    ui->widget_plot_7->graph(0)->setAntialiasedFill(false);
    ui->widget_plot_7->graph(0)->setLineStyle((QCPGraph::LineStyle)1);//曲线画笔
    ui->widget_plot_7->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssNone,5));//曲线形状

    //设置样式
    ui->widget_plot_2->xAxis->setLabel(QStringLiteral("时间/50ms"));//设置x坐标轴名称
    ui->widget_plot_2->xAxis->setLabelColor(QColor(20,20,20));//设置x坐标轴名称颜色
    ui->widget_plot_2->xAxis->setRange(0,30);//设定x轴的范围
    ui->widget_plot_2->yAxis->setLabel(QStringLiteral("ax(*10)"));//设置y坐标轴名称
    ui->widget_plot_2->yAxis->setLabelColor(QColor(20,20,20));//设置y坐标轴名称颜色
    ui->widget_plot_2->yAxis->setRange(-12000,12000);//设定y轴范围
    ui->widget_plot_2->axisRect()->setupFullAxesBox(true);//设置缩放，拖拽，设置图表的分类图标显示位置
    ui->widget_plot_2->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom| QCP::iSelectAxes);
//    ui->widget_plot_2->axisRect()->insetLayout()->setInsetAlignment(0,Qt::AlignTop | Qt::AlignRight);//图例显示位置右上
//    ui->widget_plot_2->legend->setVisible(true);//显示图例

    ui->widget_plot_3->xAxis->setLabel(QStringLiteral("时间/50ms"));//设置x坐标轴名称
    ui->widget_plot_3->xAxis->setLabelColor(QColor(20,20,20));//设置x坐标轴名称颜色
    ui->widget_plot_3->xAxis->setRange(0,30);//设定x轴的范围
    ui->widget_plot_3->yAxis->setLabel(QStringLiteral("ay(*10)"));//设置y坐标轴名称
    ui->widget_plot_3->yAxis->setLabelColor(QColor(20,20,20));//设置y坐标轴名称颜色
    ui->widget_plot_3->yAxis->setRange(-12000,12000);//设定y轴范围
    ui->widget_plot_3->axisRect()->setupFullAxesBox(true);//设置缩放，拖拽，设置图表的分类图标显示位置
    ui->widget_plot_3->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom| QCP::iSelectAxes);
//    ui->widget_plot_3->axisRect()->insetLayout()->setInsetAlignment(0,Qt::AlignTop | Qt::AlignRight);//图例显示位置右上
//    ui->widget_plot_3->legend->setVisible(true);//显示图例

    ui->widget_plot_4->xAxis->setLabel(QStringLiteral("时间/50ms"));//设置x坐标轴名称
    ui->widget_plot_4->xAxis->setLabelColor(QColor(20,20,20));//设置x坐标轴名称颜色
    ui->widget_plot_4->xAxis->setRange(0,30);//设定x轴的范围
    ui->widget_plot_4->yAxis->setLabel(QStringLiteral("az(*10)"));//设置y坐标轴名称
    ui->widget_plot_4->yAxis->setLabelColor(QColor(20,20,20));//设置y坐标轴名称颜色
    ui->widget_plot_4->yAxis->setRange(-12000,12000);//设定y轴范围
    ui->widget_plot_4->axisRect()->setupFullAxesBox(true);//设置缩放，拖拽，设置图表的分类图标显示位置
    ui->widget_plot_4->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom| QCP::iSelectAxes);
//    ui->widget_plot_4->axisRect()->insetLayout()->setInsetAlignment(0,Qt::AlignTop | Qt::AlignRight);//图例显示位置右上
//    ui->widget_plot_4->legend->setVisible(true);//显示图例

    ui->widget_plot_5->xAxis->setLabel(QStringLiteral("时间/50ms"));//设置x坐标轴名称
    ui->widget_plot_5->xAxis->setLabelColor(QColor(20,20,20));//设置x坐标轴名称颜色
    ui->widget_plot_5->xAxis->setRange(0,30);//设定x轴的范围
    ui->widget_plot_5->yAxis->setLabel(QStringLiteral("gx(*10)"));//设置y坐标轴名称
    ui->widget_plot_5->yAxis->setLabelColor(QColor(20,20,20));//设置y坐标轴名称颜色
    ui->widget_plot_5->yAxis->setRange(-12000,12000);//设定y轴范围
    ui->widget_plot_5->axisRect()->setupFullAxesBox(true);//设置缩放，拖拽，设置图表的分类图标显示位置
    ui->widget_plot_5->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom| QCP::iSelectAxes);
//    ui->widget_plot_5->axisRect()->insetLayout()->setInsetAlignment(0,Qt::AlignTop | Qt::AlignRight);//图例显示位置右上
//    ui->widget_plot_5->legend->setVisible(true);//显示图例

    ui->widget_plot_6->xAxis->setLabel(QStringLiteral("时间/50ms"));//设置x坐标轴名称
    ui->widget_plot_6->xAxis->setLabelColor(QColor(20,20,20));//设置x坐标轴名称颜色
    ui->widget_plot_6->xAxis->setRange(0,30);//设定x轴的范围
    ui->widget_plot_6->yAxis->setLabel(QStringLiteral("gy(*10)"));//设置y坐标轴名称
    ui->widget_plot_6->yAxis->setLabelColor(QColor(20,20,20));//设置y坐标轴名称颜色
    ui->widget_plot_6->yAxis->setRange(-12000,12000);//设定y轴范围
    ui->widget_plot_6->axisRect()->setupFullAxesBox(true);//设置缩放，拖拽，设置图表的分类图标显示位置
    ui->widget_plot_6->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom| QCP::iSelectAxes);
//    ui->widget_plot_6->axisRect()->insetLayout()->setInsetAlignment(0,Qt::AlignTop | Qt::AlignRight);//图例显示位置右上
//    ui->widget_plot_6->legend->setVisible(true);//显示图例

    ui->widget_plot_7->xAxis->setLabel(QStringLiteral("时间/50ms"));//设置x坐标轴名称
    ui->widget_plot_7->xAxis->setLabelColor(QColor(20,20,20));//设置x坐标轴名称颜色
    ui->widget_plot_7->xAxis->setRange(0,30);//设定x轴的范围
    ui->widget_plot_7->yAxis->setLabel(QStringLiteral("gz(*10)"));//设置y坐标轴名称
    ui->widget_plot_7->yAxis->setLabelColor(QColor(20,20,20));//设置y坐标轴名称颜色
    ui->widget_plot_7->yAxis->setRange(-12000,12000);//设定y轴范围
    ui->widget_plot_7->axisRect()->setupFullAxesBox(true);//设置缩放，拖拽，设置图表的分类图标显示位置
    ui->widget_plot_7->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom| QCP::iSelectAxes);
//    ui->widget_plot_7->axisRect()->insetLayout()->setInsetAlignment(0,Qt::AlignTop | Qt::AlignRight);//图例显示位置右上
//    ui->widget_plot_7->legend->setVisible(true);//显示图例

    //replot
    ui->widget_plot_2->replot();
    ui->widget_plot_3->replot();
    ui->widget_plot_4->replot();
    ui->widget_plot_5->replot();
    ui->widget_plot_6->replot();
    ui->widget_plot_7->replot();
}

void Widget::on_pushButton_clicked()//退出按钮点击函数
{
    QApplication* app;
    app->quit();
}

void Widget::on_pushButton_2_clicked()
{
    ui->widget_plot->graph(0)->data().data()->clear();

    ui->widget_plot->graph(1)->data().data()->clear();

    ui->widget_plot->graph(2)->data().data()->clear();

    ui->widget_plot->graph(3)->data().data()->clear();

    ui->widget_plot->graph(4)->data().data()->clear();

    ui->widget_plot->graph(5)->data().data()->clear();

    //分立图表清空
    ui->widget_plot_2->graph(0)->data().data()->clear();
    ui->widget_plot_3->graph(0)->data().data()->clear();
    ui->widget_plot_4->graph(0)->data().data()->clear();
    ui->widget_plot_5->graph(0)->data().data()->clear();
    ui->widget_plot_6->graph(0)->data().data()->clear();
    ui->widget_plot_7->graph(0)->data().data()->clear();

    ui->widget_plot->replot();
    ui->widget_plot_2->replot();
    ui->widget_plot_3->replot();
    ui->widget_plot_4->replot();
    ui->widget_plot_5->replot();
    ui->widget_plot_6->replot();
    ui->widget_plot_7->replot();


    x_label = 0;
    max_data = 0;
    xzb = 0;

    ui->widget_plot->yAxis->setRange(-12000,12000);//设定y轴范围
    ui->widget_plot_2->yAxis->setRange(-12000,12000);//设定y轴范围
    ui->widget_plot_3->yAxis->setRange(-12000,12000);//设定y轴范围
    ui->widget_plot_4->yAxis->setRange(-12000,12000);//设定y轴范围
    ui->widget_plot_5->yAxis->setRange(-12000,12000);//设定y轴范围
    ui->widget_plot_6->yAxis->setRange(-12000,12000);//设定y轴范围
    ui->widget_plot_7->yAxis->setRange(-12000,12000);//设定y轴范围

    ui->widget_plot->xAxis->setRange(0,30);//设定y轴范围
    ui->widget_plot_2->xAxis->setRange(0,30);//设定y轴范围
    ui->widget_plot_3->xAxis->setRange(0,30);//设定y轴范围
    ui->widget_plot_4->xAxis->setRange(0,30);//设定y轴范围
    ui->widget_plot_5->xAxis->setRange(0,30);//设定y轴范围
    ui->widget_plot_6->xAxis->setRange(0,30);//设定y轴范围
    ui->widget_plot_7->xAxis->setRange(0,30);//设定y轴范围

}

/*--------------------------------------
 * 串口数据读取延时
 * -----------------------------------*/
void Widget::start_serialport_read()
{
//  qDebug()<<"data_comming"<<endl;
  myTimer.start(20);
  mytemp.append(myserial->readAll());

}

void Widget::on_pushButton_3_clicked()
{
    ui->widget_plot->yAxis->setRange(-12000,12000);//设定y轴范围
    ui->widget_plot_2->yAxis->setRange(-12000,12000);//设定y轴范围
    ui->widget_plot_3->yAxis->setRange(-12000,12000);//设定y轴范围
    ui->widget_plot_4->yAxis->setRange(-12000,12000);//设定y轴范围
    ui->widget_plot_5->yAxis->setRange(-12000,12000);//设定y轴范围
    ui->widget_plot_6->yAxis->setRange(-12000,12000);//设定y轴范围
    ui->widget_plot_7->yAxis->setRange(-12000,12000);//设定y轴范围

    ui->widget_plot->replot();
    ui->widget_plot_2->replot();
    ui->widget_plot_3->replot();
    ui->widget_plot_4->replot();
    ui->widget_plot_5->replot();
    ui->widget_plot_6->replot();
    ui->widget_plot_7->replot();
}



void Widget::on_pushButton_4_clicked()
{
    QString data = "CHANGE DATA";//获取文本框内容
    QByteArray array;
    array =  (data).toLatin1(); //qstring --> qbytearray
    myserial->write(array);

}
