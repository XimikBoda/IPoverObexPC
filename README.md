# IPoverObexPC

## Linux
You need to register D-Bus service by copying ```IPoverObex.conf``` to ```/etc/dbus-1/system.d/```

Also you need to stop obex.service, you can use ```systemctl --user mask obex.service```