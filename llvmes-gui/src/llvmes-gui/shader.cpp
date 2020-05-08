#include "llvmes-gui/shader.h"
#include "llvmes-gui/log.h"
#include "llvmes-gui/opengl.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace llvmes {
namespace gui {

static void CheckForError(unsigned int shader_id)
{
    int result;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &result);

    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(shader_id, length, &length, message);
        LLVMES_ERROR("{}, {}", message, GL_VERTEX_SHADER);
        glDeleteShader(shader_id);

        throw std::runtime_error("OpenGL - Failed to compile shader");
    }
}

ShaderProgramSource ParseShader(const char* program)
{
    enum class ShaderType { NONE = -1, VERTEX = 0, FRAGMENT = 1 };

    std::stringstream str;
    str << program;

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;

    while (std::getline(str, line)) {
        if (line == "")
            continue;
        if (line.find("#shader") != std::string::npos) {
            if (line.find("vertex") != std::string::npos)
                type = ShaderType::VERTEX;

            else if (line.find("fragment") != std::string::npos)
                type = ShaderType::FRAGMENT;
        }
        else {
            if (type == ShaderType::NONE) {
                throw std::runtime_error(
                    "Invalid shader format. Needs to contain vertex / fragments "
                    "directives.");
                return {};
            }
            ss[(int)type] << line << '\n';
        }
    }

    return {ss[0].str(), ss[1].str()};
}

Shader::Shader(const char* program)
{
    if (program == nullptr)
        throw std::runtime_error("Shader program empty or NULL");

    unsigned int id = glCreateProgram();

    if (id == 0)
        throw std::runtime_error("OpenGL - Failed to create shader program");

    ShaderProgramSource source = ParseShader(program);
    const char* src_vertex = source.VertexSource.c_str();
    const char* src_fragment = source.FragmentSource.c_str();

    // Compile vertex shader
    unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &src_vertex, nullptr);
    glCompileShader(vertex_shader);

    CheckForError(vertex_shader);

    // Compile fragment shader
    unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &src_fragment, nullptr);
    glCompileShader(fragment_shader);

    CheckForError(fragment_shader);

    glAttachShader(id, vertex_shader);
    glAttachShader(id, fragment_shader);
    glLinkProgram(id);
    glValidateProgram(id);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    unique_id = id;
}

Shader::~Shader()
{
    if (unique_id)
        glDeleteProgram(unique_id);
}

void Shader::Bind()
{
    glUseProgram(unique_id);
}

void Shader::Unbind()
{
    glUseProgram(0);
}

void Shader::SetUniform1i(const std::string& name, int v0)
{
    glUniform1i(GetUniformLocation(name), v0);
}

void Shader::SetUniform1iv(const std::string& name, int count, const int* value)
{
    glUniform1iv(GetUniformLocation(name), count, value);
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
    glUniform4f(GetUniformLocation(name), v0, v1, v2, v3);
}

void Shader::SetUniformMat4f(const std::string& name, const glm::mat4& mat)
{
    glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
}

void Shader::SetUniform1f(const std::string& name, float v0)
{
    glUniform1f(GetUniformLocation(name), v0);
}

void Shader::SetUniform3f(const std::string& name, const glm::vec3& vec3)
{
    glUniform3f(GetUniformLocation(name), vec3.x, vec3.y, vec3.z);
}

int Shader::GetUniformLocation(const std::string& name)
{
    if (uniform_cache.find(name) != uniform_cache.end())
        return uniform_cache[name];

    int location = glGetUniformLocation(unique_id, name.c_str());
    if (location == -1)
        LLVMES_WARN("Unable to set Uniform {}, doesn't exist", name);

    uniform_cache[name] = location;
    return location;
}

}  // namespace gui
}  // namespace llvmes