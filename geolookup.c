// Tiny program for finding geolocation of an IP address or a website.
// github.com/melyyn

#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include <json-c/json.h>
#include <arpa/inet.h>
#include <netdb.h> // For DNS lookup

#define GEOLOCATION_API "https://ipinfo.io/"

// Function to perform an HTTP GET request
size_t write_callback(void *contents, size_t size, size_t nmemb, char *output) {
    strcat(output, (char *)contents);
    return size * nmemb;
}

// Function to perform a DNS lookup to get the IP address from the URL
int dns_lookup(const char *url, char *ip_buffer, int ip_buffer_size) {
    struct hostent *he;
    struct in_addr **addr_list;
    int i;

    if ((he = gethostbyname(url)) == NULL) {
        // Unable to resolve the hostname
        return 0;
    }

    addr_list = (struct in_addr **)he->h_addr_list;

    for (i = 0; addr_list[i] != NULL; i++) {
        strncpy(ip_buffer, inet_ntoa(*addr_list[i]), ip_buffer_size);
        return 1; // Success, return the first IP address found
    }

    return 0; // No IP address found
}

// Function to fetch geolocation information and print it
void get_geolocation_info(const char *ip_or_url) {
    char ip_buffer[16] = {0}; // Buffer to store the IP address

    // Check if the input is a URL, and if so, perform a DNS lookup to get the IP
    if (!dns_lookup(ip_or_url, ip_buffer, sizeof(ip_buffer))) {
        printf("Error: Unable to resolve the URL to an IP address.\n");
        return;
    }

    char url[100];
    snprintf(url, sizeof(url), "%s%s/json", GEOLOCATION_API, ip_buffer);

    char buffer[4096] = {0};
    CURL *curl = curl_easy_init();

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, buffer);

        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (res == CURLE_OK) {
            // Parse JSON and extract geolocation data
            struct json_object *json = json_tokener_parse(buffer);
            struct json_object *city, *region, *country, *hostname;

            json_object_object_get_ex(json, "city", &city);
            json_object_object_get_ex(json, "region", &region);
            json_object_object_get_ex(json, "country", &country);
            json_object_object_get_ex(json, "hostname", &hostname);

            printf("IP/URL: %s\n", ip_or_url);
            printf("City: %s\n", json_object_get_string(city));
            printf("Region: %s\n", json_object_get_string(region));
            printf("Country: %s\n", json_object_get_string(country));
            printf("Hostname: %s\n", json_object_get_string(hostname));

            json_object_put(json);
        } else {
            printf("Error fetching geolocation data.\n");
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s [ip_or_url]\n", argv[0]);
        return 1;
    }

    // Initialize libcurl
    curl_global_init(CURL_GLOBAL_DEFAULT);

    // Fetch geolocation information
    get_geolocation_info(argv[1]);

    // Clean up libcurl
    curl_global_cleanup();

    return 0;
}

