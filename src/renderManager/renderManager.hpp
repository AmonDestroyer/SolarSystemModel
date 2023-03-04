#ifndef RenderManager_h
#define RenderManager_h

#include "../geometry/geometry.hpp"

#include <GL/glew.h>    // include GLEW and new version of GL on Windows
#include <GLFW/glfw3.h> // GLFW helper library

using glm::mat4;

class RenderManager {
private:
    vec3 color;
    GLuint sphereVAO;
    GLuint sphereNumPrimitives;
    GLuint cylinderVAO;
    GLuint cylinderNumPrimitives;
    GLuint mvploc;
    GLuint colorloc;
    GLuint camloc;
    GLuint ldirloc;
    mat4 projection;
    mat4 view;
    GLuint shaderProgram;
    GLFWwindow *window;
    Geometry geo;

    void SetUpWindowAndShaders();
    void MakeModelView(mat4 &model);
    
public:
    enum ShapeType
    {
        SPHERE,
        CYLINDER
    };

    RenderManager();
    void SetView(vec3 &camera, vec3 &origin, vec3 &up);
    void SetUpGeometry();
    void SetColor(double r, double g, double b);
    void Render(ShapeType, mat4 model);
    GLFWwindow   *GetWindow() { return window; };
};

#endif