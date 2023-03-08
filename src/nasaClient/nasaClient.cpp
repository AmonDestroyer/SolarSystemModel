#include "nasaClient/nasaClient.hpp"
#include <map>
#include <iostream>

using std::cout;
using std::endl;
using std::cerr;

std::map<std::string, long>objects = {
    {"Sun", 10},
    {"Mercury", 1},
    {"Venus", 2},
    {"Earth", 399},
    {"Moon", 301},
    {"Mars", 4},
    {"Jupiter", 599},
    {"Saturn", 699},
    {"Uranus", 799},
    {"Neptune", 899},
    {"JWS", -170}
};
NasaClient::NasaClient() {
}

void NasaClient::test() {
    //Testing of curl
    cerr << "Running Test" << endl;
    this->curl = curl_easy_init();
    cerr << "Initiated Curl" << endl;
    curl_easy_cleanup(this->curl);
    cerr << "Cleaned up Curl" << endl;
}
//===============================================================================================================
// Planet Class
//...............................................................................................................
// Construct
//...............................................................................................................

Planet::Planet(std::string name) {
    this->name = name;
    this->index = objects[name];
    this->dataDate = "";
}

//...............................................................................................................
// Public Functions
//...............................................................................................................

void Planet::updateData(glm::vec3 pos, float radius, std::string dataDate) {
    this->pos = pos;
    this->radius = radius;
    this->dataDate = dataDate;
}

std::string Planet::getDataDate() {
    return this->dataDate;
}

std::string Planet::getName() {
    return this->name;
}

long Planet::getIndex() {
    return this->index;
}
