#pragma once

#include <glad/glad.h>

#include <string>

/// @brief A wrapper class for OpenGL's shader program. In this project, we will 
///        only need vertex shader, fragment shader, and compute shader.
class ShaderProgram
{
private:
    /// @brief The ID of a linked shader program.
    GLuint m_id;

    /// @brief Read the content of a file into a string. This function is used to read
    ///        shader source code files
    /// @param filepath the path of a file
    /// @return the content of the file as a string
    static std::string readFile(const char* filepath);

    /// @brief Compile the shader written in the given file
    /// @param shaderType The type of shader, GL_VERTEX_SHADER, etc.
    /// @param shaderPath The path of shader source file
    /// @return the shader ID
    static GLuint compileShader(GLenum shaderType, const char* shaderPath);

    /// @brief Link the compute shader into a program
    /// @param computePath 
    /// @return the program ID
    static GLuint linkProgram(const char* computePath);

    /// @brief Link the vertex shader and fragment shader into a program
    /// @param vertexPath the path of vertex shader source file
    /// @param fragmentPath the path of fragment shader source file
    /// @return the program ID
    static GLuint linkProgram(const char* vertexPath, const char* fragmentPath);

    /// @brief Whether the shader is compiled successfully
    /// @param shader the shader ID
    static GLint compileSuccess(GLuint shader, const char* shaderPath = "");

    /// @brief Whether the program is linked successfully
    /// @param program the program ID
    static GLint linkSuccess(GLuint program);

public:
    /// @brief Create an empty shader program that's only used as a temporary variable.
    ShaderProgram() = default;

    /// @brief Delete the program on GPU
    ~ShaderProgram();

    /// @brief Move only
    ShaderProgram(const ShaderProgram& other) = delete;

    /// @brief Move only
    ShaderProgram& operator=(const ShaderProgram& other) = delete;

    /// @brief Move constructor
    ShaderProgram(ShaderProgram&& other) noexcept;

    /// @brief Move assignment
    ShaderProgram& operator=(ShaderProgram&& other) noexcept;

    /// @brief Activate this program
    inline void activate() const { glUseProgram(m_id); }

    /// @brief Whether this program is available
    inline bool available() const { return m_id; }
};