#ifndef CONFIGMGR_H
#define CONFIGMGR_H

#include "global.h"

struct SectionInfo {
    SectionInfo(){}
    ~SectionInfo(){
        section_datas_.clear();
    }

    SectionInfo(const SectionInfo& src) {
        section_datas_ = src.section_datas_;
    }

    SectionInfo& operator = (const SectionInfo& src) {
        if (&src == this) {
            return *this;
        }

        this->section_datas_ = src.section_datas_;
        return *this;
    }

    std::string  operator[](const std::string  &key) {
        if (section_datas_.find(key) == section_datas_.end()) {
            return "";
        }
        // 这里可以添加一些边界检查  

        return section_datas_[key];
    }

    std::map<std::string, std::string> section_datas_;
};

class ConfigMgr
{
public:

    ~ConfigMgr() {
        config_map_.clear();
    }
    SectionInfo operator[](const std::string& section) {
        if (config_map_.find(section) == config_map_.end()) {
            return SectionInfo();
        }
        return config_map_[section];
    }

    //使用单例也可以，没用就直接用个全局的也行(其他地方都要用的话就用单例)
    static ConfigMgr& Inst() {
        static ConfigMgr cfg_mgr;
        return cfg_mgr;
    }

    ConfigMgr& operator=(const ConfigMgr& src) {
        if (&src == this) {
            return *this;
        }

        this->config_map_ = src.config_map_;
        return *this;
    };

    ConfigMgr(const ConfigMgr& src) {
        this->config_map_ = src.config_map_;
    }
    
private:
    ConfigMgr();

    boost::filesystem::path getConfigPath();

private:
    // 存储section和key-value对的map  
    std::map<std::string, SectionInfo> config_map_;
};

#endif // CONFIGMGR_H