
 575  sudo insmod lab_char_module1.ko 
  576  dmesg 
  577  clear
  578  lsmod 
  579  ls /sys/class/misc/mycdrv
  580  more  /sys/class/misc/mycdrv
  581  ls /sys/class/misc/mycdrv/dev 
  582  more /sys/class/misc/mycdrv/dev 
  583  clear
  584  ls
  585  more /sys/class/misc/mycdrv/uevent 
  586  ls /dev/mycdrv 
  587  dmesg 
  588  echo 1 > /dev/mycdrv 
  589  sudo echo 1 > /dev/mycdrv 
  590  chmod 777 /dev/mycdrv 
  591  sudo chmod 777 /dev/mycdrv 
  592  sudo echo 1 > /dev/mycdrv 
  593  dmesg 
  594  lsmod 
  595  ls
  596  dmesg 
  597  cat /dev/mycdrv 
  598  clear
  599  ls
  600  sudo echo 'Let's do it' > /dev/mycdrv 

  601  sudo echo 'Lets do it' > /dev/mycdrv
  602  more /dev/mycdrv 
  603  cat /dev/mycdrv 
  604  clear
  605  history
  606  ls
  607  make clean
  608  ls
  609  sudo rmmod lab_char_module1 
  610  clear
  611  dmesg 
  612  tail -f /var/log/syslog
  613  history

