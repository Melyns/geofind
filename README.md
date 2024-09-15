# GeoFind   [![CI](https://github.com/Melyyn/geofind/actions/workflows/ci.yml/badge.svg)](https://github.com/Melyyn/geofind/actions/workflows/ci.yml)

GeoFind is a command-line tool/script that fetches geolocation information from an IP address or a domain using https://ipinfo.io

![Screenshot](/.github/screenshot.png)

## Prerequisites
- libcurl & libjson-c

```bash
sudo apt update
sudo apt install libcurl4-openssl-dev libjson-c-dev gcc
```

## Installation
Clone the repository:
```bash
git clone https://github.com/melyyn/geofind.git && cd geofind
```

Build & Install:
```bash
cat build | bash && sudo mv geofind /usr/local/bin
```

## Usage
```bash
geofind [IP address or domain]
geofind me
```

## Examples
```bash
$ geofind 8.8.8.8

IP: 8.8.8.8
City: Mountain View
Region: California
Country: United States
Org: AS15169 Google LLC
Hostname: dns.google
```

```bash
$ geofind nasa.gov

IP: 23.22.39.120
City: Ashburn
Region: Virginia
Country: United States
Org: AS14618 Amazon.com, Inc.
Hostname: ec2-23-22-39-120.compute-1.amazonaws.com
```

## License
[GNU General Public License v3.0](https://github.com/Melyyn/geofind/blob/main/LICENSE)
