#include "configmgr.h"

ConfigMgr::ConfigMgr(){
    boost::filesystem::path config_path;
    try{
        // 获取当前工作目录  (构建目录)
        //boost::filesystem::path current_path = boost::filesystem::current_path();
        // 构建config.ini文件的完整路径 
        //boost::filesystem::path config_path = current_path / "config.ini";

        boost::filesystem::path config_path = getConfigPath();

        // 使用Boost.PropertyTree来读取INI文件  
        boost::property_tree::ptree pt;
        boost::property_tree::read_ini(config_path.string(), pt);


        // 遍历INI文件中的所有section  
        for (const auto& section_pair : pt) {
            const std::string& section_name = section_pair.first;
            const boost::property_tree::ptree& section_tree = section_pair.second;

            // 对于每个section，遍历其所有的key-value对  
            std::map<std::string, std::string> section_config;
            for (const auto& key_value_pair : section_tree) {
                const std::string& key = key_value_pair.first;
                const std::string& value = key_value_pair.second.get_value<std::string>();
                section_config[key] = value;
            }
            SectionInfo sectionInfo;
            sectionInfo.section_datas_ = section_config;
            // 将section的key-value对保存到config_map中  
            config_map_[section_name] = sectionInfo;
        }

    }catch(const std::exception& e){
        std::cerr << "Error: " << e.what() << std::endl;
    }

    // 输出所有的section和key-value对  
    // for (const auto& section_entry : config_map_) {
    //     const std::string& section_name = section_entry.first;
    //     SectionInfo section_config = section_entry.second;
    //     std::cout << "[" << section_name << "]" << std::endl;
    //     for (const auto& key_value_pair : section_config.section_datas_) {
    //         std::cout << key_value_pair.first << "=" << key_value_pair.second << std::endl;
    //     }
    // }

}

// 获取配置文件路径的函数
boost::filesystem::path ConfigMgr::getConfigPath() {
    // 1. 首先检查命令行参数或环境变量中指定的配置文件路径
    // 从环境变量中获取配置文件路径
    const char* config_path = std::getenv("CQCHAT_SERVER_CONFIG_PATH");
    // 如果环境变量中存在配置路径则直接返回
    if (config_path) {
        return boost::filesystem::path(config_path);
    }

    // 2. 检查相对于可执行文件的路径
    // 获取当前可执行文件的路径
    boost::filesystem::path exe_path = boost::filesystem::canonical("/proc/self/exe").parent_path();
    // 构造相对于可执行文件的配置文件路径
    boost::filesystem::path relative_config = exe_path / "../etc/cqchat_server/config.ini";
    // 如果相对路径存在则返回
    if (boost::filesystem::exists(relative_config)) {
        return relative_config;
    }

    // 3. 检查系统配置目录
    // 构造系统配置文件的路径
    boost::filesystem::path system_config = "/etc/cqchat_server/config.ini";
    // 如果系统配置文件存在则返回
    if (boost::filesystem::exists(system_config)) {
        return system_config;
    }

    // 4. 最后使用开发环境的路径
    // 获取当前源文件的路径
    boost::filesystem::path source_path(__FILE__);
    boost::filesystem::path dev_config = source_path.parent_path().parent_path().parent_path() / "etc" /  "config.ini";
    // 返回开发环境下的配置文件路径
    if (boost::filesystem::exists(dev_config)) {
        return dev_config;
    }

    // 如果以上路径都不存在，则抛出异常
    throw std::runtime_error("Failed to find config.ini");
}
