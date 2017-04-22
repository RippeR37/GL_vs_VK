#pragma once

#include <base/Clock.h>

#include <glm/vec2.hpp>

#include <functional>
#include <string>
#include <vector>

struct GLFWwindow;

namespace base {
namespace gl {
class Window
{
  public:
    Window(const glm::uvec2& size, const std::string& title);
    virtual ~Window();

    bool create();
    virtual void update();
    void destroy();

    void setSize(const glm::uvec2& size);
    void setPosition(const glm::ivec2& position);
    void setTitle(const std::string& title);
    void appendTitle(const std::string& text);

    void setDisplayingFPS(bool flag);
    void setCountingFPS(bool flag);
    void setFPSRefreshRate(double refreshRate);

    void setFPSCountCallback(const std::function<void(int)>& function);
    void setDestroyCallback(const std::function<void()>& function);

    bool isDisplayingFPS() const;
    bool isCountingFPS() const;
    double getFPSRefreshRate() const;
    int getFPS() const;
    unsigned int getWidth() const;
    unsigned int getHeight() const;
    const glm::uvec2& getSize() const;
    const glm::ivec2& getPosition() const;
    const std::string& getTitle() const;

    bool isCreated() const;
    bool shouldClose() const;
    double getFrameTime() const;
    GLFWwindow* getHandle();

    static void setHint(int option, int value);
    static void setHints(const std::vector<std::pair<int, int>>& hints);
    static void setDefaultHints();

    static void deinitialize();

  protected:
    void setFPSCount(int fpsCount);

    int _fpsCount;
    unsigned int _framesCount;
    bool _isDisplayingFPS;
    bool _isCountingFPS;
    double _lastFpsMeasure;
    double _thisFpsMeasure;
    double _fpsTime;
    double _fpsRefreshRate;
    double _frameTime;
    GLFWwindow* _handle;
    std::string _title;
    glm::uvec2 _windowSize;
    glm::ivec2 _windowPosition;
    std::function<void(int)> _fpsCountCallback;
    std::function<void()> _destroyCallback;

    static bool _hintsSet;

  private:
    static void initializeGLFW();
    static void initializeGLEW();
    static void setFocus(GLFWwindow* window, int focused);
};
}
}
