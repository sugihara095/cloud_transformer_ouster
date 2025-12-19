from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    return LaunchDescription([
        Node(
            package='cloud_transformer_ouster',
            executable='cloud_transformer_ouster_node',
            name='cloud_transformer_ouster',
            output='screen',
            parameters=[{
                # 必要であればパラメータをここに書く
            }]
        )
    ])
