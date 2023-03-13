#include "nasaClient.hpp"
#include "json.hpp"

#include <map>
#include <iostream>
#include <stdlib.h>
#include <regex>

using json = nlohmann::json;
using std::cout;
using std::endl;
using std::cerr;

//===============================================================================================================
// Helper Function Definition
//===============================================================================================================

static size_t cb(void *data, size_t size, size_t nmemb, void *clientp);
struct memory {
  char *response;
  size_t size;
};

std::map<std::string, long>objects = {
    {"Sun", 10},
    {"Mercury", 1},
    {"Venus", 2},
    {"Earth", 399},
    {"Moon", 301},
    {"Mars", 499},
    {"Jupiter", 599},
    {"Saturn", 699},
    {"Uranus", 799},
    {"Neptune", 899},
    {"JWS", -170}
};

//===============================================================================================================
// NasaClient Class
//...............................................................................................................
// Constructor and Destructor
//...............................................................................................................
NasaClient::NasaClient() {
    this->curl = curl_easy_init();
}

NasaClient::~NasaClient() {
    curl_easy_cleanup(this->curl);
}


//...............................................................................................................
// Public Methods
//...............................................................................................................
void NasaClient::setEndpoint(std::string endpoint) {
    this->endpoint = endpoint;
}

// Public Methods
void NasaClient::setKey(std::string key) {
    this->apiKey = key;
}

void NasaClient::getBodyData(Body &body, std::string date) {
    if (body.getDataDate() == date)
        return;
    struct memory chunk = {0};
    std::string juliandDate = this->getJulianDate(date);
    std::string endpoint = "https://ssd.jpl.nasa.gov/api/horizons.api?COMMAND='" + std::to_string(body.getIndex()) + "'" +
                            "&EPHEM_TYPE='VECTORS'" +
                            "&START_TIME='JD" + juliandDate.c_str() + "'" +
                            "&STOP_TIME='JD" + std::to_string(atol(juliandDate.c_str()) + 1) + "'" +
                            "&STEP_SIZE='1d'";
    curl_easy_setopt(this->curl, CURLOPT_URL, endpoint.c_str());
    curl_easy_setopt(this->curl, CURLOPT_WRITEFUNCTION, cb);
    curl_easy_setopt(this->curl, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_perform(curl);
    json data = json::parse(chunk.response);
    free(chunk.response);
    std::string result = data["result"];

    // Get Position Data
    size_t index[4];
    index[0] = result.find("X =");
    index[1] = result.find("Y =");
    index[2] = result.find("Z =");
    index[3] = result.find("VX=");
    char values[3][BUFSIZ];
    for (int i=0; i<3; i++) {
        int offset = 3;
        int additionalOffset = 1;
        if (i == 2)
            additionalOffset ++;
        int len = index[i+1] - index[i] - (offset + additionalOffset);
        result.copy(values[i], len, index[i]+offset);
    }
    glm::vec3 pos(atof(values[0]), atof(values[1]), atof(values[2]));

    // Get Physical Data
    //Radius
    float radius = 0;
    if (body.getIndex() > 0) {
        std::string radiusTag[3] = {"radius", "= ", " "};
        size_t radiusIndex[3];
        radiusIndex[0] = result.find(radiusTag[0]);
        radiusIndex[1] = result.find(radiusTag[1], radiusIndex[0]) + radiusTag[1].length();
        radiusIndex[2] = result.find(radiusTag[2], radiusIndex[1]);
        char radiusText[BUFSIZ];
        result.copy(radiusText, radiusIndex[2] - radiusIndex[1], radiusIndex[1]);
        radius = atof(radiusText);
        if (radius == 0) {
            std::string secondaryTag[3] = {"Radius (km) ", "=  ", " "};
            radiusIndex[0] = result.find(secondaryTag[0]);
            radiusIndex[1] = result.find(secondaryTag[1], radiusIndex[0]) + secondaryTag[1].length();
            radiusIndex[2] = result.find(secondaryTag[2], radiusIndex[1]);
            char radiusText[BUFSIZ];
            result.copy(radiusText, radiusIndex[2] - radiusIndex[1], radiusIndex[1]);
            radius = atof(radiusText);
        }
    }

    //Update Data
    body.updateData(pos, radius, date);
}

void NasaClient::test() {
    //Testing of curl
    cerr << "Running Test" << endl;
    std::string testDate = "2000-01-01_12:00";
    std::string julianDate = this->getJulianDate(testDate);
    cerr << "Converted Julian Date: " << julianDate << endl;
    std::string calendarDate = this->getCalendarDate(std::to_string(atol(julianDate.c_str()) + 1));
    cerr << "Converted provided Julian Date to Calendar Date: " << calendarDate << endl;
    Body sun("Sun");
    this->getBodyData(sun, "2023-03-07");
    Body jws("JWS");
    this->getBodyData(jws, "2023-03-07");
    cerr << "Cleaned up Curl" << endl;
}

//...............................................................................................................
// Private Methods
// Judlian Date API: https://ssd-api.jpl.nasa.gov/doc/jd_cal.html
// Horizon API: https://ssd-api.jpl.nasa.gov/doc/horizons.html
// Horizon Application: https://ssd.jpl.nasa.gov/horizons/app.html#/
//...............................................................................................................

std::string NasaClient::getJulianDate(std::string calendarDate) {
    return convertDate(calendarDate, "cd", "jd");
}

std::string NasaClient::getCalendarDate(std::string julianDate) {
    return convertDate(julianDate, "jd", "cd");
}

std::string NasaClient::convertDate(std::string date, std::string dateType, std::string returnDateType) {
    struct memory chunk = {0};
    std::string converterEndpoint = "https://ssd-api.jpl.nasa.gov/jd_cal.api?" + dateType + "=" + date;
    // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    curl_easy_setopt(this->curl, CURLOPT_URL, converterEndpoint.c_str());
    curl_easy_setopt(this->curl, CURLOPT_WRITEFUNCTION, cb);
    curl_easy_setopt(this->curl, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_perform(curl);
    json data = json::parse(chunk.response);
    free(chunk.response);
    return data[returnDateType];
}

//===============================================================================================================
// Body Class
//...............................................................................................................
// Construct
//...............................................................................................................

Body::Body(std::string name, glm::vec3 color) {
    this->name = name;
    this->index = objects[name];
    this->dataDate = "";
    this->color = color;
}

//...............................................................................................................
// Public Functions
//...............................................................................................................

void Body::updateData(glm::vec3 pos, float radius, std::string dataDate) {
    this->pos = pos;
    this->radius = radius;
    this->dataDate = dataDate;
}

std::string Body::getDataDate() {
    return this->dataDate;
}

std::string Body::getName() {
    return this->name;
}

long Body::getIndex() {
    return this->index;
}

glm::vec3 Body::getPos() {
    return this->pos;
}

float Body::getRadius() {
    return this->radius;
}

void Body::setColor(glm::vec3 color) {
    this->color = color;
}

glm::vec3 Body::getColor() {
    return this->color;
}

void Body::setTexId(GLuint texId) {
    this->texId = texId;
}

GLuint Body::getTexId() {
    return this->texId;
}

//===============================================================================================================
// Helper Functions
//===============================================================================================================
static size_t cb(void *data, size_t size, size_t nmemb, void *clientp)
{
  size_t realsize = size * nmemb;
  struct memory *mem = (struct memory *)clientp;
 
  char *ptr = (char *)realloc(mem->response, mem->size + realsize + 1);
  if(ptr == NULL)
    return 0;  /* out of memory! */
 
  mem->response = ptr;
  memcpy(&(mem->response[mem->size]), data, realsize);
  mem->size += realsize;
  mem->response[mem->size] = 0;
 
  return realsize;
}



// Below is code use to check for curl run errors and to print them if they arrise
// char errbuf[CURL_ERROR_SIZE];
    // if (res != CURLE_OK)
    // {
    //     size_t len = std::strlen(errbuf);
    //     fprintf(stderr, "\nlibcurl: (%d) ", res);
    //     if(len)
    //         fprintf(stderr, "%s%s", errbuf,
    //                 ((errbuf[len - 1] != '\n') ? "\n" : ""));
    //     else
    //         fprintf(stderr, "%s\n", curl_easy_strerror(res));
    // }