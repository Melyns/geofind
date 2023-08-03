# GeoFind   [![CI](https://github.com/Melyyn/geofind/actions/workflows/ci.yml/badge.svg)](https://github.com/Melyyn/geofind/actions/workflows/ci.yml)

GeoFind is a tiny command-line tool that fetches geolocation information from an IP address or a domain. It uses ipinfo.io to provide info associated with the given IP address or a website.

![Screenshot](/.github/screenshot.png)

## Prerequisites
- libcurl & libjson-c

```bash
sudo apt update
sudo apt install libcurl4-openssl-dev libjson-c-dev
```

## Installation
Clone the repository:
```bash
git clone https://github.com/melyyn/geofind.git && cd geofind
```

Build:
```bash
gcc geofind.c -o geofind -lcurl -ljson-c
```

Move the executable to your bin directory
```bash
sudo mv geofind /usr/local/bin/
```

## Usage
```bash
geofind [IP address or domain]
```

## Examples
```bash
$ geofind 8.8.8.8

IP: 8.8.8.8
City: Mountain View
Region: California
Country: United States
Hostname: dns.google
```

```bash
$ geofind nasa.gov

IP: 23.22.39.120
City: Ashburn
Region: Virginia
Country: United States
Hostname: ec2-23-22-39-120.compute-1.amazonaws.com
```
