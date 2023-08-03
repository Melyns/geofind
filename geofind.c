// geofind: a tiny tool for finding geolocation data of an IP address or a website. //
// github.com/melyyn //

#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include <json-c/json.h>
#include <arpa/inet.h>
#include <netdb.h>

#define GEO_API "https://ipinfo.io/"

// Structure to hold country code and name mapping
struct CountryCode {
    const char* code;
    const char* name;
};

// List of country codes and their corresponding full names
static const struct CountryCode country_codes[] = {
    { "AF", "Afghanistan" },
    { "AL", "Albania" },
    { "DZ", "Algeria" },
    { "AS", "American Samoa" },
    { "AD", "Andorra" },
    { "AO", "Angola" },
    { "AI", "Anguilla" },
    { "AQ", "Antarctica" },
    { "AG", "Antigua and Barbuda" },
    { "AR", "Argentina" },
    { "AM", "Armenia" },
    { "AW", "Aruba" },
    { "AU", "Australia" },
    { "AT", "Austria" },
    { "AZ", "Azerbaijan" },
    { "BS", "Bahamas" },
    { "BH", "Bahrain" },
    { "BD", "Bangladesh" },
    { "BB", "Barbados" },
    { "BY", "Belarus" },
    { "BE", "Belgium" },
    { "BZ", "Belize" },
    { "BJ", "Benin" },
    { "BM", "Bermuda" },
    { "BT", "Bhutan" },
    { "BO", "Bolivia" },
    { "BQ", "Bonaire" },
    { "BA", "Bosnia and Herzegovina" },
    { "BW", "Botswana" },
    { "BV", "Bouvet Island" },
    { "BR", "Brazil" },
    { "IO", "British Indian Ocean Territory" },
    { "BN", "Brunei" },
    { "BG", "Bulgaria" },
    { "BF", "Burkina Faso" },
    { "BI", "Burundi" },
    { "CV", "Cabo Verde" },
    { "KH", "Cambodia" },
    { "CM", "Cameroon" },
    { "CA", "Canada" },
    { "KY", "Cayman Islands" },
    { "CF", "Central African Republic" },
    { "TD", "Chad" },
    { "CL", "Chile" },
    { "CN", "China" },
    { "CX", "Christmas Island" },
    { "CC", "Cocos Islands" },
    { "CO", "Colombia" },
    { "KM", "Comoros" },
    { "CG", "Congo" },
    { "CD", "Democratic Republic of Congo" },
    { "CK", "Cook Islands" },
    { "CR", "Costa Rica" },
    { "HR", "Croatia" },
    { "CU", "Cuba" },
    { "CW", "Curaçao" },
    { "CY", "Cyprus" },
    { "CZ", "Czech Republic" },
    { "DK", "Denmark" },
    { "DJ", "Djibouti" },
    { "DM", "Dominica" },
    { "DO", "Dominican Republic" },
    { "EC", "Ecuador" },
    { "EG", "Egypt" },
    { "SV", "El Salvador" },
    { "GQ", "Equatorial Guinea" },
    { "ER", "Eritrea" },
    { "EE", "Estonia" },
    { "ET", "Ethiopia" },
    { "FK", "Falkland Islands" },
    { "FO", "Faroe Islands" },
    { "FJ", "Fiji" },
    { "FI", "Finland" },
    { "FR", "France" },
    { "GF", "French Guiana" },
    { "PF", "French Polynesia" },
    { "TF", "French Southern Territories" },
    { "GA", "Gabon" },
    { "GM", "Gambia" },
    { "GE", "Georgia" },
    { "DE", "Germany" },
    { "GH", "Ghana" },
    { "GI", "Gibraltar" },
    { "GR", "Greece" },
    { "GL", "Greenland" },
    { "GD", "Grenada" },
    { "GP", "Guadeloupe" },
    { "GU", "Guam" },
    { "GT", "Guatemala" },
    { "GG", "Guernsey" },
    { "GN", "Guinea" },
    { "GW", "Guinea-Bissau" },
    { "GY", "Guyana" },
    { "HT", "Haiti" },
    { "HM", "Heard Island and McDonald Islands" },
    { "VA", "Holy See" },
    { "HN", "Honduras" },
    { "HK", "Hong Kong" },
    { "HU", "Hungary" },
    { "IS", "Iceland" },
    { "IN", "India" },
    { "ID", "Indonesia" },
    { "IR", "Iran" },
    { "IQ", "Iraq" },
    { "IE", "Ireland" },
    { "IM", "Isle of Man" },
    { "IL", "Israel" },
    { "IT", "Italy" },
    { "JM", "Jamaica" },
    { "JP", "Japan" },
    { "JE", "Jersey" },
    { "JO", "Jordan" },
    { "KZ", "Kazakhstan" },
    { "KE", "Kenya" },
    { "KI", "Kiribati" },
    { "KP", "North Korea" },
    { "KR", "South Korea" },
    { "KW", "Kuwait" },
    { "KG", "Kyrgyzstan" },
    { "LA", "Lao People's Democratic Republic" },
    { "LV", "Latvia" },
    { "LB", "Lebanon" },
    { "LS", "Lesotho" },
    { "LR", "Liberia" },
    { "LY", "Libya" },
    { "LI", "Liechtenstein" },
    { "LT", "Lithuania" },
    { "LU", "Luxembourg" },
    { "MO", "Macao" },
    { "MK", "Macedonia" },
    { "MG", "Madagascar" },
    { "MW", "Malawi" },
    { "MY", "Malaysia" },
    { "MV", "Maldives" },
    { "ML", "Mali" },
    { "MT", "Malta" },
    { "MH", "Marshall Islands" },
    { "MQ", "Martinique" },
    { "MR", "Mauritania" },
    { "MU", "Mauritius" },
    { "YT", "Mayotte" },
    { "MX", "Mexico" },
    { "FM", "Micronesia" },
    { "MD", "Moldova" },
    { "MC", "Monaco" },
    { "MN", "Mongolia" },
    { "ME", "Montenegro" },
    { "MS", "Montserrat" },
    { "MA", "Morocco" },
    { "MZ", "Mozambique" },
    { "MM", "Myanmar" },
    { "NA", "Namibia" },
    { "NR", "Nauru" },
    { "NP", "Nepal" },
    { "NL", "Netherlands" },
    { "NC", "New Caledonia" },
    { "NZ", "New Zealand" },
    { "NI", "Nicaragua" },
    { "NE", "Niger" },
    { "NG", "Nigeria" },
    { "NU", "Niue" },
    { "NF", "Norfolk Island" },
    { "MP", "Northern Mariana Islands" },
    { "NO", "Norway" },
    { "OM", "Oman" },
    { "PK", "Pakistan" },
    { "PW", "Palau" },
    { "PS", "Palestine, State of" },
    { "PA", "Panama" },
    { "PG", "Papua New Guinea" },
    { "PY", "Paraguay" },
    { "PE", "Peru" },
    { "PH", "Philippines" },
    { "PN", "Pitcairn" },
    { "PL", "Poland" },
    { "PT", "Portugal" },
    { "PR", "Puerto Rico" },
    { "QA", "Qatar" },
    { "RE", "Réunion" },
    { "RO", "Romania" },
    { "RU", "Russia" },
    { "RW", "Rwanda" },
    { "BL", "Saint Barthélemy" },
    { "SH", "Saint Helena" },
    { "KN", "Saint Kitts and Nevis" },
    { "LC", "Saint Lucia" },
    { "MF", "Saint Martin (French part)" },
    { "PM", "Saint Pierre and Miquelon" },
    { "VC", "Saint Vincent and the Grenadines" },
    { "WS", "Samoa" },
    { "SM", "San Marino" },
    { "ST", "Sao Tome and Principe" },
    { "SA", "Saudi Arabia" },
    { "SN", "Senegal" },
    { "RS", "Serbia" },
    { "SC", "Seychelles" },
    { "SL", "Sierra Leone" },
    { "SG", "Singapore" },
    { "SX", "Sint Maarten (Dutch part)" },
    { "SK", "Slovakia" },
    { "SI", "Slovenia" },
    { "SB", "Solomon Islands" },
    { "SO", "Somalia" },
    { "ZA", "South Africa" },
    { "GS", "South Georgia and the South Sandwich Islands" },
    { "SS", "South Sudan" },
    { "ES", "Spain" },
    { "LK", "Sri Lanka" },
    { "SD", "Sudan" },
    { "SR", "Suriname" },
    { "SJ", "Svalbard and Jan Mayen" },
    { "SZ", "Swaziland" },
    { "SE", "Sweden" },
    { "CH", "Switzerland" },
    { "SY", "Syria" },
    { "TW", "Taiwan" },
    { "TJ", "Tajikistan" },
    { "TZ", "Tanzania" },
    { "TH", "Thailand" },
    { "TL", "Timor-Leste" },
    { "TG", "Togo" },
    { "TK", "Tokelau" },
    { "TO", "Tonga" },
    { "TT", "Trinidad and Tobago" },
    { "TN", "Tunisia" },
    { "TR", "Turkey" },
    { "TM", "Turkmenistan" },
    { "TC", "Turks and Caicos Islands" },
    { "TV", "Tuvalu" },
    { "UG", "Uganda" },
    { "UA", "Ukraine" },
    { "AE", "United Arab Emirates" },
    { "GB", "United Kingdom" },
    { "US", "United States" },
    { "UM", "United States Minor Outlying Islands" },
    { "UY", "Uruguay" },
    { "UZ", "Uzbekistan" },
    { "VU", "Vanuatu" },
    { "VE", "Venezuela" },
    { "VN", "Viet Nam" },
    { "VG", "Virgin Islands (British)" },
    { "VI", "Virgin Islands (U.S.)" },
    { "WF", "Wallis and Futuna" },
    { "EH", "Western Sahara" },
    { "YE", "Yemen" },
    { "ZM", "Zambia" },
    { "ZW", "Zimbabwe" },
    { "AX", "Åland Islands" },
};

#define NUM_COUNTRY_CODES (sizeof(country_codes) / sizeof(country_codes[0]))

// Function to perform an HTTP GET request
size_t write_callback(void *contents, size_t size, size_t nmemb, char *output) {
    strcat(output, (char *)contents);
    return size * nmemb;
}

// Function to get the full country name from the country code
const char* get_country_name(const char* country_code) {
    for (size_t i = 0; i < NUM_COUNTRY_CODES; ++i) {
        if (strcmp(country_codes[i].code, country_code) == 0) {
            return country_codes[i].name;
        }
    }
    return "Unknown";
}

// Function to fetch geolocation information and print it
void get_geolocation_info(const char *ip_or_url) {
    struct addrinfo hints, *res, *p;
    int status;
    char ip_buffer[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((status = getaddrinfo(ip_or_url, NULL, &hints, &res)) != 0) {
        printf("Error: %s\n", gai_strerror(status));
        return;
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
    char url[100];
    snprintf(url, sizeof(url), "%s%s/json", GEO_API, ip_buffer);

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

            printf("\nIP: %s\n", ip_buffer);
            printf("City: %s\n", json_object_get_string(city));
            printf("Region: %s\n", json_object_get_string(region));
            printf("Country: %s\n", get_country_name(json_object_get_string(country))); // Use full country name

            if (json_object_get_type(hostname) == json_type_null) {
                printf("Hostname: (N/A)\n");
            } else {
                printf("Hostname: %s\n", json_object_get_string(hostname));
            }

            json_object_put(json);
        } else {
            printf("Error fetching geolocation data.\n");
        }
    }
}


// Function to print the usage information
void print_usage(const char *program_name) {
    printf("Usage: %s [IP or Domain]\n", program_name);
}

int main(int argc, char *argv[]) {
    if (argc == 2 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)) {
        // Print help message and exit if -h or --help argument is provided
        print_usage(argv[0]);
        return 0;
    } else if (argc != 2) {
        // Print usage information and exit if invalid number of arguments
        print_usage(argv[0]);
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


