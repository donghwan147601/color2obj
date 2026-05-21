#include <cstdio>
#include <opencv2/opencv.hpp> // OpenCV 헤더 추가

int main(int argc, char ** argv)
{
  (void) argc;
  (void) argv;

  printf("hello world my_cpp_pkg package\n");

  cv::Mat test_image = cv::Mat::zeros(480, 640, CV_8UC3);
  printf("OpenCV Matrix created: %d x %d\n", test_image.cols, test_image.rows);

  return 0;
}
