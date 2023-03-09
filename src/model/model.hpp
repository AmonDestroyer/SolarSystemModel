#ifndef Model_h
#define Model_h

#include "../renderManager/renderManager.hpp"
#include "../nasaClient/nasaClient.hpp"
#include <string>
#include <map>

class Model {
private:
    std::string date;
    std::map<std::string, Body *> bodys;
    long nbodys;
    NasaClient client;
public:
    Model(const std::string date);
    ~Model();

    void generateModel(RenderManager &rm);
    void setDate(std::string date);
    Body * getBody(std::string name);
};
#endif