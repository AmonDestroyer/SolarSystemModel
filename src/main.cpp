///////////////////////////////////////////////////////////////////////////////
// main.cpp
// ========
// drawing a sphere using vertex array (glDrawElements)
// dependency: freeglut/glut
//
//  AUTHOR: Song Ho Ahn (song.ahn@gmail.com)
// CREATED: 2017-11-02
// UPDATED: 2023-03-13
///////////////////////////////////////////////////////////////////////////////

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
// #include <GL/glut.h>
#include <GL/glut.h>
#endif
#include <glm/glm.hpp>

#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>
#include <fstream>
#include <time.h>
#include <math.h>
#include "Bmp.h"
#include "Sphere.h"
#include "model/model.hpp"



// GLUT CALLBACK functions
void displayCB();
void reshapeCB(int w, int h);
void timerCB(int millisec);
void keyboardCB(unsigned char key, int x, int y);
void specialKeyboardCB(int key, int x, int y);
void mouseCB(int button, int stat, int x, int y);
void mouseMotionCB(int x, int y);

void initGL();
int  initGLUT(int argc, char **argv);
bool initSharedMem();
void clearSharedMem();
void initLights();
void setCamera(glm::vec3 cameraPos, glm::vec3 target);
void drawString(const char *str, int x, int y, float color[4], void *font);
void drawString3D(const char *str, float pos[3], float color[4], void *font);
void toOrtho();
void toPerspective(float fov, float near, float far);
GLuint loadTexture(const char* fileName, bool wrap=true);
void zoom(int dir);
void setFov(float desiredFov);
void focusCurrentBody();
void generateModel();
void getUserDateInput();

// Structs
typedef struct view {
    float fovLimits[2] = {0.001f, 45.0f};
    float fov = 45.0f;
    float near;
    float far;
    int nbodies;
    int currentBodyIndex;
    glm::vec3 camera; //Camera Pos
    glm::vec3 target; //Target Pos
    std::string visibleBodies;
    std::string date;
} View;

// constants
const int   SCREEN_WIDTH    = 1000;
const int   SCREEN_HEIGHT   = 1000;
const float CAMERA_DISTANCE = 4.0f;
const int   TEXT_WIDTH      = 8;
const int   TEXT_HEIGHT     = 13;
std::string IMAGE_PATH = "imgs/";
std::string viewableBodies[] = {
    "Earth",
    "Moon",
    "Sun",
    "Mercury",
    "Venus",
    "Mars",
    "Jupiter",
    "Saturn",
    "Uranus",
    "Neptune"
    };
std::map<std::string, std::string> texture_info = {
    {"Earth", "earth.bmp"},
    {"Moon", "moon.bmp"},
    {"Sun", "sun.bmp"},
    {"Mercury", "mercury.bmp"},
    {"Venus", "venus.bmp"},
    {"Mars", "mars.bmp"},
    {"Jupiter", "jupiter.bmp"},
    {"Saturn", "saturn.bmp"},
    {"Uranus", "uranus.bmp"},
    {"Neptune", "neptune.bmp"}
    };


// global variables
bool firstRender = true;
void *font = GLUT_BITMAP_8_BY_13;
int screenWidth;
int screenHeight;
bool mouseLeftDown;
bool mouseRightDown;
bool mouseMiddleDown;
float mouseX, mouseY;
float cameraAngleX;
float cameraAngleY;
float cameraDistance;
int drawMode;
int imageWidth;
int imageHeight;
std::string date;
Model *model;
View *view;
GLuint *textureIds;

Sphere sphere(1.0f, 36, 18);           // radius, sectors, stacks, smooth(default)



///////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
    // init global vars
    initSharedMem();

    // init GLUT and GL
    initGLUT(argc, argv);
    initGL();

    // load BMP image
    for (int i = 0; i<view->nbodies; i++) {
        std::string name = viewableBodies[i];
        Body *body = model->getBody(name);
        std::string imagePath = IMAGE_PATH + texture_info[name];
        textureIds[i] = loadTexture(imagePath.c_str());
        body->setTexId(textureIds[i]);
    }

    // the last GLUT call (LOOP)
    // window will be shown and display callback is triggered by events
    // NOTE: this call never return main().
    glutMainLoop(); /* Start GLUT event-processing loop */

    return 0;
}



///////////////////////////////////////////////////////////////////////////////
// initialize GLUT for windowing
///////////////////////////////////////////////////////////////////////////////
int initGLUT(int argc, char **argv)
{
    // GLUT stuff for windowing
    // initialization openGL window.
    // it is called before any other GLUT routine
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL);   // display mode

    glutInitWindowSize(screenWidth, screenHeight);  // window size

    glutInitWindowPosition(100, 100);               // window location

    // finally, create a window with openGL context
    // Window will not displayed until glutMainLoop() is called
    // it returns a unique ID
    int handle = glutCreateWindow(argv[0]);     // param is the title of window

    // register GLUT callback functions
    glutDisplayFunc(displayCB);
    glutTimerFunc(33, timerCB, 33);             // redraw only every given millisec
    glutReshapeFunc(reshapeCB);
    glutKeyboardFunc(keyboardCB);
    glutSpecialFunc(specialKeyboardCB);
    glutMouseFunc(mouseCB);
    glutMotionFunc(mouseMotionCB);

    return handle;
}



///////////////////////////////////////////////////////////////////////////////
// initialize OpenGL
// disable unused features
///////////////////////////////////////////////////////////////////////////////
void initGL()
{
    glShadeModel(GL_SMOOTH);                    // shading mathod: GL_SMOOTH or GL_FLAT
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);      // 4-byte pixel alignment

    // enable /disable features
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    //glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_CULL_FACE);

    // track material ambient and diffuse from surface color, call it before glEnable(GL_COLOR_MATERIAL)
    //glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    //glEnable(GL_COLOR_MATERIAL);

    glClearColor(0, 0, 0, 0);                   // background color
    glClearStencil(0);                          // clear stencil buffer
    glClearDepth(1.0f);                         // 0 is near, 1 is far
    glDepthFunc(GL_LEQUAL);

    initLights();
}



///////////////////////////////////////////////////////////////////////////////
// write 2d text using GLUT
// The projection matrix must be set to orthogonal before call this function.
///////////////////////////////////////////////////////////////////////////////
void drawString(const char *str, int x, int y, float color[4], void *font)
{
    glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT); // lighting anad color mask
    glDisable(GL_LIGHTING);     // need to disable lighting for proper text color
    glDisable(GL_TEXTURE_2D);

    glColor4fv(color);          // set text color
    glRasterPos2i(x, y);        // place text position

    // loop all characters in the string
    while(*str)
    {
        glutBitmapCharacter(font, *str);
        ++str;
    }

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
    glPopAttrib();
}



///////////////////////////////////////////////////////////////////////////////
// draw a string in 3D space
///////////////////////////////////////////////////////////////////////////////
void drawString3D(const char *str, float pos[3], float color[4], void *font)
{
    glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT); // lighting and color mask
    glDisable(GL_LIGHTING);     // need to disable lighting for proper text color
    glDisable(GL_TEXTURE_2D);

    glColor4fv(color);          // set text color
    glRasterPos3fv(pos);        // place text position

    // loop all characters in the string
    while(*str)
    {
        glutBitmapCharacter(font, *str);
        ++str;
    }

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
    glPopAttrib();
}



///////////////////////////////////////////////////////////////////////////////
// initialize global variables
///////////////////////////////////////////////////////////////////////////////
bool initSharedMem()
{
    screenWidth = SCREEN_WIDTH;
    screenHeight = SCREEN_HEIGHT;

    mouseLeftDown = mouseRightDown = mouseMiddleDown = false;
    mouseX = mouseY = 0;

    cameraAngleX = cameraAngleY = 0.0f;
    cameraDistance = CAMERA_DISTANCE;

    drawMode = 0; // 0:fill, 1: wireframe, 2:points

    view = new View();
    view->date = "2023-03-21";
    model = new Model(view->date);
    view->nbodies = end(viewableBodies) - begin(viewableBodies);
    textureIds = (GLuint *)malloc(sizeof(GLuint) * view->nbodies);
    for (int i = 0; i < view->nbodies; i++) {
        std::string name = viewableBodies[i];
    }
    
    view->currentBodyIndex = 0;
    view->camera = model->getBody("JWS")->getPos();
    view->target = model->getBody(viewableBodies[view->currentBodyIndex])->getPos();

    return true;
}



///////////////////////////////////////////////////////////////////////////////
// clean up global vars
///////////////////////////////////////////////////////////////////////////////
void clearSharedMem()
{
}



///////////////////////////////////////////////////////////////////////////////
// initialize lights
///////////////////////////////////////////////////////////////////////////////
void initLights()
{
    // set up light colors (ambient, diffuse, specular)
    GLfloat lightKa[] = {0, 0, 0, 1};           // specular light
    GLfloat lightKd[] = {.7f, .7f, .7f, 1.0f};  // diffuse light
    GLfloat lightKs[] = {0, 0, 0, 1};           // specular light
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightKa);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightKd);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightKs);

    // position the light
    glm::vec3 sunPos = model->getBody("Sun")->getPos();
    float lightPos[4] = {sunPos.x, sunPos.y, sunPos.z, 1}; // directional light
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    glEnable(GL_LIGHT0);                        // MUST enable each light source after configuration
}



///////////////////////////////////////////////////////////////////////////////
// set camera position and lookat direction
///////////////////////////////////////////////////////////////////////////////
void setCamera(glm::vec3 cameraPos, glm::vec3 target)
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(cameraPos.x, cameraPos.y, cameraPos.z, target.x, target.y, target.z, 0, 1, 0); // eye(x,y,z), focal(x,y,z), up(x,y,z)
}



///////////////////////////////////////////////////////////////////////////////
// load raw image as a texture
///////////////////////////////////////////////////////////////////////////////
GLuint loadTexture(const char* fileName, bool wrap)
{
    Image::Bmp bmp;
    if(!bmp.read(fileName))
        return 0;     // exit if failed load image
    
    // get bmp info
    int width = bmp.getWidth();
    int height = bmp.getHeight();
    const unsigned char* data = bmp.getDataRGB();
    GLenum type = GL_UNSIGNED_BYTE;    // only allow BMP with 8-bit per channel
    // We assume the image is 8-bit, 24-bit or 32-bit BMP
    GLenum format;
    int bpp = bmp.getBitCount();
    if(bpp == 8)
        format = GL_LUMINANCE;
    else if(bpp == 24)
        format = GL_RGB;
    else if(bpp == 32)
        format = GL_RGBA;
    else
        return 0;               // NOT supported, exit

    // gen texture ID
    GLuint texture;
    glGenTextures(1, &texture);
    
    // set active texture and configure it
    glBindTexture(GL_TEXTURE_2D, texture);

    // select modulate to mix texture with color for shading
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);

    // if wrap is true, the texture wraps over at the edges (repeat)
    //       ... false, the texture ends at the edges (clamp)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap ? GL_REPEAT : GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap ? GL_REPEAT : GL_CLAMP);
    //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // copy texture data
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, type, data);
    //glGenerateMipmap(GL_TEXTURE_2D);

    // build our texture mipmaps
    switch(bpp)
    {
    case 8:
        gluBuild2DMipmaps(GL_TEXTURE_2D, 1, width, height, GL_LUMINANCE, type, data);
        break;
    case 24:
        gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_RGB, type, data);
        break;
    case 32:
        gluBuild2DMipmaps(GL_TEXTURE_2D, 4, width, height, GL_RGBA, type, data);
        break;
    }

    // bmp.printSelf();
    return texture;
}



///////////////////////////////////////////////////////////////////////////////
// display info messages
///////////////////////////////////////////////////////////////////////////////
void showInfo()
{
    // backup current model-view matrix
    glPushMatrix();                     // save current modelview matrix
    glLoadIdentity();                   // reset modelview matrix

    // set to 2D orthogonal projection
    glMatrixMode(GL_PROJECTION);        // switch to projection matrix
    glPushMatrix();                     // save current projection matrix
    glLoadIdentity();                   // reset projection matrix
    //gluOrtho2D(0, screenWidth, 0, screenHeight); // set to orthogonal projection
    glOrtho(0, screenWidth, 0, screenHeight, -1, 1); // set to orthogonal projection

    float color[4] = {1, 1, 1, 1};

    std::stringstream ss;
    ss << std::fixed << std::setprecision(3);

    int line = 1;

    ss << "Date: " << view->date << std::ends;
    drawString(ss.str().c_str(), 1, screenHeight-(line++ * TEXT_HEIGHT), color, font);
    ss.str("");

    ss << "Current Target: " << viewableBodies[view->currentBodyIndex] << std::ends;
    drawString(ss.str().c_str(), 1, screenHeight-(line++ * TEXT_HEIGHT), color, font);
    ss.str("");

    ss << "FOV (deg): " << view->fov << std::ends;
    drawString(ss.str().c_str(), 1, screenHeight-(line++ * TEXT_HEIGHT), color, font);
    ss.str("");

    ss << "Visible Bodies: " << view->visibleBodies << std::ends;
    drawString(ss.str().c_str(), 1, screenHeight-(line++ * TEXT_HEIGHT), color, font);
    ss.str("");


    // Controls
    line++; // Add Blank line
    ss << "CONTROLS" << std::ends;
    drawString(ss.str().c_str(), 1, screenHeight-(line++ * TEXT_HEIGHT), color, font);
    ss.str("");

    ss << "Scroll Wheel = Zoom" << std::ends;
    drawString(ss.str().c_str(), 1, screenHeight-(line++ * TEXT_HEIGHT), color, font);
    ss.str("");

    ss << "Left/Right Arrow = Change Target" << std::ends;
    drawString(ss.str().c_str(), 1, screenHeight-(line++ * TEXT_HEIGHT), color, font);
    ss.str("");

    ss << "` = Change Date" << std::ends;
    drawString(ss.str().c_str(), 1, screenHeight-(line++ * TEXT_HEIGHT), color, font);
    ss.str("");

    ss << "Space = Refocus to Target" << std::ends;
    drawString(ss.str().c_str(), 1, screenHeight-(line++ * TEXT_HEIGHT), color, font);
    ss.str("");

    // unset floating format
    ss << std::resetiosflags(std::ios_base::fixed | std::ios_base::floatfield);

    // restore projection matrix
    glPopMatrix();                   // restore to previous projection matrix

    // restore modelview matrix
    glMatrixMode(GL_MODELVIEW);      // switch to modelview matrix
    glPopMatrix();                   // restore to previous modelview matrix
}



///////////////////////////////////////////////////////////////////////////////
// set projection matrix as orthogonal
///////////////////////////////////////////////////////////////////////////////
void toOrtho()
{
    // set viewport to be the entire window
    glViewport(0, 0, (GLsizei)screenWidth, (GLsizei)screenHeight);

    // set orthographic viewing frustum
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, screenWidth, 0, screenHeight, -1, 1);

    // switch to modelview matrix in order to set scene
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}



///////////////////////////////////////////////////////////////////////////////
// set the projection matrix as perspective
///////////////////////////////////////////////////////////////////////////////
void toPerspective(float fov, float near, float far)
{
    // set viewport to be the entire window
    glViewport(0, 0, (GLsizei)screenWidth, (GLsizei)screenHeight);

    // set perspective viewing frustum
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fov, (float)(screenWidth)/screenHeight, near, far); // FOV, AspectRatio, NearClip, FarClip

    // switch to modelview matrix in order to set scene
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}









//=============================================================================
// CALLBACKS
//=============================================================================

void displayCB()
{
    // Get Textures
    if (firstRender) {
        firstRender = false;
        focusCurrentBody();
    }

    // clear buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // save the initial ModelView matrix before modifying ModelView matrix
    glPushMatrix();

    //Set the camera position and look direction
    setCamera(view->camera, view->target);
    

    // set material
    float ambient[]  = {0.1f, 0.1f, 0.1f, 1};
    float diffuse[]  = {0.7f, 0.7f, 0.7f, 1};
    float specular[] = {1.0f, 1.0f, 1.0f, 1};
    float shininess  = 32;
    glMaterialfv(GL_FRONT, GL_AMBIENT,   ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR,  specular);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);

    // // draw right sphere with texture
    generateModel();

    showInfo();     // print max range of glDrawRangeElements

    glPopMatrix();

    glutSwapBuffers();
}


void reshapeCB(int w, int h)
{
    screenWidth = w;
    screenHeight = h;
    toPerspective(view->fov, view->near, view->far);
    std::cout << "window resized: " << w << " x " << h << std::endl;

#ifdef _WIN32
    HWND handle = ::GetActiveWindow();
    RECT rect;
    ::GetWindowRect(handle, &rect); // with non-client area; border, titlebar etc.
    std::cout << "=========================" << std::endl;
    std::cout << "full window size with border: " << (rect.right - rect.left) << "x" << (rect.bottom - rect.top) << std::endl;
    ::GetClientRect(handle, &rect); // only client dimension
    std::cout << "client window size: " << (rect.right - rect.left) << "x" << (rect.bottom - rect.top) << std::endl;
    std::cout << "=========================" << std::endl;
#endif
}


void timerCB(int millisec)
{
    glutTimerFunc(millisec, timerCB, millisec);
    glutPostRedisplay();
}


void keyboardCB(unsigned char key, int x, int y)
{
    switch(key)
    {
    case 27: // ESCAPE
        clearSharedMem();
        exit(0);
        break;

    case 'd': // switch rendering modes (fill -> wire -> point)
    case 'D':
        ++drawMode;
        drawMode %= 3;
        if(drawMode == 0)        // fill mode
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);
        }
        else if(drawMode == 1)  // wireframe mode
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_CULL_FACE);
        }
        else                    // point mode
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_CULL_FACE);
        }
        break;
    case '`':
        getUserDateInput();
        break;

    case ' ':
        focusCurrentBody();
        break;

    default:
        ;
    }
}

void specialKeyboardCB(int key, int x, int y) {
    switch(key)
    {
        // Next Selection
        case GLUT_KEY_RIGHT:
        view->currentBodyIndex++;
        view->currentBodyIndex %= view->nbodies;
        focusCurrentBody();
        break;
        // Previous Selection
        case GLUT_KEY_LEFT:
        view->currentBodyIndex += view->nbodies - 1;
        view->currentBodyIndex %= view->nbodies;
        focusCurrentBody();
        break;
    }
}


void mouseCB(int button, int state, int x, int y)
{
    mouseX = x;
    mouseY = y;

    if(button == GLUT_LEFT_BUTTON)
    {
        if(state == GLUT_DOWN)
        {
            mouseLeftDown = true;
        }
        else if(state == GLUT_UP)
            mouseLeftDown = false;
    }

    else if(button == GLUT_RIGHT_BUTTON)
    {
        if(state == GLUT_DOWN)
        {
            mouseRightDown = true;
        }
        else if(state == GLUT_UP)
            mouseRightDown = false;
    }

    else if(button == GLUT_MIDDLE_BUTTON)
    {
        if(state == GLUT_DOWN)
        {
            mouseMiddleDown = true;
        }
        else if(state == GLUT_UP)
            mouseMiddleDown = false;
    }

    else if (button == 3) {
        //Wheel down (zoom in)
        zoom(-1);
    } else if (button == 4) {
        //Wheel up (zoom out)
        zoom(1);
    }
}


void mouseMotionCB(int x, int y)
{
    if(mouseLeftDown)
    {
        // cameraAngleY += (x - mouseX);
        // cameraAngleX += (y - mouseY);
        mouseX = x;
        mouseY = y;
    }
    if(mouseRightDown)
    {
        // cameraDistance -= (y - mouseY) * 0.2f;
        mouseY = y;
    }
}

//=============================================================================
// HELPERS
//=============================================================================

void zoom(int dir) {
    float sensitivity = 0.1f;
    // cameraDistance -= dir * sensitivity;
    float desiredFov = view->fov * (1 + dir*sensitivity);
    setFov(desiredFov);
}

void setFov(float desiredFov) {
    if (desiredFov < view->fovLimits[0])
        desiredFov = view->fovLimits[0];
    if (desiredFov > view->fovLimits[1])
        desiredFov = view->fovLimits[1];
    view->fov = desiredFov;
}

void focusCurrentBody() {    
    std::string targetName = viewableBodies[view->currentBodyIndex];
    Body *target = model->getBody(targetName);
    view->target = target->getPos();
    glm::vec3 vecCameraTarget = view->target - view->camera;
    float desiredFov = glm::degrees(atan2(4*target->getRadius(), glm::length(vecCameraTarget)));
    setFov(desiredFov);
}


void generateModel() {
    std::string targetName = viewableBodies[view->currentBodyIndex];
    Body *target = model->getBody(targetName);
    glm::vec3 vecCameraTarget = target->getPos() - view->camera;
    glm::vec3 normVecCameraTarget = glm::normalize(vecCameraTarget);
    view->visibleBodies = targetName;

    float near = glm::length(vecCameraTarget) - 2 * target->getRadius();
    float far = glm::length(vecCameraTarget) + 2 * target->getRadius();
    for (int i = 0; i < view->nbodies; i++) {
        std::string name = viewableBodies[i];
        Body *body = model->getBody(name);
        glm::vec3 bodyPos = body->getPos();

        glm::vec3 vecCameraBody = bodyPos - view->camera;
        glm::vec3 normVecCameraBody = glm::normalize(vecCameraBody);

        float dotProduct = glm::dot(normVecCameraBody, normVecCameraTarget);
        if (dotProduct >= 1)
            dotProduct = 1;
        else if (dotProduct <= -1)
            dotProduct = M_PI;

        float theta = glm::degrees(acos(dotProduct));

        //Deal with floating point error calcualtion
        if (name == targetName) {
            theta = 0;
        }

        if ((theta >= 0) && (theta < (view->fov/2))) {
            if (name != targetName) {
                view->visibleBodies += ", " + name;
            }

            //Model item and apply texture
            float bodyRadius = body->getRadius();
            GLuint texId = textureIds[i];
            glPushMatrix();
            glTranslatef(bodyPos.x, bodyPos.y, bodyPos.z);
            glRotatef(-90, 0, 1, 0);
            glBindTexture(GL_TEXTURE_2D, texId);
            sphere.setRadius(bodyRadius);
            sphere.draw();
            glPopMatrix();

            // Update Frustom
            // Project body vector onto target normal vector
            float projDistance = glm::dot(vecCameraBody, normVecCameraTarget);
            if (projDistance < near)
                near = projDistance - 3*bodyRadius;
            else if (projDistance > far)
                far = projDistance + 3*bodyRadius;
        }
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    view->near = near;
    view->far = far;
    toPerspective(view->fov, near, far);
}

void getUserDateInput() {
    std::string desiredDate;
    std::cout << "Enter a Desired Date (yyyy-mm-dd): ";
    std::cin >> desiredDate;

    view->date = desiredDate;
    model->setDate(view->date);

    focusCurrentBody();
}