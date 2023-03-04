#include "nasaClient/nasaClient.hpp"
#include <iostream>

using std::cout;
using std::endl;
using std::cerr;

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