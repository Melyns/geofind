# GeoFind
GeoFind is a simple command-line tool that fetches geolocation information from an IP address or URL. It uses ipinfo.io to provide info such as city, region, country, and hostname associated with the given IP address or a URL.

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

Build the executable:
```bash
gcc geofind.c -o geofind -lcurl -ljson-c
```

Move the executable to your bin directory
```bash
sudo mv geofind /usr/local/bin/
```

## Usage
```bash
geofind [IP address or URL]
```

## Example
