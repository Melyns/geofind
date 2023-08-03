# GeoFind
GeoFind is a tiny command-line tool that fetches geolocation information from an IP address or a domain. It uses ipinfo.io to provide info such as city, region, country, and hostname associated with the given IP address or a URL.

![Screenshot](/.github/screenshot.png)

## Prerequisites
- libcurl (Curl library)
- libjson-c (JSON-C library)

```bash
sudo apt update
sudo apt install libcurl4-openssl-dev libjson-c-dev
```

## Installation
Clone the repository:
```bash
git clone https://github.com/melyyn/geofind.git
cd geofind
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

## Example
```bash
$ geofind 8.8.8.8

IP: 8.8.8.8
City: Mountain View
Region: California
Country: United States
Hostname: dns.google
```
```bash
$ geofind 12.12.12.12

IP: 12.12.12.12
City: Anchorage
Region: Alaska
Country: United States
Hostname: doh.att.net


