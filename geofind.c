//
//		 ██████╗ ███████╗ ██████╗ ███████╗██╗███╗   ██╗██████╗ 
//		██╔════╝ ██╔════╝██╔═══██╗██╔════╝██║████╗  ██║██╔══██╗
//		██║  ███╗█████╗  ██║   ██║█████╗  ██║██╔██╗ ██║██║  ██║
//		██║   ██║██╔══╝  ██║   ██║██╔══╝  ██║██║╚██╗██║██║  ██║
//		╚██████╔╝███████╗╚██████╔╝██║     ██║██║ ╚████║██████╔╝
//		 ╚═════╝ ╚══════╝ ╚═════╝ ╚═╝     ╚═╝╚═╝  ╚═══╝╚═════╝ 
//         a simple script for fetching geolocation data from an IP or website.                          
//				https://github.com/melyns
//
#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include <json-c/json.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdbool.h>
#include <stdlib.h>
#include "country_codes.h"

#define GEO_API "https://ipinfo.io/"
#define MAX_JSON_BUFFER_SIZE 4096
#define MAX_URL_SIZE (sizeof(GEO_API) + INET6_ADDRSTRLEN + 5)
#define UNKNOWN_COUNTRY "Unknown"
#define OUTPUT_FILE_SUFFIX "_geofind.txt"

size_t write_callback(void *contents, size_t size, size_t nmemb, char *output) {
    size_t total_size = size * nmemb;
    size_t remaining_space = MAX_JSON_BUFFER_SIZE - strlen(output);
    size_t copy_size = total_size < remaining_space ? total_size : remaining_space;
    memcpy(output + strlen(output), contents, copy_size);
    return copy_size;
}

const char* get_country_name(const char* country_code) {
    for (size_t i = 0; i < NUM_COUNTRY_CODES; ++i) {
        if (strcmp(country_codes[i].code, country_code) == 0) {
            return country_codes[i].name;
        }
    }
    return UNKNOWN_COUNTRY;
}

bool get_geolocation_info(const char *ip_or_url, FILE *output_file) {
    struct addrinfo hints, *res, *p;
    int status;
    char ip_buffer[INET6_ADDRSTRLEN] = {0};

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((status = getaddrinfo(ip_or_url, NULL, &hints, &res)) != 0) {
        fprintf(output_file, "Error resolving IP or domain %s: %s\n", ip_or_url, gai_strerror(status));
        return false;
    }

    for (p = res; p != NULL; p = p->ai_next) {
        if (p->ai_family == AF_INET) {
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
            inet_ntop(p->ai_family, &(ipv4->sin_addr), ip_buffer, sizeof ip_buffer);
            break;
        }
    }

    freeaddrinfo(res);

    if (strlen(ip_buffer) == 0) {
        fprintf(output_file, "Error: Unable to resolve IP address for %s.\n", ip_or_url);
        return false;
    }

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
            struct json_object *json = json_tokener_parse(buffer);
            if (json != NULL && json_object_is_type(json, json_type_object)) {
                struct json_object *city, *region, *country, *hostname, *org;

                json_object_object_get_ex(json, "city", &city);
                json_object_object_get_ex(json, "region", &region);
                json_object_object_get_ex(json, "country", &country);
                json_object_object_get_ex(json, "hostname", &hostname);
                json_object_object_get_ex(json, "org", &org);

                fprintf(output_file, "IP: %s\n", ip_buffer);
                fprintf(output_file, "City: %s\n", city ? json_object_get_string(city) : "(null)");
                fprintf(output_file, "Region: %s\n", region ? json_object_get_string(region) : "(null)");
                fprintf(output_file, "Country: %s\n", country ? get_country_name(json_object_get_string(country)) : UNKNOWN_COUNTRY);
                fprintf(output_file, "Org: %s\n", org && json_object_get_type(org) != json_type_null ? json_object_get_string(org) : "(N/A)");
                fprintf(output_file, "Hostname: %s\n", hostname && json_object_get_type(hostname) != json_type_null ? json_object_get_string(hostname) : "(N/A)");

                json_object_put(json);
                success = true;
            } else {
                fprintf(output_file, "Error parsing JSON or invalid JSON format.\n");
            }
        } else {
            fprintf(output_file, "Error fetching geolocation data.\n");
        }
    } else {
        fprintf(output_file, "Error initializing libcurl.\n");
    }

    return success;
}

void print_usage(const char *program_name) {
    printf("Usage:\n");
    printf("%s <IP address or domain> [file.txt]\n", program_name);
    printf("Self lookup: %s me\n", program_name);
}

void process_file(const char* filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return;
    }

    char output_filename[FILENAME_MAX];
    snprintf(output_filename, sizeof(output_filename), "%s%s", filename, OUTPUT_FILE_SUFFIX);
    FILE *output_file = fopen(output_filename, "w");
    if (!output_file) {
        perror("Error opening output file");
        fclose(file);
        return;
    }

    char line[INET6_ADDRSTRLEN];
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;

        if (strlen(line) > 0) {
            fprintf(output_file, "\nFetching geolocation for: %s\n", line);
            bool success = get_geolocation_info(line, output_file);
            if (!success) {
                fprintf(output_file, "Failed to fetch geolocation for: %s\n", line);
            }
        }
    }

    fclose(file);
    fclose(output_file);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Error: Invalid number of arguments.\n");
        print_usage(argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
        print_usage(argv[0]);
        return 0;
    }

    curl_global_init(CURL_GLOBAL_DEFAULT);

    if (argc == 2) {
        if (strcmp(argv[1], "me") == 0) {
            system("curl https://ipinfo.io");
        } else {
            if (strstr(argv[1], ".txt") != NULL) {
                printf("Processing IPs from file: %s\n", argv[1]);
                process_file(argv[1]);
            } else {
                printf("\nFetching geolocation for: %s\n", argv[1]);
                get_geolocation_info(argv[1], stdout);
            }
        }
    }

    curl_global_cleanup();

    return 0;
}
