//
//		 ██████╗ ███████╗ ██████╗ ███████╗██╗███╗   ██╗██████╗ 
//		██╔════╝ ██╔════╝██╔═══██╗██╔════╝██║████╗  ██║██╔══██╗
//		██║  ███╗█████╗  ██║   ██║█████╗  ██║██╔██╗ ██║██║  ██║
//		██║   ██║██╔══╝  ██║   ██║██╔══╝  ██║██║╚██╗██║██║  ██║
//		╚██████╔╝███████╗╚██████╔╝██║     ██║██║ ╚████║██████╔╝
//		 ╚═════╝ ╚══════╝ ╚═════╝ ╚═╝     ╚═╝╚═╝  ╚═══╝╚═════╝ 
//         a simple tool for fetching geolocation data from an IP or website.                          
//				https://github.com/melyyn
//

#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include <json-c/json.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdbool.h>
#include "country_codes.h"

#define GEO_API "https://ipinfo.io/"
#define MAX_JSON_BUFFER_SIZE 4096
#define MAX_URL_SIZE (sizeof(GEO_API) + INET6_ADDRSTRLEN + 5) // 5 for "/json"
#define UNKNOWN_COUNTRY "Unknown"

// Function to perform an HTTP GET request
size_t write_callback(void *contents, size_t size, size_t nmemb, char *output) {
    size_t total_size = size * nmemb;
    size_t remaining_space = MAX_JSON_BUFFER_SIZE - strlen(output);
    size_t copy_size = total_size < remaining_space ? total_size : remaining_space;
    memcpy(output + strlen(output), contents, copy_size);
    return copy_size;
}

// Function to get the full country name from the country code
const char* get_country_name(const char* country_code) {
    for (size_t i = 0; i < NUM_COUNTRY_CODES; ++i) {
        if (strcmp(country_codes[i].code, country_code) == 0) {
            return country_codes[i].name;
        }
    }
    return UNKNOWN_COUNTRY;
}

// Function to fetch geolocation information and print it
bool get_geolocation_info(const char *ip_or_url) {
    struct addrinfo hints, *res, *p;
    int status;
    char ip_buffer[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((status = getaddrinfo(ip_or_url, NULL, &hints, &res)) != 0) {
        printf("Error: %s\n", gai_strerror(status));
        return false;
    }

    for (p = res; p != NULL; p = p->ai_next) {
        if (p->ai_family == AF_INET) { // IPv4
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
            inet_ntop(p->ai_family, &(ipv4->sin_addr), ip_buffer, sizeof ip_buffer);
            break;
        }
    }

    freeaddrinfo(res);

    // API request
    char url[MAX_URL_SIZE];
    snprintf(url, sizeof(url), "%s%s/json", GEO_API, ip_buffer);

    char buffer[MAX_JSON_BUFFER_SIZE] = {0};
    CURL *curl = curl_easy_init();
    bool success = false;

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, buffer);

        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (res == CURLE_OK) {
            // Parse JSON and extract geolocation data
            struct json_object *json = json_tokener_parse(buffer);
            if (json != NULL && json_object_is_type(json, json_type_object)) {
                struct json_object *city, *region, *country, *hostname, *org;

                json_object_object_get_ex(json, "city", &city);
                json_object_object_get_ex(json, "region", &region);
                json_object_object_get_ex(json, "country", &country);
                json_object_object_get_ex(json, "hostname", &hostname);
                json_object_object_get_ex(json, "org", &org);

                printf("\nIP: %s\n", ip_buffer);
                printf("City: %s\n", json_object_get_string(city));
                printf("Region: %s\n", json_object_get_string(region));
                printf("Country: %s\n", get_country_name(json_object_get_string(country)));

                if (json_object_get_type(org) == json_type_null) {
                    printf("Org: (N/A)\n");
                } else {
                    printf("Org: %s\n", json_object_get_string(org));
                }

                if (json_object_get_type(hostname) == json_type_null) {
                    printf("Hostname: (N/A)\n");
                } else {
                    printf("Hostname: %s\n", json_object_get_string(hostname));
                }

                json_object_put(json);
                success = true;
            } else {
                printf("Error parsing JSON or invalid JSON format.\n");
            }
        } else {
            printf("Error fetching geolocation data.\n");
        }
    } else {
        printf("Error initializing libcurl.\n");
    }

    return success;
}

// Function to print the usage information
void print_usage(const char *program_name) {
    printf("Usage: %s [IP or Domain]\n", program_name);
}

int main(int argc, char *argv[]) {
    bool success = false;

    if (argc != 2) {
        // Print usage information and exit if invalid number of arguments
        print_usage(argv[0]);
        return 1;
    }

    // Initialize libcurl
    curl_global_init(CURL_GLOBAL_DEFAULT);

    // Fetch geolocation information
    success = get_geolocation_info(argv[1]);

    // Clean up libcurl
    curl_global_cleanup();

    return 0;
}
