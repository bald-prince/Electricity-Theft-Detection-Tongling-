# 电力用户窃电检测与定位装置开发与应用
## 一.采集端

### 版本一

#### stm32芯片型号
    stm32f103c8t6
#### 采集芯片
    型号：BL0910
    通信协议：SPI协议
#### 原理图及PCB
    文件名：8路测量-c8t6 V3.1.eprj
    绘制软件：立创eda专业版
#### 采集数据类型
    电流、电压、频率
#### 4G通信模块
    移远EC200U
#### 服务器
    阿里云ECS + 阿里云物联网平台
#### 工程文件
    MQTT_HAL_BL0910
    采用BL0910作为采集芯片，采集8路电流、电压信息，用作窃电行为判别数据支持，采用阿里云物联网平台对接EC200U设备，上传服务器。

### 版本二
#### stm32芯片型号
    stm32f103c8t6
#### 采集芯片
    型号：RN8302
    通信协议：SPI协议
#### 原理图及PCB
    文件：BC20+RN8302.eprj
    绘制软件：立创eda专业版
#### 采集数据类型
    电流、电压、频率、有功功率、无功功率、电能
#### 4G通信模块
    移远BC20
#### 服务器
    阿里云ECS + EMQX
#### 工程文件
    BC20+EMQX+RN8302(裸机)/FreeRtos+BC20+EMQX+RN8302(带FreeRTos操作系统)
    采用RN8302作为采集芯片，采集三相电流、电压、频率、有功功率、无功功率、电量等参数，用作窃电行为判别，同时作为实验室内部采集数据使用
## 二.显示端
    electricty-theft：嵌入式显示端用于显示采集端上传的数据，通过连接到EMQX服务器或者阿里云服务器， 采用基于IMX6ULL的硬件平台，采用Qt框架采用c++进行开发。
    功能：
        1.订阅话题的形式获取数据并解析JSON数据，并以文本的形式和曲线的形式进行显示。
        2.连接数据库，并根据设备客户端id查询数据，并进行显示
        3.连接wifi，实现无线连接。
    使用方法：
        Qt Creator打开编译之后将编译后的可执行文件拷贝至硬件平台运行即可。
    注意事项：
        1.软键盘的使用，将electricty-theft中的dict整个文件夹拷贝至可执行文件的同目录，将libSoft-keyboard.so拷贝至qt安装目录下plugins/platforminputcontexts即可
        我的对应的是/usr/lib/plugins/platforminputcontexts。
        2.连接wifi的脚本alientek_usb_wifi_setup.sh根据自己的实际情况进行修改
        
        
   
    
    
