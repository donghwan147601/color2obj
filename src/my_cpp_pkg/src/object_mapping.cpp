#include "my_cpp_pkg/object_mapping.hpp"
#include <yaml-cpp/yaml.h>
#include <cmath>
#include <limits>
#include <ament_index_cpp/get_package_share_directory.hpp>
#include <iostream>
#include <string>

void ObjectRegistry::registerObject(const ObjectSpec& obj) {
    db.push_back(obj);
}

void ObjectRegistry::loadFromYaml(const std::string& pkg_name, const std::string& file_path) {
    try {
        std::string share_dir = ament_index_cpp::get_package_share_directory(pkg_name);
        std::string full_path = share_dir + "/" + file_path;

        YAML::Node config = YAML::LoadFile(full_path);

        for (const auto& item : config["object_database"]) {
            ObjectSpec obj;
            obj.id = item["id"].as<std::string>();
            obj.h_mean = item["h"].as<int>();
            obj.s_mean = item["s"].as<int>();
            obj.v_mean = item["v"].as<int>();
            obj.r_mean = item["r"].as<int>();
            obj.g_mean = item["g"].as<int>();
            obj.b_mean = item["b"].as<int>();
            
            this->registerObject(obj);
        }
    } catch (const std::exception& e) {
        std::cerr << "YAML 로딩 실패: " << e.what() << std::endl;
    }
}

std::string ObjectRegistry::findBestMatch(int h, int s, int v, int r, int g, int b) {
    std::string best_id = "UNKNOWN";
    double min_dist = std::numeric_limits<double>::max();

    for (const auto& obj : db) {
        int dh = std::abs(h - obj.h_mean);
        if (dh > 90) {
            dh = 180 - dh;
        }

        double hsv_dist = std::pow(dh, 2) * 2.0 +
                          std::pow(s - obj.s_mean, 2) * 1.0 +  std::pow(v - obj.v_mean, 2) * 0.5;

   
        double dr = static_cast<double>(r - obj.r_mean);
        double dg = static_cast<double>(g - obj.g_mean);
        double dbv = static_cast<double>(b - obj.b_mean);
        double rgb_dist = std::pow(dr, 2) * 0.02 +
                          std::pow(dg, 2) * 0.02 +
                          std::pow(dbv, 2) * 0.02;

        double dist = hsv_dist + rgb_dist;

        if (dist < min_dist) {
            min_dist = dist;
            best_id = obj.id;
        }
    }

    // HSV+RGB 거리 합을 기준으로 임계값을 둡니다.
    // HSV 차이가 크거나 RGB 차이가 크면 UNKNOWN으로 처리합니다.
    if (min_dist > 5000.0) {
        return "UNKNOWN";
    }

    return best_id;
}

// std::string ObjectRegistry::findBestMatch(int h, int s, int v) {
//     std::string best_id = "UNKNOWN";
//     double min_dist = std::numeric_limits<double>::max();

//     // 1. 영상에서 얻은 H, S를 극좌표(x, y)로 변환
//     // H는 0~180 범위를 가정, 라디안으로 변환 (H * PI / 90.0)
//     double h_rad = h * M_PI / 90.0;
//     double s_val = static_cast<double>(s);
//     double x = s_val * std::cos(h_rad);
//     double y = s_val * std::sin(h_rad);

//     for (const auto& obj : db) {
//         // 2. 데이터베이스의 H, S를 동일하게 좌표 변환
//         double obj_h_rad = obj.h_mean * M_PI / 90.0;
//         double obj_x = static_cast<double>(obj.s_mean) * std::cos(obj_h_rad);
//         double obj_y = static_cast<double>(obj.s_mean) * std::sin(obj_h_rad);

//         // 3. HS 평면상의 유클리드 거리 (기하학적 거리)
//         double dist_hs = std::sqrt(std::pow(x - obj_x, 2) + std::pow(y - obj_y, 2));
        
//         // 4. V(명도) 값 차이 계산
//         double dist_v = std::abs(v - obj.v_mean);
        
//         // 5. 최종 거리 계산 (HS 거리 가중치 2.0, V 거리 가중치 0.3)
//         // HS 평면에서 물체간 거리가 물리적으로 멀면 인식 거부
//         double total_dist = dist_hs * 2.0 + dist_v * 0.3;

//         if (total_dist < min_dist) {
//             min_dist = total_dist;
//             best_id = obj.id;
//         }
//     }

//     // 6. 임계값(Threshold) 확인
//     // 극좌표 변환 후에는 거리 단위가 달라지므로 50.0 정도로 타이트하게 설정
//     if (min_dist > 100.0) {
//         return "UNKNOWN";
//     }

//     return best_id;
// }
