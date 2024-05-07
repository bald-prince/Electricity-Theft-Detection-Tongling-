# 一.采集端

## 版本一

### stm32芯片型号
    
    stm32f103c8t6
### 采集芯片
    型号：BL0910
    
    通信协议：SPI协议

### 原理图及PCB
  文件名：8路测量-c8t6 V3.1.eprj
  
  绘制软件：立创eda专业版

### 采集数据类型
  电流、电压、频率

### 4G通信模块
  移远EC200U

### 服务器
  阿里云ECS + 阿里云物联网平台

### 工程文件
  MQTT_HAL_BL0910
  
  采用BL0910作为采集芯片，采集8路电流、电压信息，用作窃电行为判别数据支持，采用阿里云物联网平台对接EC200U设备，上传服务器。

## 版本二

stm32芯片型号：stm32f103c8t6

采集芯片信号：RN8302，SPI协议

采集数据：电流、电压、频率、有功功率、无功功率

4G通信模块：BC20

服务器：阿里云ECS + EMQX

BC20+EMQX+RN8302：采用RN8302作为采集芯片，采集三项电流、电压、频率、有功功率、无功功率、电量等参数，用作窃电行为判别，同时作为实验室内部采集数据使用

# 二.显示端

IMX6ULL-Show：嵌入式显示端用于显示采集端上传的数据，通过连接到EMQX服务器或者阿里云服务器，订阅话题的形式获取数据并解析JSON数据。
采用基于IMX6ULL的硬件平台，采用Qt框架采用c++进行开发。
