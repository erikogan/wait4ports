# wait4ports

A small utility to wait for network peer availability. It polls each
address and port, sleeping between runs.

Designed to be used in docker, specifically docker-compose, containers that
need to wait for linked containers to be ready.

## Usage

```
wait4ports tcp://127.0.0.1:9160 tcp://8.8.8.8:53
```
