import socket
import network
import time


host='192.168.43.172'
port = 5683
SSID="pradeepraja"
PASSWORD="Yamuna@03"
wlan=None
s=None

def connectWifi(ssid,passwd):
  global wlan
  wlan=network.WLAN(network.STA_IF)
  wlan.active(True)
  wlan.disconnect()
  wlan.connect(ssid,passwd)
  while(wlan.ifconfig()[0]=='0.0.0.0'):
    time.sleep(1)
  return True
try:
  if(connectWifi(SSID,PASSWORD) == True):
    s=socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.setsockopt(socket.SOL_SOCKET,socket.SO_REUSEADDR,1)  
    ip=wlan.ifconfig()[0]
    while True:
      s.sendto(b'hii\r\n',(host,port))
      time.sleep(1)
except:
  if (s):
    s.close()
  wlan.disconnect()
  wlan.active(False)

