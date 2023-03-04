#include "shaders/shaders.hpp"
#include <string.h>

const char *GetVertexShader()
{
   static char vertexShader[4096];
   strcpy(vertexShader,
           "#version 400\n"
           "layout (location = 0) in vec3 vertex_position;\n"
           "layout (location = 1) in vec3 vertex_normal;\n"
           "out float shading_amount;\n"
           "uniform mat4 MVP;\n"
           "uniform vec3 cameraloc;  // Camera position \n"
           "uniform vec3 lightdir;   // Lighting direction \n"
           "uniform vec4 lightcoeff; // Lighting coeff, Ka, Kd, Ks, alpha\n"
           "void main() {\n"
           "  gl_Position = MVP*vec4(vertex_position, 1.0);\n"
           "shading_amount = lightcoeff[0];\n"
           "float diffuse_factor = lightcoeff[1] * dot(vertex_normal, lightdir);\n"
           "shading_amount += diffuse_factor;\n"
           "vec3 reflection = 2 * vertex_normal * dot(vertex_normal, lightdir) - lightdir;\n"
           "vec3 view = cameraloc - vertex_position;\n"
           "view = normalize(view);\n"
           "float specular_factor = dot(reflection, view);\n"
           "if (specular_factor < 0) {\n"
           "specular_factor = 0;\n"
           "} else {\n"
           "specular_factor = pow(specular_factor, lightcoeff[3]) * lightcoeff[2];\n"
           "}\n"
           "shading_amount += specular_factor;\n"
           "}\n"
         );
   return vertexShader;
}

const char *GetFragmentShader()
{
   static char fragmentShader[4096];
   strcpy(fragmentShader,
           "#version 400\n"
           "uniform vec3 color;\n"
           "in float shading_amount;\n"
           "out vec4 frag_color;\n"
           "void main() {\n"
           "frag_color = vec4(color.x, color.y, color.z, 1.0) * shading_amount;\n"
           "}\n"
         );
   return fragmentShader;
}