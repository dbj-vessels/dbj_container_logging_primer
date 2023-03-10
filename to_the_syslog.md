[This video](https://youtu.be/CSb-sHNM2NY) is very wrongly named. Watch it first.

# From Container to the Syslog
<!-- Compiler: dusan.jovanovic@fisglobal.com 
[inspiration](https://www.cloudsavvyit.com/14114/how-to-connect-to-localhost-within-a-docker-container/)
-->
There are basically two options when using syslog from a container. 

1. Pass syslog message over a network
1. Pass syslog messages over container logs

Various source files in here contain the other very usefull details.

## 1. Pass syslog message over a network

"The Easy Option", is to target the default syslog server `localhost:514` on the host machine.

Docker Desktop (18.03+) for Windows (and Mac) supports `host.docker.internal` as a functioning alias for `localhost`. Use this string inside your containers to access your host machine. (TODO: how is this working on a Linux Host + Docker?)

1. localhost and 127.0.0.1 – Resolves to the container
2. host.docker.internal – Resolves to the outside host

## 2.Pass syslog messages over container logs

ALPINE ash `echo`:
```
echo "And this is a simple echo to dev/console" > /dev/console
```
From a C/C++ code, the standard CRT `system()` function might be used in a similar manner:
```
//
system( "Hello container log ..." " > /dev/console");
```

## Docker howto

### Howto push to the https://hub.docker.com

To be apsolutely certain advice works for you, first do the `docker logout` and then `docker login`.

- You have a hub.docker.com account and it has a name. For example `dbjdbj`.
- Docker push needs your image to be in a NAMESPACE same as you hub account.
- thus you need to rename the image to `dbjdbj/imagename`
  
Lets assume image you want to push is called `imagename`
```
docker tag imagename dbjdbj/imagename
```
And now you should be able to push it
```
docker push dbjdbj/imagename
```
But why this complexity, docker knows I am logged in as `dbjdbj`?

Because docker does not use your logged in name as the default namespace.
Docker knows you are logged in as `dbjdbj` but when you push you need to explicitly use it as a "namespace". 
Thus the image has to be named `<hub login name>/<image name>:<tag>` before the push. 

Byt why, why? Becayse in the hub.docker.com you can create your own [namespaces](https://docs.docker.com/docker-hub/repos/). Please read about that elsewhere.

> (c) dbj@dbj.org CC BY SA 4.0