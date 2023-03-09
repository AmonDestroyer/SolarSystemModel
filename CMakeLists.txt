cmake_minimum_required(VERSION 2.8.12.1) 
 
#####################################################################
# geometry LIBRARY
#####################################################################
PROJECT(geometry)

add_library(${PROJECT_NAME}
  src/geometry/geometry.cpp)

add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
  PUBLIC include
  PRIVATE src)

if(APPLE) 
target_link_libraries(${PROJECT_NAME}
  PUBLIC GLEW::glew_s)
else() 
target_link_libraries(${PROJECT_NAME}
  PUBLIC GLEW)
endif()

# #####################################################################
# # renderManager LIBRARY
# #####################################################################
PROJECT(renderManager)

add_library(${PROJECT_NAME}
  src/${PROJECT_NAME}/${PROJECT_NAME}.cpp)

add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
  PUBLIC include
  PRIVATE src)

target_link_libraries(${PROJECT_NAME}
  PUBLIC glfw ${OPENGL_gl_LIBRARY}
  PRIVATE geometry)

#####################################################################
# nasaClient LIBRARY
#####################################################################
PROJECT(nasaClient) 
add_library(${PROJECT_NAME}
  src/nasaClient/nasaClient.cpp)

add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
  PUBLIC include
  PRIVATE src)

target_link_libraries(${PROJECT_NAME}
  PUBLIC curl)


#####################################################################
# shaders LIBRARY
#####################################################################
PROJECT(shaders) 
add_library(${PROJECT_NAME}
  src/${PROJECT_NAME}/${PROJECT_NAME}.cpp)

add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
  PUBLIC include
  PRIVATE src)

# #####################################################################
# # model LIBRARY
# #####################################################################
PROJECT(model)

add_library(${PROJECT_NAME}
  src/${PROJECT_NAME}/${PROJECT_NAME}.cpp)

add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
  PUBLIC include
  PRIVATE src)

target_link_libraries(${PROJECT_NAME}
  PUBLIC glfw ${OPENGL_gl_LIBRARY}
  PRIVATE renderManager)

#####################################################################
# Space Executable
#####################################################################
PROJECT(space) 
 
SET(OpenGL_GL_PREFERENCE LEGACY) 
 
find_package(OpenGL REQUIRED) 
find_package(glfw3  REQUIRED) 
find_package(GLEW   REQUIRED) 
find_package(glm   REQUIRED) 
 
message("GL include dir: ${OPENGL_INCLUDE_DIR}") 
message("GL link libraries: ${OPENGL_gl_LIBRARY}") 


add_executable(${PROJECT_NAME} src/main.cpp) 
target_link_libraries(${PROJECT_NAME}
  PUBLIC ${OPENGL_gl_LIBRARY} 
  PRIVATE nasaClient renderManager shaders model) 

