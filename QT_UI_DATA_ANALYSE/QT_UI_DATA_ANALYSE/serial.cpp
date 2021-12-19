#include "serial.h"
#include "ui_serial.h"

serial::serial(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::serial)
{
    ui->setupUi(this);
    //user code
    system_init();

}

serial::~serial()
{
    delete ui;
}
/* --------------------------------------------------------------------------------
 * function
 *  ------------------------------------------------------------------------------*/
void serial:: system_init()
{
    // port config
    global_port.setParity(QSerialPort::NoParity);//无奇偶校验
    global_port.setDataBits(QSerialPort::Data8);//数据位8位
    global_port.setStopBits(QSerialPort::OneStop);//1位停止位
    //connect
    connect(ui->btn_open,&QPushButton::clicked,this,serial::btn_open_port);
    connect(ui->btn_close,&QPushButton::clicked,this,serial::btn_close_port);
//    connect(ui->btn_send,&QPushButton::clicked,this,serial::btn_send_data);
//    connect(&global_port,&QSerialPort::readyRead,this, &receive_data);
    connect(&global_port,&QSerialPort::readyRead,this,&serial::start_serialport_read);
    connect(&myTimer,SIGNAL(timeout()),this,SLOT(receive_data()));


    connect(ui->btn_search,&QPushButton::clicked,this,serial::btn_search_port);
//    connect(ui->btn_clear,&QPushButton::clicked,this,serial::btn_clear_text);
//    connect(ui->btn_send_newline,&QPushButton::clicked,this,serial::btn_send_newline);
}

/* ---------------------------------------------------------------------------------
 * slots
 *  ------------------------------------------------------------------------------*/
void serial:: btn_open_port(bool)
{
//    qDebug()<<ui->cmb_port_name->currentIndex();
    /*--------------------port name -------------------------*/
    global_port.setPortName(ui->cmb_port_name->currentText());
    /*--------------------baud rate-------------------------*/
    global_port.setBaudRate(QSerialPort::Baud115200);
    //open port
    global_port.open(QIODevice::ReadWrite);//打开串口，读写模式
    //设置控件状态
    ui->lab_status->setText("串口已连接");
    ui->btn_open->setEnabled(false);
//    //test
//    global_port.write("hello");
}

/*---------------------send data-------------------------------*/
//void serial::btn_send_data(bool)
//{
//    QString data = ui->lint_send_data->text();//获取文本框内容
//    QByteArray array;
//    if(send_newline_flag == 1){
//        array =  (data.append("\r\n")).toLatin1(); //qstring --> qbytearray
//        global_port.write(array);
//    }
//    else{
//        array =  data.toLatin1(); //qstring --> qbytearray
//        global_port.write(array);
//    }

//}
/*--------------------receive data-----------------------------*/
void serial::receive_data()
{
//    array = global_port.readAll();
    myTimer.stop();
    if(read_flag==1)
    {
        QString StrI1=tr(array.mid(15,array.indexOf("\r")-16));
        dataI1=StrI1.toFloat()/10;//将字符串转换成float类型进行数据处理
        ui->textEdit->clear();
        ui->textEdit->setText(StrI1);
        qDebug()<<StrI1<<endl;
    }

    else if(read_flag==2)
    {
        QString StrI2=tr(array.mid(array.indexOf("x")+2,array.indexOf("y")-array.indexOf("x")-3));
        QString StrI3=tr(array.mid(array.indexOf("y")+2,array.indexOf("z")-array.indexOf("y")-3));
        QString StrI4=tr(array.mid(array.indexOf("z")+2,array.indexOf("\r")-array.indexOf("z")-2));

        dataI2=StrI2.toFloat()/10;
        dataI3=StrI3.toFloat()/10;
        dataI4=StrI4.toFloat()/10;

        ui->textEdit_2->clear();
        ui->textEdit_2->setText(StrI2);

        ui->textEdit_3->clear();
        ui->textEdit_3->setText(StrI3);

        ui->textEdit_4->clear();
        ui->textEdit_4->setText(StrI4);
        qDebug()<<StrI2<<" "<<StrI3<<" "<<StrI4<<endl;
    }

    array.clear();


}
/*-------------------close port---------------------------------*/
void serial::btn_close_port(bool)
{
    global_port.close();
    //设置控件状态
    ui->lab_status->setText("串口未连接");
    ui->cmb_port_name->setEnabled(true);
    ui->btn_open->setEnabled(true);
}
/*------------------search port----------------------------------*/
void serial::btn_search_port(bool)
{
    ui->cmb_port_name->clear();
    //通过QSerialPortInfo查找可用串口
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        ui->cmb_port_name->addItem(info.portName());
        //串口信息打印
//        qDebug() << "Name : " << info.portName();
//        qDebug() << "Description : " << info.description();
//        qDebug() << "Manufacturer: " << info.manufacturer();
//        qDebug() << "Serial Number: " << info.serialNumber();
//        qDebug() << "System Location: " << info.systemLocation();
    }

}
/*---------------clear text--------------------------------------*/
//void serial::btn_clear_text(bool)
//{
//    ui->plainTextEdit->clear();
//}
/*---------------send newline mode-------------------------------*/
//void serial::btn_send_newline(bool)
//{
//    send_newline_flag = 1-send_newline_flag;
//    if(send_newline_flag == 1){
//        ui->lab_mode->setText("换行模式");
//    }
//    else{
//        ui->lab_mode->setText("标准模式");
//    }
//}


void serial::start_serialport_read()
{
//  qDebug()<<"data_comming"<<endl;
  myTimer.start(30);
  array.append(global_port.readAll());

}

void serial::on_btn_send_clicked()
{
    QByteArray arrayy;
    arrayy =  "READ A MAX"; //qstring --> qbytearray
    global_port.write(arrayy);
    read_flag = 2;
}



void serial::on_btn_send_newline_clicked()
{
    QByteArray arrayx;
    arrayx =  "READ TIME ALL"; //qstring --> qbytearray
    global_port.write(arrayx);
    read_flag = 1;
}


void serial::data_analyse()
{
    int ca1,ca2,res;
    if(dataI1<2100)
    {
        ca1=100;
    }
    else
    {
        ca1=100-(dataI1-2100)/10;
    }
    ca2 = (dataI2+dataI3+dataI4)/12/3;
    res = (ca1+ca2)/2;

    if(res>=60)
    {
        ui->textEdit_5->setText("体能优秀");
    }

    else if(res<60&&res>=40)
    {
        ui->textEdit_5->setText("体能良好");
    }

    else if(res<40&&res>=20)
    {
        ui->textEdit_5->setText("体能及格");
    }
//    qDebug()<<ca1<<" "<<ca2<<" "<<res<<endl;

}

void serial::on_pushButton_clicked()
{
    data_analyse();
}
