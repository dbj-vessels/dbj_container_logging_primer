
# WIP

- This alpine based image is at hub.docker.com. Pull it as `dbjdbj/dbjsyslog`. Its size is 3.38 MB as per Docker Desktop report :wink: .
- there are md's in here, read them all. Slowly and carefully
- Why SysLOG ? It was the firs. It is in use everywhere around you, at home in office. everyhwere.

## [Architecture rumminations](architecture.md)

## [To the syslog](to_the_syslog.md) from container

Why not direct to syslog? Many reasons

- coupling with the host os
- decoupling from K8S
   - K8S manages containers
   - K8S logging agents are part of Enteprise Logging (EL) infrastructure
        - high frequence high pressure logging and monitoring
        - EL is a network of "log relays" agents required by the EL infrastructure
