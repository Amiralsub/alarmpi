import time
import os
import subprocess
import socket
import smtplib
from email.mime.text import MIMEText
# lancement
ip= subprocess.getoutput("ifconfig wlan0 |grep inet | awk '{print $2}'")
subject = "reboot du orangepi"
body = "le orangepi vient de rebooter. IP:"+ip
sender = "gfgh@gmail.com"
recipients = ["hfhgf@gmail.com", "oliv_ren@yahoo.fr"]
password = "hgfhgfhghgf"

# Signal: 'INT','PA','SET','ABORT'
switches = [74, 75, 79, 230]
states = [0, 0, 0, 0]
oldMsg = ''


def send_email(subject, body, sender, recipients, password):
    msg = MIMEText(body)
    msg['Subject'] = subject
    msg['From'] = sender
    msg['To'] = ', '.join(recipients)
    with smtplib.SMTP_SSL('smtp.gmail.com', 465) as smtp_server:
       smtp_server.login(sender, password)
       smtp_server.sendmail(sender, recipients, msg.as_string())
    print("Message sent!")

def initialisation_gpio(switch):
     os.system("echo "+ str(switch) +"> /sys/class/gpio/export")
     os.system("echo in> /sys/class/gpio/gpio"+ str(switch) +"/direction")

for i in (switches):
    initialisation_gpio(i)

send_email(subject, body, sender, recipients, password)

while 1:
  for i in range(0, len(switches)):
    #states[i] = GPIO.input(switches[i])
    states[i] = subprocess.getoutput("cat /sys/class/gpio/gpio"+str(switches[i])+"/value")
    # ore : trace à supprimer
    print("etat de gpio"+str(switches[i])+":"+states[i])
  if states[0] == 1:
    newMsg = 'ALARM!'
  elif states[1] == 0:
    newMsg = 'PANIC!'
  elif states[2] == 0:
    newMsg = 'Alarm set'
  elif states[3] == 0:
    newMsg = 'Awaiting restore'
  else:
    newMsg = 'Alarm unset'
  if newMsg != oldMsg:
    # Do something here eg email/sms/growl...!
    print (newMsg)
    subject = "ALARME DE LA MAISON"
    body = newMsg
    send_email(subject, body, sender, recipients, password)

  oldMsg = newMsg
  time.sleep(0.1)
