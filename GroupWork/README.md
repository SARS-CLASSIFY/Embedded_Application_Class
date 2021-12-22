# GROUP WORK(大作业)

- 实现基础的秒表功能KEY5 KEY6停止开始计时

- 实现总时长FLASH存储，最大100条数据

- 实现MUP6050六轴传感器数据读取并50ms为间隔的记录，存至FLASH中

- 自主编写QT上位机实现六轴曲线绘制

- 编写上位机实现体能自动分析

- HC05蓝牙串口透传功能实现

- 串口数据获取指令

  ```c
  CHANGE DATA //读上位机数据格式
  READ A MAX //读最大加速度a
  READ TIME ALL //读所有记录用时长度
  CLEAR ALL DATA //清除所有数据
  READ AD DATA //读采集的数据
  READ RECORD  //读记录
  ```

- 上位机数据接收格式：Af%dBC%dyDE%dzFG%dHI%dJK%dL,ax,ay,az,gx,gy,gz

