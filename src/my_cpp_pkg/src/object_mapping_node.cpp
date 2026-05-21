#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/int32_multi_array.hpp>
#include <std_msgs/msg/string.hpp>
#include "my_cpp_pkg/object_mapping.hpp"

class ObjectMappingNode : public rclcpp::Node {
public:
    ObjectMappingNode()
    : Node("object_mapping_node") {
        registry_.loadFromYaml("my_cpp_pkg", "resource/object_table.yaml");
        object_label_publisher_ = this->create_publisher<std_msgs::msg::String>("/camera/object_label", 10);

        subscription_ = this->create_subscription<std_msgs::msg::Int32MultiArray>(
            "/camera/object_hsv", 10,
            std::bind(&ObjectMappingNode::hsv_callback, this, std::placeholders::_1));

        RCLCPP_INFO(this->get_logger(), "object_mapping_node started, listening on /camera/object_hsv");
    }

private:
    void hsv_callback(const std_msgs::msg::Int32MultiArray::SharedPtr msg) {
        if (msg->data.size() < 6) {
            RCLCPP_WARN(this->get_logger(), "HSV+RGB message too short, expected 6 values");
            return;
        }

        int h = msg->data[0];
        int s = msg->data[1];
        int v = msg->data[2];
        int r = msg->data[3];
        int g = msg->data[4];
        int b = msg->data[5];
        std::string match = registry_.findBestMatch(h, s, v, r, g, b);

        RCLCPP_INFO(this->get_logger(), "HSV (%d, %d, %d) RGB (%d, %d, %d) -> object: %s",
                    h, s, v, r, g, b, match.c_str());

        std_msgs::msg::String label_msg;
        label_msg.data = match;
        object_label_publisher_->publish(label_msg);
    }

    ObjectRegistry registry_;
    rclcpp::Subscription<std_msgs::msg::Int32MultiArray>::SharedPtr subscription_;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr object_label_publisher_;
};

int main(int argc, char * argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<ObjectMappingNode>());
    rclcpp::shutdown();
    return 0;
}
