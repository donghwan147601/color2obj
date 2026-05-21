#!/usr/bin/env bash
# Clean environment and run the ROS2 node without Snap/VSCodium library contamination.

unset LD_PRELOAD
unset GIO_MODULE_DIR
unset GIO_EXTRA_MODULES
unset XDG_CONFIG_DIRS
unset XDG_DATA_DIRS
unset GSETTINGS_SCHEMA_DIR_VSCODE_SNAP_ORIG
unset XDG_CONFIG_DIRS_VSCODE_SNAP_ORIG
unset GTK_PATH_VSCODE_SNAP_ORIG
unset GTK_PATH

# Source ROS2 and workspace environment cleanly.
source /opt/ros/humble/setup.bash
source "$(pwd)/install/setup.bash"

topic=${1:-/camera/rgb_masked}
echo "Running on image topic: ${topic}"
ros2 run my_cpp_pkg image_processing --ros-args -p image_topic:=${topic}
