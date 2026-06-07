#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/point_cloud2.hpp>
#include <pcl_conversions/pcl_conversions.h>
#include <pcl/point_cloud.h>
#include <pcl/common/transforms.h>
#include <Eigen/Dense>

// --- Ouster OS1 用 PointType を定義 ---
struct OusterPoint
{
  PCL_ADD_POINT4D;   // x, y, z, padding
  float intensity;   // intensity
  uint32_t t;  // t（uint32 -> double）
  uint16_t reflectivity;
  uint16_t ring;
  uint16_t ambient;
  uint32_t range;
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
} EIGEN_ALIGN16;

POINT_CLOUD_REGISTER_POINT_STRUCT(
  // 順番大事
  OusterPoint,
  (float, x, x)
  (float, y, y)
  (float, z, z)
  (float, intensity, intensity)
  (uint32_t, t, t)
  (uint16_t, reflectivity, reflectivity)
  (uint16_t, ring, ring)
  (uint16_t, ambient, ambient)
  (uint32_t, range, range)
)

class CloudTransformer : public rclcpp::Node
{
public:
  CloudTransformer()
  : Node("cloud_transformer_ouster")
  {
    // これをしないとYAMLから値を読み込めない
    this->declare_parameter<double>("sensor_height", 1.75);

    sub_ = this->create_subscription<sensor_msgs::msg::PointCloud2>(
      "/ouster/points",
      rclcpp::SensorDataQoS(),
      std::bind(&CloudTransformer::callback, this, std::placeholders::_1)
    );

    pub_ = this->create_publisher<sensor_msgs::msg::PointCloud2>(
      "/ouster/points_base",
      rclcpp::SensorDataQoS()
    );
  }

private:
  void callback(const sensor_msgs::msg::PointCloud2::SharedPtr msg)
  {
    pcl::PointCloud<OusterPoint>::Ptr cloud(new pcl::PointCloud<OusterPoint>);
    pcl::fromROSMsg(*msg, *cloud);

    // callback内で毎回取得することで、実行中に `ros2 param set` で変更した際も即座に反映
    double sensor_height = 1.75; // 取得失敗時の安全策としての初期値
    this->get_parameter("sensor_height", sensor_height);

    // ---- 座標変換行列 ----
    Eigen::Affine3f transform = Eigen::Affine3f::Identity();
    // 取得した sensor_height を適用
    transform.translation() << 0.0, 0.0, sensor_height;

    pcl::PointCloud<OusterPoint>::Ptr transformed_cloud(new pcl::PointCloud<OusterPoint>);
    pcl::transformPointCloud(*cloud, *transformed_cloud, transform);

    sensor_msgs::msg::PointCloud2 output_msg;
    pcl::toROSMsg(*transformed_cloud, output_msg);
    output_msg.header = msg->header;
    output_msg.header.frame_id = "os_base";

    pub_->publish(output_msg);
  }

  rclcpp::Subscription<sensor_msgs::msg::PointCloud2>::SharedPtr sub_;
  rclcpp::Publisher<sensor_msgs::msg::PointCloud2>::SharedPtr pub_;
};

int main(int argc, char **argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<CloudTransformer>());
  rclcpp::shutdown();
  return 0;
}
