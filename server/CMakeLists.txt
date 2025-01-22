cmake_minimum_required(VERSION 3.14)
project(cqchat_server)

# 全局设置
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# 设置可执行文件输出目录为 server/bin
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
# 设置动态库输出目录为 build/lib
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
# 设置静态库输出目录为 build/lib
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)


# 添加编译选项
# -Wall: 启用所有常见的警告信息
# -Wextra: 启用额外的警告信息，比Wall更严格
add_compile_options(-Wall -Wextra)


# 添加子目录
add_subdirectory(src)


# 安装选项
# 定义一个选项INSTALL_PREFIX，用于指定安装路径前缀，默认为OFF
# option(INSTALL_PREFIX "Specify installation prefix" OFF)


# 包含 GNUInstallDirs 模块
#CMAKE_INSTALL_BINDIR: 二进制文件目录 (通常是 "bin")
#CMAKE_INSTALL_LIBDIR: 库文件目录 (通常是 "lib")
#CMAKE_INSTALL_INCLUDEDIR: 头文件目录 (通常是 "include")
#CMAKE_INSTALL_SYSCONFDIR: 配置文件目录 (通常是 "etc")
include(GNUInstallDirs)

if(UNIX)
    # 安装bin目录
    install(
        # 将源目录bin/下的文件安装到指定目标bin目录
        DIRECTORY ${CMAKE_SOURCE_DIR}/bin/
        #cmake -DCMAKE_INSTALL_PREFIX=/desired/install/path ..
        DESTINATION ${CMAKE_INSTALL_PREFIX}/bin  #默认情况下，CMAKE_INSTALL_PREFIX 的值是：/usr/local
        USE_SOURCE_PERMISSIONS  # 保持源文件的权限
        FILES_MATCHING PATTERN "*" # 匹配所有文件
        PATTERN "*.cmake" EXCLUDE  # 排除.cmake文件和CMakeFiles目录
        PATTERN "CMakeFiles" EXCLUDE
    )

    # 安装配置目录
    # 将源目录conf/下的.ini文件安装到指定目标conf目录
    # install(
    #     DIRECTORY ${CMAKE_SOURCE_DIR}/conf/
    #     DESTINATION ${CMAKE_INSTALL_PREFIX}/conf
    #     FILES_MATCHING PATTERN "*"
    # )
    install(
        FILES ${CMAKE_SOURCE_DIR}/conf/config.ini
        DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}cqchat_server   #usr/local/etc 路径默认是相对于 CMAKE_INSTALL_PREFIX 的，不需要显式添加
        RENAME config.ini.example
    )

    # 创建日志目录
    # 在指定目标路径下创建logs目录
    install(
        DIRECTORY
        DESTINATION ${CMAKE_INSTALL_PREFIX}/logs/cqchat_server
    )

    # 安装启动脚本
    install(
        PROGRAMS ${CMAKE_SOURCE_DIR}/scripts/start.sh
        DESTINATION ${CMAKE_INSTALL_BINDIR} # 默认是 /usr/local/bin 相对于 CMAKE_INSTALL_PREFIX。
        RENAME cqchat_server-start
    )

    # 安装 systemd service 文件
    install(
        FILES ${CMAKE_SOURCE_DIR}/scripts/cqchat_server.service
        DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/systemd/system
        RENAME cqchat_server.service
    )

endif()