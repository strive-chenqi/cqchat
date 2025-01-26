const fs = require('fs');  //文件读写库
const path = require('path');  // 路径处理库

function getConfigPath() {
    // 1. 首先检查环境变量中指定的配置文件路径
    const envConfig = process.env.CQCHAT_VERIFY_SERVER_CONFIG;
    if (envConfig && fs.existsSync(envConfig)) {
        return envConfig;
    }

    // 2. 检查相对于可执行文件的路径
    const exePath = process.execPath;  // node 可执行文件路径
    const exeDir = path.dirname(exePath);
    const relativeConfig = path.join(exeDir, '../etc/cqchat_server/config.json');
    if (fs.existsSync(relativeConfig)) {
        return relativeConfig;
    }

    // 3. 检查系统配置目录
    const systemConfig = '/usr/local/etc/cqchat_server/config.json';
    if (fs.existsSync(systemConfig)) {
        return systemConfig;
    }

    // 4. 最后使用开发环境的路径
    const devConfig = path.join(__dirname, '../../etc/config.json');
    if (fs.existsSync(devConfig)) {
        return devConfig;
    }

    // 如果都找不到，抛出错误
    throw new Error('Failed to find config.json');
}

function loadConfig() {
    const configPath = getConfigPath();
    let config = JSON.parse(fs.readFileSync(configPath, 'utf8'));
    return config;
}


let config = loadConfig();
let email_user = config.email.user;
let email_pass = config.email.pass;
let mysql_host = config.mysql.host;
let mysql_port = config.mysql.port;
let redis_host = config.redis.host;
let redis_port = config.redis.port;
let redis_passwd = config.redis.passwd;


module.exports = {email_pass, email_user, mysql_host, mysql_port,redis_host, redis_port, redis_passwd}