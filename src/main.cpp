#include <iostream>
#include <stdio.h>

#include <stdlib.h>
#include <vector>
#include <math.h>
#include <string>

#include "model/model.hpp"
#include "nasaClient/nasaClient.hpp"
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

int width = 700;
int height = 700;
bool firstMouse = true;
float yaw = 0.0f;
float pitch = 0.0f;
float lastX = (float)width / 2.0;
float lastY = (float)height / 2.0;
float fov = 45.0f;
glm::vec3 direction(0, 0, 0);

//
// main function
//

int main() 
{
  //Testing of curl
  std::string date = "2023-03-08";
  NasaClient client;
  client.test();
  Model model("2023-03-08");
  RenderManager rm;
  GLFWwindow *window = rm.GetWindow();

  glm::vec3 up(0, 1, 0);
  int counter=0;

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); 
  glfwSetCursorPosCallback(window, mouse_callback);  
  glfwSetScrollCallback(window, scroll_callback);

    Body *jws = model.getBody("JWS");
    glm::vec3 camera = jws->getPos();
    Body *earth = model.getBody("Earth");
    glm::vec3 origin = earth->getPos();
    direction = glm::normalize(origin - camera);
    setInitialView(direction);
  
  while (!glfwWindowShouldClose(window)) {
    double angle=counter/2000.0*2*M_PI;
    counter++;
    //angle = 3*M_PI/8;

    
    glm::vec3 lookDir = glm::normalize(direction);
    // TODO: The far plane for the camera is set to 100, I need to figure out where this is set so I can
    // Update that value to be rediculously large.
    rm.SetView(camera, origin, up, lookDir);
    rm.updateProjection(fov);
    //std::cerr << "(origin) [fov]: (" << origin.x << ", " << origin.y << ", " << origin.z << ") [" << fov << "]" << endl;

    // wipe the drawing surface clear
    glClearColor(0.3, 0.3, 0.8, 1.0);
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

// TODO
// Far plane is set to 100, need to figure out how to increase that by a lot and adjust based on input.
// Mouse movement needs to set desired look direction relative to the camera position

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    fov -= (float)yoffset;
    if (fov < 0.001f)
        fov = 0.001f;
    if (fov > 45.0f)
        fov = 45.0f; 
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

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

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