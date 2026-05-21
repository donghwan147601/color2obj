#ifndef OBJECT_REGISTRY_HPP
#define OBJECT_REGISTRY_HPP

#include <vector>
#include <string>

struct ObjectSpec {
    std::string id;
    int h_mean, s_mean, v_mean;
    int r_mean, g_mean, b_mean;
};

class ObjectRegistry {
private:
    std::vector<ObjectSpec> db;

public:
    void registerObject(const ObjectSpec& obj);
    void loadFromYaml(const std::string& pkg_name, const std::string& file_path); 
    std::string findBestMatch(int h, int s, int v, int r, int g, int b);
};

#endif