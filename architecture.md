
# Logging infrastructure research

Key runtime assumptions are

- [Cloud](https://en.wikipedia.org/wiki/Cloud_computing)
- [Kubernetes](https://kubernetes.io/)
- [Docker](https://www.docker.com/) and Containers

Those are key factors shaping up the research and suggested solutions.

<!-- div class="page"/ -->

# 1. Kubernetes Agents

Standard Kubernetes option for the loggin solutions are so called "Agents". Those are separate processes on the Kubernetes Nodes (mapped to hostVM's).

Agents are installed using `cubectl` command and are thus "known" to the Kubernetes; and are  managed when Kubernetes decides to shutdown, start or restart that VM host.
```
 +-------------------------------------------------------------------+
 | Kubernetes "node" mapped to VM                                    |
 |                                  +-----------------------+        |
 |                                  |                       |        |
 |                                  |     Logging Agent     |        |
 |                                  |     (logging relay)   |        |
 |                                  +-----------------------+        |
 | +------------------------------------------  |  ----------------+ |
 | |                k8s Pod                     |                  | |
 | |   +------------------------------------docker daemon-------+  | |
 | |   |                                        |               |  | |
 | |   |  Docker      +-------------------------+-+             |  | |
 | |   |              |  Read                     |  Read       |  | |
 | |   |              v                           v             |  | |
 | |   |  +---------Logs----------+   +---------Logs----------+ |  | |
 | |   |  |                       |   |                       | |  | |
 | |   |  |      Container        |   |      Container        | |  | |
 | |   |  |                       |   |                       | |  | |
 | |   |  +-----------------------+   +-----------------------+ |  | |
 | |   +--------------------------------------------------------+  | |
 | +---------------------------------------------------------------+ |
 +-------------------------------------------------------------------+
```
In this scenario Agent is pulling logging information from containers. In syslog scenarios clients are pushing messages. Benefit of that is Syslog servers/relays do not need to know about container syslog senders.

Agent is reading all the logs of all the containers present in the "node".  As Kubernetes can make containers "come and go" in a very dinamic manner. Thus agent needs to comunicate with both Kubernetes controler and docker daemon (aka service) to know the number of containers and their ID's.

Almost all logging infrastructure vendors are using Agents. Basically Agent is relaying logs to the outside to the final concentrator and visualiser, like e.g. "Elastic Stack" and "Kibana", or "Splunk". "Syslog" too, as almost all vendors and clients have already in place a full "Syslog" infrastructure.

# 2. Using out-of-the-box Docker logging drivers

The following logging drivers are supported out-of-the-box ina Docker. See the link to each driver’s documentation for its configurable options, if applicable. If you are using [logging driver plugins](https://docs.docker.com/config/containers/logging/plugins/) (LDP's), you may see more options.

Docker Engine [managed plugin system](https://docs.docker.com/engine/extend/) and LDP's are primarily supported and oriented towards GO programing language in which Docker in itself is written too. Many of the features are also **not** supported on Windows daemons.

| Driver	| Description
|-----------|------
|none|	No logs are available for the container and docker logs does not return any output.
|local|	Logs are stored in a custom format designed for minimal overhead.
|json-file|	The logs are formatted as JSON. The default logging driver for Docker.
|syslog|	Writes logging messages to the syslog facility. The syslog daemon must be running on the host machine.
|journald|	Writes log messages to journald. The journald daemon must be running on the host machine.
|gelf|	Writes log messages to a Graylog Extended Log Format (GELF) endpoint such as Graylog or Logstash.
|fluentd|	Writes log messages to fluentd (forward input). The fluentd daemon must be running on the host machine.
|awslogs|	Writes log messages to Amazon CloudWatch Logs.
|splunk|	Writes log messages to splunk using the HTTP Event Collector.
|etwlogs|	Writes log messages as Event Tracing for Windows (ETW) events. Only available on Windows platforms.
|gcplogs|	Writes log messages to Google Cloud Platform (GCP) Logging.
|logentries|	Writes log messages to Rapid7 Logentries.

It is important to note only the first three drivers are writing to containers logs only. All the rest are passing the logs to some external logging system.

Sometimes they are called "Log Shippers", thus we will call them the same or just abbreviated: "Shippers".

# 2. Log Shipper in a container

Shipper in a container is a component. With all the benefits of the encapsulation and decoupling, associated with components. This is simpler and lighter solution than "Agent". It does also work on non Kubernetes installations. It is managed by Docker and can be part of a [Docker Compose](https://docs.docker.com/compose/)d application. Or a part of a [Docker Swarm](https://docs.docker.com/engine/swarm/key-concepts/) for natively managing a cluster of Docker Engines.

```
      Cloud "Region" (Amazon, AWS, Google, etc.)
    +----------------------------------------------------------------+
    |   1 .. N Host VM's (optional Kubernetes Node and Pods on each) |
    |   +--------------------------------------------------------+   |
    |   |             Docker                                     |   |
    |   |  +-----------------------+  +-----------------------+  |   |     Central aggregations
    |   |  |                       |  | Container             |            and dash boards
    |   |  |    1..N Containers    |  |   1 Log Shipper relay +- -- -- ->  "Splunk" , "ELK"
    |   |  |                       |  |                       |            etc.
    |   |  +------  Log     +------+  +------ port -----------+  |   |     ...
    |   |           Shipper                    ^                 |   |
    |   |           Driver                     |                 |   |     Integrated with
    |   |                                      |                 |   |     Cloud infrastructure
    |   |              |                       |                 |   |     due to conectivity
    |   |              +-----------------------+                 |   |     security and compliance
    |   +--------------------------------------------------------+   |
    +----------------------------------------------------------------+
```

Kubernetes "pods" are placing containers under the rule of Kubernetes. Thus managed in orderly fashion.

Please note why we can not place the "Log shipper relay" on the Host VM (outside of a container) in case it is acting as Kubernetes node.
That is because VM acting as a Kubernetes node does "come and go" as ordered by the Kubernetes system. And that might render very low resilience of the whole solution where processes might be simply aborted removed from memory.

Shipping Relay user container Docker setup in this scenario can be simple and generic. Both ends of the container to container communication are on the same host. Here is the example for when using "Syslog" logg shipper driver.
```
  "log-driver": "syslog",
  "log-opts": {
    "syslog-address": "udp://host.docker.internal:514",
    "syslog-facility": "user",
    "tag": "{{.ImageName}}/{{.Name}}/{{.ID}}"
  }
```
Default continer network is `bridge`. And `host.docker.internal` is docker alias for external `localhost`. 
Also udp traffic over localhost is not (that) unreliable. Tcp and required security for tcp might not be required.
## TO DO 
- that separate syslog container relays the logs to some central aggregator external to the solution.
  - that has to be shaped up for the development testing phase and final production phase
  - for ALPINE based log shipper relay container we migh use https://github.com/moonbuggy/docker-syslog-ng-alpine
- This configuration has to be setup and tested, on the Cloud of choice
- The [`tcp+tls` protocol](https://docs.docker.com/config/containers/logging/syslog/) will have to be investigated.

# 3. Separate  Log Shipper host

This concept assumes separate Log Shipper server machine, "targeted" by all the other senders in the solution; wherever they are. That separate server is a [SPOF](https://en.wikipedia.org/wiki/Single_point_of_failure) but that issue has a standard remedy, outside of the context of this text.

This architecture also contains the host VM, host OS, potential use of the host as`Kubernetes` "node" and finally the Cloud on which the lot resides.
```
    Cloud "Region" (Amazon, AWS, Google, etc.)
   +-------------------------------------------------+
   |                                                 |   +-----------------+
   | Host VM's (optional Kubernetes Node and Pods)   |   | Log Server      |
   |      +---------------------------------+        |   |                 |
   |      |            Docker               |        |   |  Final          |
   |      |                                 |        |   |  destionation   |
   |      |    +-----------------------+    |        |   |  or relay       |
   |      |    |                       |    |        |   |                 |
   |      |    | 1..N Containers       |    |        |   +-----------------+
   |      |    |                       |    |        |     Server IP:514
   |      |    |       Log             |    |        |            ^
   |      |    +-----  Shipper --------+    |        |            |
   |      |            Driver               +        +            |
   |      |             +-----------------------------------------+
   |      +---------------------------------+        +
   +-------------------------------------------------+
```
That is very resilient solution because Log server is on a separate machine. This logging solution will work regardless of the Kubernetes use and even regardless of the Docker use.

But. That solution is not very good for production setup on customer site. In that scenario, docker configuration is not generic. Change of the foreign syslog server IP, will require reconfig and restarting of all the containers used. Docker syslog driver needs that IP changed because it is hard-coded. 

And in case that IP changes at runtime, and reconfi is not performed, syslog messages will simply disappear on the consumer side.

Client container setup

UDP is known as unreliable protocol. TCP is reliable protocol required for inter server communication. The docker syslog driver setup might be, for the external SYSLOG host `10.68.0.11` this:
```
  "log-driver": "syslog",
  "log-opts": {
    "syslog-address": "tcp://10.68.0.11:514",
    "syslog-facility": "user",
    "tag": "{{.ImageName}}/{{.Name}}/{{.ID}}"
  }
```
<!- The host Docker daemon configuration file on Windows is here: `C:\Users\<USER NAME>\.docker\daemon.json` -->

Issue immediately spotted above is: `"syslog-address" : "tcp://192.168.0.11:514"`. 

That is a hardcoded IP. There are various ways to solve that problem.

## To DO

- Hardcoded IP is a "problem to be solved" on the Docker hosts where container syslog driver (and others) will need TCP access to the foreign host. 
- The [`tcp+tls` protocol](https://docs.docker.com/config/containers/logging/syslog/) might have to be investigated and used.



