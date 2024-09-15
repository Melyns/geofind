# GeoFind   [![CI](https://github.com/Melyyn/geofind/actions/workflows/ci.yml/badge.svg)](https://github.com/Melyyn/geofind/actions/workflows/ci.yml)

Command-line tool that fetches geolocation information from an IP address or a domain using https://ipinfo.io

![image](https://github.com/user-attachments/assets/feb6b2b0-997a-45cc-a43d-192b01eb6277)

## Prerequisites
- libcurl, libjson-c, curl, gcc

```bash
sudo apt update
sudo apt install curl libcurl4-openssl-dev libjson-c-dev gcc
```

## Installation
Clone the repository:
```bash
git clone https://github.com/melyns/geofind.git && cd geofind
```

Build & Install:
```bash
chmod +x build.sh && ./build.sh
```

## Usage
```bash
geofind [IP address or domain]
geofind me
geofind file.txt
```
You can use a text file containing list of IPs and/or domains seperated by line break as input. The output file will have `_geofind.txt` as prefix and will be saved in the same location as the input file. For example your list should look like this:
```
nasa.gov
1.1.1.1
archive.org
8.8.8.8
www.google.com
```

You can also scan for multiple IPs/domains at once by seperating each target with space.

## Examples

Single target

```bash
$ geofind 8.8.8.8

IP: 8.8.8.8
City: Mountain View
Region: California
Country: United States
Org: AS15169 Google LLC
Hostname: dns.google
```

Multiple targets at once, each seperated by space

```bash
$ geofind 6.9.42.0 microsoft.com www.fbi.gov

Results for: 6.9.42.0
IP: 6.9.42.0
City: Columbus
Region: Ohio
Country: United States
Org: (N/A)
Hostname: (N/A)

Results for: microsoft.com
IP: 20.236.44.162
City: Moses Lake
Region: Washington
Country: United States
Org: AS8075 Microsoft Corporation
Hostname: (N/A)

Results for: www.fbi.gov
IP: 104.16.148.244
City: San Francisco
Region: California
Country: United States
Org: AS13335 Cloudflare, Inc.
Hostname: (N/A)
```

Process targets from a list

```bash
$ geofind ip.txt
Processing IPs from file: ip.txt
```

## License
[GNU General Public License v3.0](https://github.com/Melyyn/geofind/blob/main/LICENSE)
