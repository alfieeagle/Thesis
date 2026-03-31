import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch_ros.actions import Node
from launch.substitutions import LaunchConfiguration

def generate_launch_description():
    pkg_project = get_package_share_directory('ros_wrapper')
    bridge_config_path = os.path.join(pkg_project, 'config', 'bridge_config.yaml')
    use_sim_time = LaunchConfiguration('use_sim_time')

    return LaunchDescription([
        Node(
            package='ros_gz_bridge',
            executable='parameter_bridge',
            parameters=[{
                'config_file': bridge_config_path,
                'use_sim_time': use_sim_time
            }],
            output='screen',
        ),
        
        Node(
            package='ros_wrapper',
            executable='buoyancy_node',
            name='buoyancy_node',
            output='screen',
            parameters=[{'use_sim_time': use_sim_time}]
        )
    ])