#!/bin/sh
export PATH=$PATH:/home/root/shell/wifi

#正点原子@ALIENTEK 
#USB WIFI脚本 
#功能:脚本支持station模式、softap模式、bridge模式相互切换（若相互切换不成功，请重启板子）

#使用方法说明
usage_and_exit() {
	echo ""
	echo "*****************************************************************************************"
	echo "*usage: ${0##*/} [-m mode] [-i ssid] [-p psk] [-d device] [-e ethernet] [-h]*"
	echo "*                                                                                       *"
	echo "*station mode eg:${0##*/} -m station -i ssid -p psk -d wlan0                *"
	echo "*                                                                                       *"
	echo "*softap mode eg: ${0##*/} -m softap -d wlan0                                *"
	echo "*                                                                                       *"
	echo "*bridge mode eg:${0##*/} -m bridge -d wlan0 -e eth0                         *"
	echo "*eg: ${0##*/} -m mode mode=[ station | softap | bridge ]                    *" 
	echo "*****************************************************************************************"
	echo ""
    return
}

#使用方法举例：
#source ./alientek_usb_wifi_setup.sh -m bridge -d wlan0 -e eth0
#source ./alientek_usb_wifi_setup.sh -m station -i ALIENTEK -p 15902020353 -d wlan0
#source ./alientek_usb_wifi_setup.sh -m softap -d wlan0


#版本信息
version() {
	echo "version 1.1"
	echo "作者:DZM@ALIENTEK"
}

#获取参数
while [ $# -gt 0 ]; do
  case $1 in
    --help | -h)
      usage_and_exit $0;break;;
    -m) shift; mode=$1; shift; ;;
    -i) shift; ssid=$1; shift; ;;
	-p) shift; psk=$1; shift; ;;
	-d) shift; device=$1; shift; ;;
	-e) shift; ethernet=$1; shift; ;;
    --version) version $0; break;;
    *) usage_and_exit; break; ;;
  esac
done

#判断参数
if [ -n "$mode" ]; then
echo "您的WIFI配置信息是："
echo "mode  : $mode"
fi

if [ -n "$ssid" ]; then
echo "ssid  : $ssid"
echo "psk   : $psk"
fi

if [ -n "$device" ]; then
echo "device: $device"
fi

if [ -n "$ethernet" ]; then
echo "ethernet  : $ethernet"
fi

#kill掉相关进程
processkill()
{
a=$(ps -aux |grep -E "hostapd" | grep -v grep | awk '{print $2}')
if [ -n "$a" ];then
	kill -9 $a
fi

a=$(ps -aux |grep -E "hostapd-rtl871xdrv" | grep -v grep | awk '{print $2}')
if [ -n "$a" ];then
	kill -9 $a
fi

a=$(ps -aux |grep -E "udhcpd" | grep -v grep  | awk '{print $2}')
if [ -n "$a" ];then
	kill -9 $a
fi

a=$(ps -aux |grep -E "wpa_supplicant" | grep -v grep | awk '{print $2}')
if [ -n "$a" ];then
	kill -9 $a
fi

a=$(ps -aux |grep -E "udhcpc" | grep -v grep | awk '{print $2}')
if [ -n "$a" ];then
	kill -9 $a
fi
}

#创建配置wifi的信息
touch /$PWD/wifi.conf
wifi_conf="/$PWD/wifi.conf"

#bridge Mode(桥接模式)
if [ "$mode" == "bridge" ]; then 
	ifconfig $device down
	ifconfig $device up
	ifconfig $ethernet down
	ifconfig $ethernet up 
	sleep 2
	if [ "$WIFI_MODE" == "station" ]; then 
		wpa_supplicant -B -D wext -i $device -c /etc/wpa_supplicant.conf
	fi
	processkill
	sleep 1
	sync
    echo -ne "interface=wlan0\nssid=alientek_bridge\ndriver=rtl871xdrv\nchannel=6\nhw_mode=g\nignore_broadcast_ssid=0\n
auth_algs=1\nwpa=3\nwpa_passphrase=12345678\nwpa_key_mgmt=WPA-PSK\nwpa_pairwise=TKIP\nrsn_pairwise=CCMP\nbridge=br0" > $wifi_conf
	rm -rf /var/lib/misc/*
	touch /var/lib/misc/udhcpd.leases
	udhcpd -fS /etc/udhcpd.conf & 
	ifconfig $device 0.0.0.0
	brctl addbr br0       
	ifconfig $ethernet 0.0.0.0
	brctl addif br0 $ethernet 
	brctl addif br0 $device         
	ifconfig br0 192.168.1.39 netmask 255.255.255.0
	hostapd-rtl871xdrv $wifi_conf -B
	export WIFI_MODE=bridge
fi

#softap Mode（热点模式）
if [ "$mode" == "softap" ]; then
	processkill
	sleep 1
	sync
    echo -ne "interface=wlan0\nssid=alientek_softap\ndriver=rtl871xdrv\nchannel=6\nhw_mode=g\nignore_broadcast_ssid=0\n
auth_algs=1\nwpa=3\nwpa_passphrase=12345678\nwpa_key_mgmt=WPA-PSK\nwpa_pairwise=TKIP\nrsn_pairwise=CCMP" > $wifi_conf
	a=$(ifconfig |grep -E "br0" | grep -v grep | awk '{print $0}')
	if [ -n "$a" ];then
		brctl delif br0 $device
		brctl delif br0 $device
		ifconfig br0 down
	fi 	
	rm -rf /var/lib/misc/*
	touch /var/lib/misc/udhcpd.leases
	ifconfig $device up
	sleep 2
	ifconfig $device 192.168.1.38 netmask 255.255.255.0
	udhcpd -fS /etc/udhcpd.conf     &
	hostapd-rtl871xdrv $wifi_conf -B
	export WIFI_MODE=softap
fi

#station Mode（上网模式）
if [ "$mode" == "station" ]; then 
	processkill
	sleep 1
	sync
	a=$(ifconfig |grep -E "br0" | grep -v grep | awk '{print $0}')
	if [ -n "$a" ];then
		brctl delif br0 $device
		brctl delif br0 $device
		ifconfig br0 down
	fi 
    echo -ne "ctrl_interface=/var/run/wpa_supplicant\n
update_config=1\nnetwork={\nssid=\"$ssid\"\npsk=\"$psk\"\n}\n" > $wifi_conf
	rm -rf /var/lib/misc/*
	ifconfig eth0 down
	ifconfig eth1 down
	ifconfig wlan0 down
	ifconfig wlan0 up
	sleep 2
	wpa_supplicant -B -D wext -i $device -c $wifi_conf
	udhcpc -R -b -i wlan0
	route add default gw 192.168.1.1 dev $device
	export WIFI_MODE=station
fi

#删除wifi.conf
rm -rf /$PWD/wifi.conf
case $mode in
    softap|station|bridge)echo "WIFI设置$mode模式完成!"
    ;;

esac

#卸载环境变量
unset device
unset wifi_conf
unset a
unset ethernet
unset mode
unset ssid
unset psk

sync
