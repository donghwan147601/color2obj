# 📦 my_cpp_pkg

`my_cpp_pkg` is a modular ROS 2 package designed for color-based object identification and ID assignment. It utilizes a two-node architecture to process visual data and map extracted color features to specific object labels.

---

## 🚀 Nodes Overview

### 1. `image_processing`
Processes incoming raw images and extracts representative color features.

*   **Input:** `/camera/rgb_masked` (`sensor_msgs/msg/Image`)
*   **Output:** 
    *   `/dcamera/processed_image` (`sensor_msgs/msg/Image`)
    *   `/camera/object_hsv` (`std_msgs/msg/Int32MultiArray`)
*   **Key Features:**
    *   **Resizing:** 640x480 resolution.
    *   **Noise Reduction:** Applied Gaussian blur.
    *   **Lighting Normalization:** CLAHE applied in the LAB color space.
    *   **Feature Extraction:** Computes average HSV/RGB values via grayscale mask.
    *   **Visualization:** Optional real-time preview via OpenCV GUI.

### 2. `object_mapping_node`
Maps the extracted color features to predefined object labels.

*   **Input:** `/camera/object_hsv` (`std_msgs/msg/Int32MultiArray`)
*   **Output:** `/camera/object_label` (`std_msgs/msg/String`)
*   **Key Features:**
    *   **Registry:** Loads color definitions from `resource/object_table.yaml`.
    *   **Matching:** Identifies the closest label based on HSV/RGB similarity.

---

## 🛠 Getting Started

### Build
```bash
colcon build --packages-select my_cpp_pkg
