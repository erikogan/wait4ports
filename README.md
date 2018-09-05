# wait4ports

A small utility to wait for network peer availability. It polls each
address and port, sleeping between runs.

Designed to be used in docker, specifically docker-compose, containers that
need to wait for linked containers to be ready.

Hostname lookups and IPv6 are supported, and you can give each service your
own name. Currently only TCP sockets are supported.

An [AlpineLinux Package](https://github.com/alpinelinux/aports/blob/master/community/wait4ports/APKBUILD)
is available. (Currently in the [`@edge`](https://pkgs.alpinelinux.org/packages?name=wait4ports&branch=edge)
repository.)

## Options

`-q`: Quiets the output to only announce when a port is ready for connection.

`-s <seconds>`: specifies the number of seconds to sleep between rounds. (Default: 1)

## Environment

Options can also be enabled via environment variables:

`WAIT4PORTS_VERBOSE`: Verbosity is on by default, but setting this value to
`0`, `n[o]`, `f[alse]`, or `of[f]` will disable it.

`WAIT4PORTS_SLEEP`: Specify the number of seconds to sleep between rounds.
(Default: 1)

## Usage

```
wait4ports [name=]tcp://<host_or_ip>:<port_or_service> [[name=]tcp://<host_or_ip>:<port_or_service>…]
```

### Example

```
wait4ports redis:6379 loadbalancer=172.17.0.1:http
```
