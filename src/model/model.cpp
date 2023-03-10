#include "model/model.hpp"
#include <iostream>

//===============================================================================================================
// Helper Function Definition
//===============================================================================================================
void SetUpEyeball(glm::mat4 modelSoFar, RenderManager &rm, bool right);
void SetUpTopMouth(glm::mat4 modelSoFar, RenderManager& rm);
void SetupBotMouth(glm::mat4 modelSoFar, RenderManager& rm);
void SetUpEar(glm::mat4 modelSoFar, RenderManager& rm, bool is_left=false);
void SetUpHead(glm::mat4 modelSoFar, RenderManager &rm, double var);
void SetUpNeck(glm::mat4 modelSoFar, RenderManager& rm);
void SetUpLeg(glm::mat4 modelSoFar, RenderManager& rm);
void SetUpLegs(glm::mat4 modelSoFar, RenderManager& rm, bool back);
void SetUpCenterBody(glm::mat4 modelSoFar, RenderManager& rm, double offset);
void SetUpBody(glm::mat4 modelSoFar, RenderManager& rm, double var);

// Transformation Matricies
glm::mat4 RotateMatrix(float degrees, float x, float y, float z);
glm::mat4 ScaleMatrix(double x, double y, double z);
glm::mat4 TranslateMatrix(double x, double y, double z);

std::string body_names[] = {"Sun",
                            "Mercury", 
                            "Venus",
                            "Earth",
                            "Moon",
                            "Mars",
                            "Jupiter",
                            "Saturn",
                            "Uranus",
                            "Neptune",
                            "JWS"};

//===============================================================================================================
// Model Class
//...............................................................................................................
// Constructor and Destructor
//...............................................................................................................
Model::Model(const std::string date) {
    this->date = date;
    this->nbodys = end(body_names) - begin(body_names);
    for (long i = 0; i < this->nbodys; i++) {
        std::string name = body_names[i];
        this->bodys.insert({name, new Body(name)});
        this->client.getBodyData(*this->bodys[name], date);
        glm::vec3 pos = this->bodys[name]->getPos();
    }
}

Model::~Model() {
    for (auto const& pair : this->bodys) {
        delete pair.second;
    }
}

//...............................................................................................................
// Public Methods
//...............................................................................................................
void Model::generateModel(RenderManager &rm) {
    glm::mat4 identity(1.0f);

    for (auto const& pair : this->bodys) {
        glm::vec3 pos = pair.second->getPos();
        glm::mat4 translate = TranslateMatrix(pos.x, pos.y, pos.z);
        float radius = pair.second->getRadius();
        glm::mat4 scale = ScaleMatrix(radius, radius, radius);
        glm::vec3 color = pair.second->getColor();
        rm.SetColor(color.x, color.y, color.z);
        rm.Render(RenderManager::SPHERE, identity * translate * scale);
    }
}

void Model::setDate(std::string date) {
    int nbodys = end(body_names) - begin(body_names);
    for (auto const& pair : this->bodys) {
        this->client.getBodyData(*pair.second, date);
    }
}

Body * Model::getBody(std::string name) {
    return this->bodys[name];
}

//===============================================================================================================
// Helper Functions
//===============================================================================================================
glm::mat4 RotateMatrix(float degrees, float x, float y, float z)
{
   glm::mat4 identity(1.0f);
   glm::mat4 rotation = glm::rotate(identity, 
                                    glm::radians(degrees), 
                                    glm::vec3(x, y, z));
   return rotation;
}

glm::mat4 ScaleMatrix(double x, double y, double z)
{
   glm::mat4 identity(1.0f);
   glm::vec3 scale(x, y, z);
   return glm::scale(identity, scale);
}

glm::mat4 TranslateMatrix(double x, double y, double z)
{
   glm::mat4 identity(1.0f);
   glm::vec3 translate(x, y, z);
   return glm::translate(identity, translate);
}