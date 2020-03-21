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

<dl>
  <dt><strong><code>-q</code>:</strong></dt>

  <dd>Quiets the output to only announce when a port is ready for connection.</dd>

  <dt><strong><code>-s &lt;interval_seconds&gt;</code>:</strong></dt>
  <dd>
    <em>(Default: 1)</em> specifies the number of seconds to sleep between rounds.
  </dd>

  <dt><strong><code>-t &lt;timeout_seconds&gt;</code>:</strong></dt>

  <dd>
    <em>(Default: 0)</em> specifies the minimum number of seconds to run
    before giving up and erroring out. If set to 0, no timeout is used and
    wait4ports will wait forever.
  </dd>
</dl>

## Environment

Options can also be enabled via environment variables:

<dl>
  <dt><strong><code>WAIT4PORTS_VERBOSE</code>:</strong></dt>
  <dd>
    Verbosity is on by default, but setting this value to <code>0</code>,
    <code>n[o]</code>, <code>f[alse]</code>, or<code>of[f]</code> will disable
    it.
  </dd>

  <dt><strong><code>WAIT4PORTS_SLEEP</code>:</strong></dt>
  <dd>
    <em>(Default: 1)</em> specify the number of seconds to sleep between rounds.
  </dd>

  <dt><strong><code>WAIT4PORTS_TIMEOUT</code>:</strong></dt>

  <dd>
    <em>(Default: 0)</em> specify the minimum number of seconds to run before
    giving up and erroring out. If set to 0, no timeout is used and wait4ports
    will wait forever.
  </dd>
</dl>

## Usage

```
wait4ports [-q] [-s <interval_seconds>] [-t <timeout_seconds>] \
           [name=]tcp://<host_or_ip>:<port_or_service> \
           [[name=]tcp://<host_or_ip>:<port_or_service>…]
```

### Example

```
wait4ports -q -t 60 tcp://redis:6379 loadbalancer=tcp://172.17.0.1:http
```
