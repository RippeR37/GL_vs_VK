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
@7z x glew-win-src.zip
@cd ../../
@echo "Done!"

@echo.
@echo "Installing GLM library..."
@echo "Done!"

@echo "Done!"
