#!/bin/bash


# This script should be run from project's base directory,
# i.e. ./scripts/linux/install_dependencies.sh
SCRIPT_DIR="`dirname $0`"


# Helper function
function install_dependency {
    echo "Installing $1..." && chmod +x $SCRIPT_DIR/dependencies/$1.sh && $SCRIPT_DIR/dependencies/$1.sh && echo "Done!" || echo "Error!"
}


# Main script

echo "###############################"
echo "### Installing dependencies ###"
echo "###############################"
echo ""

install_dependency "GLFW"
install_dependency "GLEW"
install_dependency "GLM"

