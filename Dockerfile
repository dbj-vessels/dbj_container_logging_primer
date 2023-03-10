FROM alpine:latest
LABEL Name=dbjsyslog Version=0.1.0
WORKDIR /dbj
COPY . .
# so we have a plenty of time to enter and play
ENTRYPOINT ["/bin/sh","-c","sleep infinity"]
