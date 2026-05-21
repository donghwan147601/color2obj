#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/image.hpp>
#include <std_msgs/msg/int32_multi_array.hpp>
#include <cv_bridge/cv_bridge.h>
#include <opencv2/opencv.hpp>

class ImageProcessorNode : public rclcpp::Node {
public:
    ImageProcessorNode() : Node("image_processor") {
        // 파라미터 선언
        this->declare_parameter<bool>("show_gui", true);
        this->get_parameter("show_gui", show_gui_);

        // 구독 및 퍼블리셔 설정
        subscription_ = this->create_subscription<sensor_msgs::msg::Image>(
            "/camera/rgb_masked", 10,
            std::bind(&ImageProcessorNode::image_callback, this, std::placeholders::_1));
        
        publisher_ = this->create_publisher<sensor_msgs::msg::Image>("/camera/processed_image", 10);
        hsv_publisher_ = this->create_publisher<std_msgs::msg::Int32MultiArray>("/camera/object_hsv", 10);

        if (show_gui_) {
            cv::namedWindow("Processed Image", cv::WINDOW_AUTOSIZE);
        }
    }

private:
    void image_callback(const sensor_msgs::msg::Image::SharedPtr msg) {
        // 1. ROS Image -> OpenCV Mat 변환
        cv_bridge::CvImagePtr cv_ptr;
        try {
            cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
        } catch (cv_bridge::Exception& e) {
            RCLCPP_ERROR(this->get_logger(), "cv_bridge exception: %s", e.what());
            return;
        }

        cv::Mat img = cv_ptr->image;
        if (img.empty()) return;

        // --- 5단계 이미지 전처리 파이프라인 ---

        // [1단계] Resize (표준 해상도 확보)
        cv::resize(img, img, cv::Size(640, 480));

        // [2단계] Noise Reduction (가우시안 필터링)
        cv::GaussianBlur(img, img, cv::Size(5, 5), 0);

        // [3, 4단계] Contrast Enhancement & Illumination Normalization (LAB 색공간 CLAHE)
        cv::Mat lab_img;
        cv::cvtColor(img, lab_img, cv::COLOR_BGR2Lab);
        std::vector<cv::Mat> lab_channels;
        cv::split(lab_img, lab_channels);

        // L채널에 CLAHE 적용 (조명 변화 보상)
        cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE(2.0, cv::Size(8, 8));
        clahe->apply(lab_channels[0], lab_channels[0]);
        
        cv::merge(lab_channels, lab_img);
        cv::cvtColor(lab_img, img, cv::COLOR_Lab2BGR);

        // [5단계] Segmentation (마스킹 기반 평균값 추출)
        cv::Mat mask;
        cv::cvtColor(img, mask, cv::COLOR_BGR2GRAY);
        cv::threshold(mask, mask, 10, 255, cv::THRESH_BINARY);

        cv::Mat hsv_img;
        cv::cvtColor(img, hsv_img, cv::COLOR_BGR2HSV);

        cv::Scalar hsv_mean = cv::mean(hsv_img, mask);
        cv::Scalar rgb_mean = cv::mean(img, mask);

        // 결과 퍼블리시 (HSV + RGB)
        std_msgs::msg::Int32MultiArray hsv_msg;
        hsv_msg.data = {
            static_cast<int>(hsv_mean[0]), static_cast<int>(hsv_mean[1]), static_cast<int>(hsv_mean[2]),
            static_cast<int>(rgb_mean[2]), static_cast<int>(rgb_mean[1]), static_cast<int>(rgb_mean[0])
        };
        hsv_publisher_->publish(hsv_msg);

        // 로깅
        RCLCPP_INFO_THROTTLE(this->get_logger(), *this->get_clock(), 1000, 
            "Processed -> H:%.0f, S:%.0f, V:%.0f, R:%.0f, G:%.0f, B:%.0f", 
            hsv_mean[0], hsv_mean[1], hsv_mean[2], rgb_mean[2], rgb_mean[1], rgb_mean[0]);

        // GUI 출력 및 퍼블리시
        if (show_gui_) {
            cv::imshow("Processed Image", img);
            cv::waitKey(1);
        }

        cv_bridge::CvImage out_msg;
        out_msg.header = msg->header;
        out_msg.encoding = sensor_msgs::image_encodings::BGR8;
        out_msg.image = img;
        publisher_->publish(*out_msg.toImageMsg());
    }

    bool show_gui_{true};
    rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr subscription_;
    rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr publisher_;
    rclcpp::Publisher<std_msgs::msg::Int32MultiArray>::SharedPtr hsv_publisher_;
};

int main(int argc, char * argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<ImageProcessorNode>());
    rclcpp::shutdown();
    return 0;
}