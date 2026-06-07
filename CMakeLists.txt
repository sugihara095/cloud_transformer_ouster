cmake_minimum_required(VERSION 3.8)
project(cloud_transformer_ouster)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(PCL REQUIRED)
find_package(pcl_conversions REQUIRED)

include_directories(
  include
  ${PCL_INCLUDE_DIRS}
)

add_executable(cloud_transformer_ouster_node
  src/cloud_transformer_ouster_node.cpp
)

# 依存関係を指定
ament_target_dependencies(cloud_transformer_ouster_node rclcpp sensor_msgs pcl_conversions)
target_link_libraries(cloud_transformer_ouster_node
  ${PCL_LIBRARIES}
)

# 1. 実行ファイルのインストール
install(TARGETS cloud_transformer_ouster_node
  DESTINATION lib/${PROJECT_NAME}
)

# 2. フォルダ（launch, config）のインストール
install(DIRECTORY launch config
  DESTINATION share/${PROJECT_NAME}
)

ament_package()