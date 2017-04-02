#include <base/gl/Uniform.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#define GLM_FORCE_RADIANS
#include <glm/gtc/type_ptr.hpp>

namespace base {
namespace gl {
Uniform::Uniform()
{
    _location = -1;
}

Uniform::Uniform(GLuint location)
{
    _location = location;
}

Uniform::~Uniform()
{
}

GLuint Uniform::getLocation() const
{
    return _location;
}

void Uniform::setSampler(GLint value) const
{
    glUniform1i(_location, value);
}

// glUniform1_()
template <>
void Uniform::operator=(const GLfloat& value) const
{
    glUniform1f(_location, value);
}
template <>
void Uniform::operator=(const GLint& value) const
{
    glUniform1i(_location, value);
}
template <>
void Uniform::operator=(const GLuint& value) const
{
    glUniform1ui(_location, value);
}

// glUniform2_v()
template <>
void Uniform::operator=(const glm::vec2& value) const
{
    glUniform2fv(_location, 1, glm::value_ptr(value));
}
template <>
void Uniform::operator=(const glm::ivec2& value) const
{
    glUniform2iv(_location, 1, glm::value_ptr(value));
}
template <>
void Uniform::operator=(const glm::uvec2& value) const
{
    glUniform2uiv(_location, 1, glm::value_ptr(value));
}

// glUniform3_v()
template <>
void Uniform::operator=(const glm::vec3& value) const
{
    glUniform3fv(_location, 1, glm::value_ptr(value));
}
template <>
void Uniform::operator=(const glm::ivec3& value) const
{
    glUniform3iv(_location, 1, glm::value_ptr(value));
}
template <>
void Uniform::operator=(const glm::uvec3& value) const
{
    glUniform3uiv(_location, 1, glm::value_ptr(value));
}

// glUniform4_v()
template <>
void Uniform::operator=(const glm::vec4& value) const
{
    glUniform4fv(_location, 1, glm::value_ptr(value));
}
template <>
void Uniform::operator=(const glm::ivec4& value) const
{
    glUniform4iv(_location, 1, glm::value_ptr(value));
}
template <>
void Uniform::operator=(const glm::uvec4& value) const
{
    glUniform4uiv(_location, 1, glm::value_ptr(value));
}

// glUniformMatrix_fv()
template <>
void Uniform::operator=(const glm::mat2& value) const
{
    glUniformMatrix2fv(_location, 1, GL_FALSE, glm::value_ptr(value));
}
template <>
void Uniform::operator=(const glm::mat3& value) const
{
    glUniformMatrix3fv(_location, 1, GL_FALSE, glm::value_ptr(value));
}
template <>
void Uniform::operator=(const glm::mat4& value) const
{
    glUniformMatrix4fv(_location, 1, GL_FALSE, glm::value_ptr(value));
}

// glUniformMatrix_x_fv()
template <>
void Uniform::operator=(const glm::mat2x3& value) const
{
    glUniformMatrix2x3fv(_location, 1, GL_FALSE, glm::value_ptr(value));
}
template <>
void Uniform::operator=(const glm::mat2x4& value) const
{
    glUniformMatrix2x4fv(_location, 1, GL_FALSE, glm::value_ptr(value));
}
template <>
void Uniform::operator=(const glm::mat3x4& value) const
{
    glUniformMatrix3x4fv(_location, 1, GL_FALSE, glm::value_ptr(value));
}
template <>
void Uniform::operator=(const glm::mat4x2& value) const
{
    glUniformMatrix4x2fv(_location, 1, GL_FALSE, glm::value_ptr(value));
}
template <>
void Uniform::operator=(const glm::mat4x3& value) const
{
    glUniformMatrix4x3fv(_location, 1, GL_FALSE, glm::value_ptr(value));
}

// glUniform1_()
template <>
void Uniform::set(GLfloat v0) const
{
    glUniform1f(_location, v0);
}
template <>
void Uniform::set(GLint v0) const
{
    glUniform1i(_location, v0);
}
template <>
void Uniform::set(GLuint v0) const
{
    glUniform1ui(_location, v0);
}

// glUniform2_()
template <>
void Uniform::set(GLfloat v0, GLfloat v1) const
{
    glUniform2f(_location, v0, v1);
}
template <>
void Uniform::set(GLint v0, GLint v1) const
{
    glUniform2i(_location, v0, v1);
}
template <>
void Uniform::set(GLuint v0, GLuint v1) const
{
    glUniform2ui(_location, v0, v1);
}

// glUniform3_()
template <>
void Uniform::set(GLfloat v0, GLfloat v1, GLfloat v2) const
{
    glUniform3f(_location, v0, v1, v2);
}
template <>
void Uniform::set(GLint v0, GLint v1, GLint v2) const
{
    glUniform3i(_location, v0, v1, v2);
}
template <>
void Uniform::set(GLuint v0, GLuint v1, GLuint v2) const
{
    glUniform3ui(_location, v0, v1, v2);
}

// glUniform4_()
template <>
void Uniform::set(GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3) const
{
    glUniform4f(_location, v0, v1, v2, v3);
}
template <>
void Uniform::set(GLint v0, GLint v1, GLint v2, GLint v3) const
{
    glUniform4i(_location, v0, v1, v2, v3);
}
template <>
void Uniform::set(GLuint v0, GLuint v1, GLuint v2, GLuint v3) const
{
    glUniform4ui(_location, v0, v1, v2, v3);
}

// glUniform2_v()
template <>
void Uniform::setVec(const glm::vec2& value, GLsizei count) const
{
    glUniform2fv(_location, count, glm::value_ptr(value));
}
template <>
void Uniform::setVec(const glm::ivec2& value, GLsizei count) const
{
    glUniform2iv(_location, count, glm::value_ptr(value));
}
template <>
void Uniform::setVec(const glm::uvec2& value, GLsizei count) const
{
    glUniform2uiv(_location, count, glm::value_ptr(value));
}

// glUniform3_v()
template <>
void Uniform::setVec(const glm::vec3& value, GLsizei count) const
{
    glUniform3fv(_location, count, glm::value_ptr(value));
}
template <>
void Uniform::setVec(const glm::ivec3& value, GLsizei count) const
{
    glUniform3iv(_location, count, glm::value_ptr(value));
}
template <>
void Uniform::setVec(const glm::uvec3& value, GLsizei count) const
{
    glUniform3uiv(_location, count, glm::value_ptr(value));
}

// glUniform4_v()
template <>
void Uniform::setVec(const glm::vec4& value, GLsizei count) const
{
    glUniform4fv(_location, count, glm::value_ptr(value));
}
template <>
void Uniform::setVec(const glm::ivec4& value, GLsizei count) const
{
    glUniform4iv(_location, count, glm::value_ptr(value));
}
template <>
void Uniform::setVec(const glm::uvec4& value, GLsizei count) const
{
    glUniform4uiv(_location, count, glm::value_ptr(value));
}

// glUniformMatrix_fv()
template <>
void Uniform::setMatrix(const glm::mat2& value, GLsizei count, GLboolean transpose) const
{
    glUniformMatrix2fv(_location, count, transpose, glm::value_ptr(value));
}
template <>
void Uniform::setMatrix(const glm::mat3& value, GLsizei count, GLboolean transpose) const
{
    glUniformMatrix3fv(_location, count, transpose, glm::value_ptr(value));
}
template <>
void Uniform::setMatrix(const glm::mat4& value, GLsizei count, GLboolean transpose) const
{
    glUniformMatrix4fv(_location, count, transpose, glm::value_ptr(value));
}

// glUniformMatrix_x_fv()
template <>
void Uniform::setMatrix(const glm::mat2x3& value, GLsizei count, GLboolean transpose) const
{
    glUniformMatrix2x3fv(_location, count, transpose, glm::value_ptr(value));
}
template <>
void Uniform::setMatrix(const glm::mat2x4& value, GLsizei count, GLboolean transpose) const
{
    glUniformMatrix2x4fv(_location, count, transpose, glm::value_ptr(value));
}
template <>
void Uniform::setMatrix(const glm::mat3x4& value, GLsizei count, GLboolean transpose) const
{
    glUniformMatrix3x4fv(_location, count, transpose, glm::value_ptr(value));
}
template <>
void Uniform::setMatrix(const glm::mat4x2& value, GLsizei count, GLboolean transpose) const
{
    glUniformMatrix4x2fv(_location, count, transpose, glm::value_ptr(value));
}
template <>
void Uniform::setMatrix(const glm::mat4x3& value, GLsizei count, GLboolean transpose) const
{
    glUniformMatrix4x3fv(_location, count, transpose, glm::value_ptr(value));
}
}
}
