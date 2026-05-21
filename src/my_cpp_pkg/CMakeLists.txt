cmake_minimum_required(VERSION 3.8)
project(my_cpp_pkg)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

install(DIRECTORY
  resource
  DESTINATION share/${PROJECT_NAME}
)
# 런치 파일 설치
install(DIRECTORY
  launch
  DESTINATION share/${PROJECT_NAME}
)
# find dependencies
find_package(ament_cmake REQUIRED)
# uncomment the following section in order to fill in
# further dependencies manually.
# find_package(<dependency> REQUIRED)
find_package(OpenCV REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(cv_bridge REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(yaml-cpp REQUIRED)
add_executable(my_first_node src/my_first_node.cpp )
add_executable(image_processing src/image_processing.cpp )
add_executable(object_mapping_node src/object_mapping_node.cpp src/object_mapping.cpp )

target_include_directories(my_first_node PUBLIC ${OpenCV_INCLUDE_DIRS})
target_link_libraries(my_first_node ${OpenCV_LIBRARIES})
target_include_directories(my_first_node PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
target_compile_features(my_first_node PUBLIC c_std_99 cxx_std_17)  # Require C99 and C++17

# image_processing target: include directories, compile features, and ament deps
target_include_directories(image_processing PUBLIC ${OpenCV_INCLUDE_DIRS}
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
target_compile_features(image_processing PUBLIC cxx_std_17)
ament_target_dependencies(image_processing rclcpp sensor_msgs cv_bridge OpenCV)

# object_mapping_node target: YAML loading + rclcpp
target_include_directories(object_mapping_node PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
target_compile_features(object_mapping_node PUBLIC cxx_std_17)
ament_target_dependencies(object_mapping_node rclcpp std_msgs)
target_link_libraries(object_mapping_node yaml-cpp)

install(TARGETS my_first_node image_processing object_mapping_node
  DESTINATION lib/${PROJECT_NAME})

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
