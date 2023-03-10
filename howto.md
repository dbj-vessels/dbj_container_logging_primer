
# From Container to Syslog 
## On Windows Company Laptop
### How to use and test this image

**Run time requirements**

- Windows 10 pro with
  - WSL2 
  - Docker Windows Desktop
- Visual Syslog

## To syslog over Docker container logs

Set-up

- open docker desktop gui and goto settings,docker engine
- to the configuration file add
```
  "log-driver": "syslog",
  "log-opts": {
    "syslog-address": "tcp://<this machine IPv4>:514",
    "syslog-facility": "user",
    "tag": "{{.ImageName}}/{{.Name}}/{{.ID}}"
  }
```
![](media/dockerdesktop.png)
  - click on the "Apply and restart"
- start the "VisualSyslog"

### Running

Open the cmd or PowerShell

1. `docker image pull dbjdbj/dbjsyslog`
   1. [Docker pull](https://docs.docker.com/engine/reference/commandline/pull/)
2. `docker container run -d --name <your name> dbjdbj/dbjsyslog`
   1. [Docker container run](https://docs.docker.com/engine/reference/commandline/container_run/)
3. `docker container exec -it <your name> ash`
   1. Now you are inside the container in the Alpine shell aka "ash"
   2. `syslogd -O /dev/console`
      1. That redirects container local syslog to container STDOUT
      2. As a result the following message appears on the VisualSyslog
      ![](media/visualsyslog_init_message.png)
      signalling the connection has been established
      3. `logger Hello?` will now result in a VisualSyslog showing that message
      ![](media/visualsyslog_helo_message.png)

## Conclusion

That is a lot of massaging to make syslog messages from inside a container appear on the syslog server outside of it. Using container logs.

It might be Docker syslog driver configuration has to ne readjusted to start sysloging, without any additional effort from inside a container?

## Direct to syslog

1. remove "syslog" logger driver from a Docker configuration
      1. One can disable container log files : `"log-driver": "none"` in a docker dameon config file
   1. restart Docker
2.  Start the container and exec into its shell, as above
3.  setup `syslogd` to target external `udp://localhost:514`
    1.  `syslogd -R host.docker.internal:514`
    2.  Note: udp on the localhost is actually not that unreliable
4.  `logger` now has a standard role of sending to syslog
    1.  `logger -s "Hello from logger"`
5. This image logger version is `logger from util-linux 2.37.4` thus we can use it to decide on the protocol , host, port, etc. Example:

```
# target the localhost.514 syslog server   
logger -s -n host.docker.internal -P 514 "Works"
```
Produces on the host machine on the VisualSyslog side:
```
<13>1 2022-03-07T12:07:03.908125+00:00 2053c1a5921b root - - [timeQuality tzKnown="1" isSynced="0"] Works
```   
### TCP
UDP on the localhost is actually reliable. But TCP can be used to. 
And finally TCP IP protocol can be also used, thanks to the upgraded logger:
```
logger --tcp -s -n host.docker.internal -P 514 "Message over a TCP protocol"
```
Note, above we use loclhost.
That logger line produces a message on the VisualSyslog side, in any case:
```
<13>1 2022-03-07T12:08:45.566983+00:00 2053c1a5921b root - - [timeQuality tzKnown="1" isSynced="0"] Message over a TCP protocol
```

# Conclusion

Above work is based on very light Alpine linux; distro very slightly upgraded.

For further Docker Container network related research please [review this article](https://www.cloudsavvyit.com/14114/how-to-connect-to-localhost-within-a-docker-container/).