#ifndef NasaClient_h
#define NasaClient_h

#define CURL_STATICLIB
#include <curl/curl.h>

/**
 * @brief Client object used to load and define a client interface to the NASA APIs
 * 
 */
class NasaClient {
private:
    /**
     * @brief storage of the curl object
     * 
     */
    CURL* curl;
public:
    /**
     * @brief Construct a new Client object
     * 
     */
    NasaClient();

    /**
     * @brief Used to test if the client object works correctly with curl
     * 
     */
    void test();

};


#endif