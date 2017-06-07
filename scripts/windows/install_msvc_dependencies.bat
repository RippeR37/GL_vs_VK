@cd ../../

@git submodule update --recursive --init

@echo "Installing GLFW library..."
@cd third_party/glfw/
@cmake .
@MSBuild ALL_BUILD.vcxproj
@MSBuild ALL_BUILD.vcxproj /property:Configuration=Release
@cd ../../
@echo "Done!"

@echo.
@echo "Installing GLEW library..."
@cd third_party/glew-win/
@expand -F:* glew-win-src.cab .
@cd ../../
@echo "Done!"

@echo.
@echo "Installing GLM library..."
@echo "Done!"

@echo "Done!"
