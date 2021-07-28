#ifndef SHADER_HPP
#define SHADER_HPP

bool loadShaders(const char *vertexShaderPath, const char *fragmentShaderPath);
void useShader();
void setUniformMatrix4fv(const char *name, glm::mat4 uniform);
void setUniformVec3fv(const char *name, glm::vec3 uniform);


#endif
