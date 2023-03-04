#ifndef Geometry_h
#define Geometry_h

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/vec3.hpp>   // glm::vec3
#include <glm/vec4.hpp>   // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>  // glm::translate, glm::rotate, glm::scale

#include <vector>
using std::vector;
using glm::vec3;

class Triangle
{
  public:
    glm::vec3 v0;
    glm::vec3 v1;
    glm::vec3 v2;
};

class Geometry {
private:
    vector<Triangle> SplitTriangle(vector<Triangle> &list);

    void PushVertex(vector<float>& coords, const vec3& v);
public:
    Geometry();

    void GetCylinderData(vector<float>& coords, vector<float>& normals);
    void GetSphereData(vector<float>& coords, vector<float>& normals);
};

#endif