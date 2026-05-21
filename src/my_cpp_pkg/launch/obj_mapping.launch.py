from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import ExecuteProcess
import os

def generate_launch_description():
    # 환경 변수 정리: Snap 경로 제거
    shell_env = 'export LD_LIBRARY_PATH=$(echo $LD_LIBRARY_PATH | tr ":" "\\n" | grep -v snap | tr "\\n" ":"); ' \
                'export PATH=$(echo $PATH | tr ":" "\\n" | grep -v snap | tr "\\n" ":"); '

    return LaunchDescription([
        ExecuteProcess(
            cmd=['bash', '-c', f'{shell_env} ros2 run my_cpp_pkg image_processing --ros-args -p show_gui:=true'],
            output='screen',
            name='image_processing'
        ),

        Node(
            package='my_cpp_pkg',
            executable='object_mapping_node',
            name='object_mapping_node',
            output='screen'
        )
    ])