#include <base/gl/Program.h>

#include <iostream>
#include <sstream>
#include <stdexcept>

namespace base {
namespace gl {

    Program::Program()
    {
        _isCreated = false;
        _isLinked = false;
        _programID = 0;
    }

    Program::Program(Program&& program)
        : Program()
    {
        std::swap(_isLinked, program._isLinked);
        std::swap(_isCreated, program._isCreated);
        std::swap(_programID, program._programID);

        _uniforms.swap(program._uniforms);
    }

    Program::Program(const std::string& vsPath, const std::string& fsPath)
    {
        Shader vertexShader(vsPath, Shader::Type::VertexShader);
        Shader fragmentShader(fsPath, Shader::Type::FragmentShader);

        load(vertexShader, fragmentShader);
    }

    Program::Program(const Shader& vertexShader, const Shader& fragmentShader)
    {
        load(vertexShader, fragmentShader);
    }

    Program::~Program()
    {
        destroy();
    }

    Program& Program::operator=(Program&& program)
    {
        _isCreated = false;

        std::swap(_isLinked, program._isLinked);
        std::swap(_isCreated, program._isCreated);
        std::swap(_programID, program._programID);
        _uniforms.swap(program._uniforms);

        return *this;
    }

    Uniform& Program::operator[](const std::string& uniformName)
    {
        return _uniforms.at(uniformName);
    }

    const Uniform& Program::operator[](const std::string& uniformName) const
    {
        return _uniforms.at(uniformName);
    }

    void Program::create()
    {
        destroy();

        _programID = glCreateProgram();
        _isCreated = true;
        _isLinked = false;
    }

    void Program::destroy()
    {
        if (isCreated()) {
            glDeleteProgram(_programID);

            _isCreated = false;
            _isLinked = false;
        }
    }

    void Program::load(const Shader& vertexShader, const Shader& fragmentShader)
    {
        if (!isCreated())
            create();

        if (!isLinked()) {
            _isLinked = link(vertexShader, fragmentShader);
            mapUniforms(vertexShader);
            mapUniforms(fragmentShader);
        }
    }

    void Program::use() const
    {
        if (isLinked()) {
            glUseProgram(getID());
        } else {
            std::cerr << "base::gl::Program > Attempt to use unlinked program!";
        }
    }

    void Program::unbind() const
    {
        glUseProgram(0);
    }

    bool Program::isLinked() const
    {
        return _isLinked;
    }

    GLuint Program::getID() const
    {
        if (!isCreated())
            const_cast<Program*>(this)->create();

        return _programID;
    }

    GLuint Program::getUniformLocation(const std::string& uniformName) const
    {
        return _uniforms.at(uniformName).getLocation();
    }

    const Uniform& Program::getUniform(const std::string& uniformName) const
    {
        return _uniforms.at(uniformName);
    }

    bool Program::link(const Shader& vertexShader, const Shader& fragmentShader)
    {
        std::vector<char> programErrorMsg;
        GLint result;
        GLint infoLen;

        // Create and link program
        glAttachShader(getID(), vertexShader.getID());
        glAttachShader(getID(), fragmentShader.getID());
        glLinkProgram(getID());

        // Check for errors
        glGetProgramiv(getID(), GL_LINK_STATUS, &result);
        glGetProgramiv(getID(), GL_INFO_LOG_LENGTH, &infoLen);
        programErrorMsg.resize(infoLen);
        glGetProgramInfoLog(getID(), infoLen, nullptr, programErrorMsg.data());

        if (result != GL_TRUE) {
            std::string errorMsg = std::string("Program linking failed:") + std::string(programErrorMsg.data());
            throw std::runtime_error(errorMsg);
        }

        return true;
    }

    void Program::mapUniforms(const Shader& shader)
    {
        std::stringstream stream;
        std::string uniformName;
        std::string uniformType;

        stream.str(shader._code);
        while (stream.good()) {
            stream >> uniformName;
            if (uniformName == "uniform") {
                stream >> uniformType;
                stream >> uniformName;

                // Semicolon
                if (uniformName[uniformName.size() - 1] == ';')
                    uniformName.erase(uniformName.size() - 1);

                // Arrays
                uniformName = uniformName.substr(0, uniformName.find_last_of("["));

                _uniforms[uniformName] = Uniform(glGetUniformLocation(getID(), uniformName.c_str()));
            }
        }
    }

    bool Program::isCreated() const
    {
        return _isCreated;
    }
}
}
