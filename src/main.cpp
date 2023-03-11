#include <iostream>
#include <stdio.h>
#include <ctime>

#include <stdlib.h>
#include <vector>
#include <math.h>
#include <string>

#include "model/model.hpp"
#include "renderManager/renderManager.hpp"
#include "shaders/shaders.hpp"

using std::endl;
using std::cerr;

// Function Prototypes 

const char *GetVertexShader();
const char *GetFragmentShader();
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void setInitialView(glm::vec3 viewDirection);
void escMenu(int value);

// Structures
typedef struct view{
    std::string bodyName;
    float fov;
    view(std::string name, float fov) {
        this->bodyName = name;
        this->fov = fov;
    }
} View;

// Global Variables
int width = 700;
int height = 700;
bool firstMouse = true;
float yaw = 0.0f;
float pitch = 0.0f;
float lastX = (float)width / 2.0;
float lastY = (float)height / 2.0;
float fov = 45.0f;
glm::vec3 direction(0, 0, 0);

View *views[] = {new View("Earth", 2),
                new View("Moon", 0.35f),
                new View("Sun", 1.25),
                new View("Mercury", 0.005),
                new View("Venus", 0.00488),
                new View("Mars", 0.032f),
                new View("Jupiter", 0.0195f),
                new View("Saturn", 0.0097f),
                new View("Uranus", 0.002f),
                new View("Neptune", 0.002f)
                };
// NOTE: The first view is the initial target set in the model

//
// main function
//

int main(int argc, char **argv) 
{
    //Testing of curl
    std::string date = "2023-03-08";
    Model model("2023-03-08");
    RenderManager rm;
    GLFWwindow *window = rm.GetWindow();

    glm::vec3 up(0, 1, 0);
    int counter=0;

//   glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); 
//   glfwSetCursorPosCallback(window, mouse_callback);  
    glfwSetScrollCallback(window, scroll_callback);

    
    long totalBodies = sizeof(views)/sizeof(View *);
    long currentBodyIndex = 0;

    Body *jws = model.getBody("JWS");
    glm::vec3 camera = jws->getPos();
    Body *sun = model.getBody("Sun");
    rm.updateLightLoc(sun->getPos());
    Body *currentBody = model.getBody(views[currentBodyIndex]->bodyName);
    glm::vec3 origin = currentBody->getPos();
    direction = origin - camera;
    setInitialView(direction);
    fov = views[currentBodyIndex]->fov;
    float near = glm::length(direction) - 2*currentBody->getRadius();
    float far = near + 4*currentBody->getRadius();
  
    std::time_t tic = std::time(nullptr);
    std::cerr << "Total Bodies to View: " << sizeof(views)/sizeof(View *) << std::endl;

    while (!glfwWindowShouldClose(window)) {
        double angle=counter/2000.0*2*M_PI;
        counter++;
        //angle = 3*M_PI/8;
        std::time_t toc = std::time(nullptr);
        if ((toc - tic) > 2) {
            View *nextView = views[++currentBodyIndex % totalBodies];
            currentBody = model.getBody(nextView->bodyName);
            origin = currentBody->getPos();
            fov = nextView->fov;
            tic = std::time(nullptr);
            glm::vec3 color = currentBody->getColor();
            std::cerr << "Now Showing: " << nextView->bodyName << " (" << color.x << "," << color.y << "," << color.z << ")" << std::endl;
            direction = origin - camera;
            near = glm::length(direction) - 2*currentBody->getRadius();
            far = near + 4*currentBody->getRadius();
        }
        
        rm.SetView(camera, origin, up, direction);
        rm.updateProjection(fov, near);
        //std::cerr << "(origin) [fov]: (" << origin.x << ", " << origin.y << ", " << origin.z << ") [" << fov << "]" << endl;

        // wipe the drawing surface clear
        glClearColor(0, 0, 0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        model.generateModel(rm);

        // update other events like input handling
        glfwPollEvents();
        // put the stuff we've been drawing onto the display
        glfwSwapBuffers(window);
    }
    glfwTerminate();
    return 0;
}

// Utilized Libraries
// curl
// json: https://github.com/nlohmann/json
// GLEW
// OpenGL
// glfw3
// glm

// References
// Mouse Input: https://learnopengl.com/Getting-started/Camera


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    float zoomSensitivity = 2.0f;
    if (yoffset > 0) {
        fov /= zoomSensitivity;
    }
    else {
        fov *= zoomSensitivity;
    }
    if (fov < 0.001f)
        fov = 0.001f;
    if (fov > 45.0f)
        fov = 45.0f; 

    std::cerr << "fov: " << fov << std::endl;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
        return;
    }
  
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; 
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 1000.0f;
    xoffset /= sensitivity;
    yoffset /= sensitivity;

    yaw   += xoffset;
    pitch += yoffset;

    if(pitch > 89.0f)
        pitch = 89.0f;
    if(pitch < -89.0f)
        pitch = -89.0f;

    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction = glm::normalize(direction);
}  

void setInitialView(glm::vec3 viewDirection) {
    viewDirection = glm::normalize(viewDirection);
    pitch = glm::degrees(asin(-viewDirection.y));
    yaw = glm::degrees(atan2(viewDirection.x, viewDirection.z));
}

void escMenu(int value) {
    std::cerr << "Menu Item Selected: " << value << std::endl;
}

void changeOrigin(std::string name) {
//     Body *body = model.getBody(name);
//     origin = body->getPos();
}