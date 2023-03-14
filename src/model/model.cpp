#include "model.hpp"
#include <iostream>
#include <bits/stdc++.h>

//===============================================================================================================
// Constants Definition
//===============================================================================================================
std::map<std::string, glm::vec3> body_info = {
    {"Sun", glm::vec3(1, 0.80, 0.20)},
    {"Mercury", glm::vec3(0.894, 0.788, 0.6)}, 
    {"Venus", glm::vec3(0.773, 0.447, 0.133)},
    {"Earth", glm::vec3(0.204, 0.365, 0.545)},
    {"Moon", glm::vec3(1, 0.961, 0.925)},
    {"Mars", glm::vec3(0.91, 0.396, 0.227)},
    {"Jupiter", glm::vec3(0.824, 0.71, 0.518)},
    {"Saturn", glm::vec3(0.816, 0.702, 0.467)},
    {"Uranus", glm::vec3(0.031, 0.459, 0.588)},
    {"Neptune", glm::vec3(0.424, 0.561, 0.89)},
    {"JWS", glm::vec3(1, 1, 1)}};

//===============================================================================================================
// Model Class
//...............................................................................................................
// Constructor and Destructor
//...............................................................................................................
Model::Model(const std::string date) {
    this->client = new NasaClient();
    this->date = date;
    this->nbodys = 0;
    for (auto const& pair : body_info) {
        std::string name = pair.first;
        this->bodys.insert({name, new Body(name, pair.second)});
        Body *body = this->bodys[name];
        this->client->getBodyData(*body, date);
        this->nbodys++;
    }    
}

// Model::~Model() {
//     // for (auto const& pair : this->bodys) {
//     //     delete pair.second;
//     // }
// }

//...............................................................................................................
// Public Methods
//...............................................................................................................
// void Model::generateModel(RenderManager &rm) {
//     glm::mat4 identity(1.0f);

//     for (auto const& pair : this->bodys) {
//         glBindTexture(GL_TEXTURE_2D, this->textureMap.at(pair.first));
//         glm::vec3 pos = pair.second->getPos();
//         glm::mat4 translate = TranslateMatrix(pos.x, pos.y, pos.z);
//         float radius = pair.second->getRadius();
//         glm::mat4 scale = ScaleMatrix(radius, radius, radius);
//         glm::vec3 color = pair.second->getColor();
//         rm.SetColor(color.x, color.y, color.z);
//         rm.Render(RenderManager::SPHERE, identity * translate * scale);
//         // rm.Render(pos, radius, 0);
//     }
//     glBindTexture(GL_TEXTURE_2D, 0);
// }

void Model::setDate(std::string date) {
    for (auto const& pair : this->bodys) {
        this->client->getBodyData(*pair.second, date);
    }
    this->date = date;
}

std::string Model::getDate() {
    return this->date;
}

Body * Model::getBody(std::string name) {
    return this->bodys[name];
}

//===============================================================================================================
// Helper Functions
//===============================================================================================================
