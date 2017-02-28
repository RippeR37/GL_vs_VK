#include <base/gl/Window.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

namespace base {
namespace gl {

    bool Window::_hintsSet = false;

    Window::Window(const glm::uvec2& size, const std::string& title)
    {
        _handle = nullptr;
        _hintsSet = false;
        _framesCount = 0;

        setSize(size);
        setTitle(title);

        setCountingFPS(false);
        setDisplayingFPS(false);
        setDestroyCallback(std::bind(&Window::destroy, this));
    }

    Window::~Window()
    {
        destroy();
    }

    bool Window::create()
    {
        if (isCreated())
            destroy();

        initializeGLFW();

        if (_hintsSet == false)
            setDefaultHints();

        _handle = glfwCreateWindow(getWidth(), getHeight(), _title.c_str(), nullptr, nullptr);

        if (isCreated()) {
            glfwMakeContextCurrent(getHandle());
            initializeGLEW();

            _lastFrame = glfwGetTime();
            _fpsClock.reset();

        } else {
            std::cerr << "base::gl::Window > Couldn't create OpenGL window.";
        }

        return isCreated();
    }

    void Window::update()
    {
        if (glfwWindowShouldClose(getHandle())) {
            if (_destroyCallback)
                _destroyCallback();

            return;
        }

        _thisFrame = glfwGetTime();
        _frameTime = _thisFrame - _lastFrame;
        _lastFrame = _thisFrame;

        glfwSwapBuffers(_handle);
        glfwPollEvents();

        if (isCountingFPS()) {
            _framesCount += 1;
            _fpsTime = _fpsClock.getElapsedTime();

            if (_fpsTime > getFPSRefreshRate()) {
                setFPSCount(static_cast<unsigned int>(_framesCount * (1.0 / _fpsTime)));

                if (isDisplayingFPS())
                    appendTitle(std::string(" | FPS: ") + std::to_string(getFPS()));

                if (_fpsCountCallback)
                    _fpsCountCallback(getFPS());

                _framesCount = 0;
                _fpsClock.reset();
            }
        }
    }

    void Window::destroy()
    {
        if (isCreated()) {
            glfwDestroyWindow(_handle);
            _handle = nullptr;
        }
    }

    void Window::setSize(const glm::uvec2& size)
    {
        _windowSize = size;

        if (isCreated())
            glfwSetWindowSize(_handle, getWidth(), getHeight());
    }

    void Window::setPosition(const glm::ivec2& position)
    {
        _windowPosition = position;

        if (isCreated())
            glfwSetWindowPos(getHandle(), getPosition().x, getPosition().y);
    }

    void Window::setTitle(const std::string& title)
    {
        _title = title;

        if (isCreated())
            glfwSetWindowTitle(_handle, _title.c_str());
    }

    void Window::appendTitle(const std::string& text)
    {
        std::string appendedTitle = getTitle() + text;

        if (isCreated())
            glfwSetWindowTitle(_handle, appendedTitle.c_str());
    }

    void Window::setDisplayingFPS(bool flag)
    {
        _isDisplayingFPS = flag;

        if (isDisplayingFPS())
            setCountingFPS(true);
    }

    void Window::setCountingFPS(bool flag)
    {
        _isCountingFPS = flag;

        setFPSCount(-1);
    }

    void Window::setFPSRefreshRate(double refreshRate)
    {
        _fpsRefreshRate = refreshRate;
    }

    void Window::setFPSCountCallback(const std::function<void(int)>& function)
    {
        _fpsCountCallback = function;
    }

    void Window::setDestroyCallback(const std::function<void()>& function)
    {
        _destroyCallback = function;
    }

    bool Window::isDisplayingFPS() const
    {
        return _isDisplayingFPS;
    }

    bool Window::isCountingFPS() const
    {
        return _isCountingFPS;
    }

    double Window::getFPSRefreshRate() const
    {
        return _fpsRefreshRate;
    }

    int Window::getFPS() const
    {
        return _fpsCount;
    }

    unsigned int Window::getWidth() const
    {
        return _windowSize.x;
    }

    unsigned int Window::getHeight() const
    {
        return _windowSize.y;
    }

    const glm::uvec2& Window::getSize() const
    {
        return _windowSize;
    }

    const glm::ivec2& Window::getPosition() const
    {
        return _windowPosition;
    }

    const std::string& Window::getTitle() const
    {
        return _title;
    }

    bool Window::isCreated() const
    {
        return (_handle != nullptr);
    }

    bool Window::shouldClose() const
    {
        return glfwWindowShouldClose(_handle) == GL_TRUE;
    }

    double Window::getFrameTime() const
    {
        return _frameTime;
    }

    GLFWwindow* Window::getHandle()
    {
        return _handle;
    }

    void Window::deinitialize()
    {
        glfwTerminate();
    }

    void Window::initializeGLFW()
    {
        static bool initialized = false;

        if (initialized == false) {
            // Setting error callback
            static auto errorCallbackFunc = [](int error, const char* description) {
                std::cerr << "[GLFW] Error #" + std::to_string(error) + std::string(": ") + description << std::endl;
            };

            glfwSetErrorCallback(errorCallbackFunc);

            // Initializing library
            if (glfwInit() == false) {
                errorCallbackFunc(-1, "couldn't initialize library");
                throw std::runtime_error("Failed to initialize GLFW library.");
            }

            initialized = true;
        }
    }

    void Window::initializeGLEW()
    {
        static bool initialized = false;

        if (initialized == false) {
            glewExperimental = GL_TRUE;

            if (glewInit() != GLEW_OK) {
                std::cerr << "[GLEW] Error #-1: couldn't initialize library" << std::endl;
                throw std::runtime_error("Failed to initialize GLEW library.");
            }

            initialized = true;
        }
    }

    void Window::setHint(int option, int value)
    {
        glfwWindowHint(option, value);

        _hintsSet = true;
    }

    void Window::setHints(const std::vector<std::pair<int, int>>& hints)
    {
        for (auto& hint : hints)
            glfwWindowHint(hint.first, hint.second);

        _hintsSet = true;
    }

    void Window::setDefaultHints()
    {
        setHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        setHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        setHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        setHint(GLFW_RESIZABLE, GL_FALSE);

        _hintsSet = true;
    }

    void Window::setFPSCount(int fpsCount)
    {
        _fpsCount = fpsCount;
    }
}
}
