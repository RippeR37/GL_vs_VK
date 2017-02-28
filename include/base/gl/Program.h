#pragma once

#include <base/gl/Shader.h>
#include <base/gl/Uniform.h>

#include <GL/glew.h>

#include <string>
#include <unordered_map>
#include <vector>

namespace base {
namespace gl {
    class Program {
    public:
        Program();
        Program(Program&& program);
        Program(const std::string& vsPath, const std::string& fsPath);
        Program(const Shader& vertexShader, const Shader& fragmentShader);
        Program(const Program&) = delete;
        ~Program();

        Program& operator=(const Program&) = delete;
        Program& operator=(Program&& program);

        Uniform& operator[](const std::string& uniformName);
        const Uniform& operator[](const std::string& uniformName) const;

        void create();
        void destroy();

        void load(const Shader& vertexShader, const Shader& fragmentShader);
        void use() const;
        void unbind() const;

        bool isLinked() const;
        GLuint getID() const;

        GLuint getUniformLocation(const std::string& uniformName) const;
        const Uniform& getUniform(const std::string& uniformName) const;

    private:
        void mapUniforms(const Shader& shader);
        bool link(const Shader& vertexShader, const Shader& fragmentShader);

        bool isCreated() const;

        bool _isLinked;
        bool _isCreated;
        GLuint _programID;
        std::unordered_map<std::string, Uniform> _uniforms;
    };
}
}
