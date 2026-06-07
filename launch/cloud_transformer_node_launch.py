import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    # 1. パッケージのshareディレクトリのパスを取得
    # (例: install/share/<パッケージ名>)
    package_name = 'cloud_transformer_ouster'
    pkg_share = get_package_share_directory(package_name)

    # 2. YAMLファイルのパスを結合
    config_file_path = os.path.join(pkg_share, 'config', 'transformer.param.yaml')

    return LaunchDescription([
        Node(
            package='cloud_transformer_ouster', # パッケージ名
            executable='cloud_transformer_ouster_node', # 実行ファイル名
            name='cloud_transformer_ouster_node',    # ノード名 (YAMLの記述と合わせる)
            output='screen',
            # 3. ここでYAMLファイルを読み込ませる
            parameters=[config_file_path]
        )
    ])
    