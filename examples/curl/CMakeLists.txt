cmake_minimum_required(VERSION 2.8.12.1) 
 
PROJECT(curl) 
 
SET(OpenGL_GL_PREFERENCE LEGACY) 
 
find_package(OpenGL REQUIRED) 
find_package(glfw3  REQUIRED) 
find_package(GLEW   REQUIRED) 
find_package(glm   REQUIRED) 
find_package(curl   REQUIRED) 
 
message("GL include dir: ${OPENGL_INCLUDE_DIR}") 
message("GL link libraries: ${OPENGL_gl_LIBRARY}") 
 
add_executable(curl curl.cpp) 
if(APPLE) 
  target_link_libraries(curl ${OPENGL_gl_LIBRARY} GLEW::glew_s glfw) 
else() 
  target_link_libraries(curl ${OPENGL_gl_LIBRARY} GLEW glfw) 
endif()
