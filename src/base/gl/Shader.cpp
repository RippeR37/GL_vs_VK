#include <base/File.h>
#include <base/gl/Shader.h>

#include <stdexcept>
#include <vector>

namespace base {
namespace gl {
Shader::Shader(Type type)
{
    _isCreated = false;
    _isCompiled = false;
    _type = type;
    _shaderID = 0;
}

Shader::Shader(const std::string& path, Type type)
    : Shader(type)
{
    load(path);
}

Shader::Shader(Shader&& shader)
    : Shader(shader._type)
{
    std::swap(_type, shader._type);
    std::swap(_shaderID, shader._shaderID);
    std::swap(_isCreated, shader._isCreated);
    std::swap(_isCompiled, shader._isCompiled);

    _path.swap(shader._path);
    _code.swap(shader._code);
}

Shader::~Shader()
{
    destroy();
}

Shader& Shader::operator=(Shader&& shader)
{
    _isCreated = false;

    std::swap(_type, shader._type);
    std::swap(_shaderID, shader._shaderID);
    std::swap(_isCreated, shader._isCreated);
    std::swap(_isCompiled, shader._isCompiled);

    _path.swap(shader._path);
    _code.swap(shader._code);

    return *this;
}

void Shader::create()
{
    _shaderID = glCreateShader(static_cast<GLenum>(getType()));
    _isCreated = true;
    _isCompiled = false;
}

void Shader::destroy()
{
    if (isCreated()) {
        glDeleteShader(_shaderID);

        _isCreated = false;
        _isCompiled = false;
    }
}

void Shader::load(const std::string& path)
{
    if (!isCreated())
        create();

    if (!isCompiled()) {
        _path = path;
        _code = File::readText(_path, true);
        _isCompiled = compile();
    }
}

bool Shader::isCompiled() const
{
    return _isCompiled;
}

GLuint Shader::getID() const
{
    if (!isCreated())
        const_cast<Shader*>(this)->create();

    return _shaderID;
}

Shader::Type Shader::getType() const
{
    return _type;
}

bool Shader::compile()
{
    std::vector<char> shaderErrorMsg;
    GLint result;
    GLint infoLen;
    GLchar const* codePtr;

    // Create and compile shader
    codePtr = _code.c_str();
    glShaderSource(_shaderID, 1, &codePtr, nullptr);
    glCompileShader(_shaderID);

    // Check for errors
    glGetShaderiv(_shaderID, GL_COMPILE_STATUS, &result);
    glGetShaderiv(_shaderID, GL_INFO_LOG_LENGTH, &infoLen);
    shaderErrorMsg.resize(infoLen);
    glGetShaderInfoLog(_shaderID, infoLen, nullptr, shaderErrorMsg.data());

    if (result != GL_TRUE) {
        std::string errorMsg =
            std::string("Shader compilation failed:\nFile: " + _path + "\nMessage:\n" + shaderErrorMsg.data());
        throw std::runtime_error(errorMsg);
    }

    return true;
}

bool Shader::isCreated() const
{
    return _isCreated;
}
}
}
