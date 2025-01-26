#include "httpconnection.h"
#include "logicsystem.h"

//这里必须要移动构造初始化socket_，因为socket_不存在默认构造函数
HttpConnection::HttpConnection(boost::asio::io_context& ioc):socket_(ioc)
{

}

void HttpConnection::start(){
    auto self = shared_from_this();
    http::async_read(socket_, buffer_, request_, [self](beast::error_code ec,std::size_t bytes_transferred) {
            try {
                if (ec) {
                    std::cout << "http read err is " << ec.what() << std::endl;
                    return;
                }

                //处理读到的字节数，http我们不需要再做粘包处理
                //忽略未使用的bytes_transferred参数（字节数）
                boost::ignore_unused(bytes_transferred);
                self->handleRequest();

                //返回可能会超时，启动一个超时检测
                self->checkDeadline();

            }
            catch (std::exception& exp) {
                std::cout << "exception is " << exp.what() << std::endl;
            }
        }
    );   
     
}


void HttpConnection::handleRequest(){
    //设置版本
    response_.version(request_.version());
    //设置为短链接
    response_.keep_alive(false);

    if (request_.method() == http::verb::get) {   

        preParseGetParam();
        
        bool success = LogicSystem::GetInstance()->handleGet(get_url_, shared_from_this()); //LogicSystem逻辑类
        if (!success) {
            response_.result(http::status::not_found); //404
            response_.set(http::field::content_type, "text/plain"); //设置响应类型
            beast::ostream(response_.body()) << "url not found\r\n";
            writeResponse();
            return;
        }

        response_.result(http::status::ok); //200
        response_.set(http::field::server, "GateServer"); //设置服务器名称
        //beast::ostream(response_.body()) << "url not found\r\n";  这个就在逻辑层里面去处理了
        writeResponse();
        return;
    }

    if (request_.method() == http::verb::post) {
        bool success = LogicSystem::GetInstance()->handlePost(request_.target(), shared_from_this());
        if (!success) {
            response_.result(http::status::not_found);
            response_.set(http::field::content_type, "text/plain");
            beast::ostream(response_.body()) << "url not found\r\n";
            writeResponse();
            return;
        }

        response_.result(http::status::ok);
        response_.set(http::field::server, "CServer");
        writeResponse();
        return;
    }
}    

void HttpConnection::writeResponse(){
    auto self = shared_from_this();

    //虽然http底层帮我们切包了，我们不用处理粘包，但他切包原理也是通过包体的长度来切的，所以我们需要设置包体的长度
    response_.content_length(response_.body().size());

    http::async_write(socket_, response_, [self](beast::error_code ec,[[maybe_unused]] std::size_t bytes_transferred) {
        //关闭连接
        self->socket_.shutdown(tcp::socket::shutdown_send, ec);
        self->deadline_.cancel();  //取消超时检测，定时器 
        });
}

void HttpConnection::checkDeadline(){
    auto self = shared_from_this();
    //定时机制不需要我们再去写，这里只写一下如果出错，关闭连接  
    deadline_.async_wait([self](beast::error_code ec) {
        if (!ec) { //超时，正常到期表示没出错
            //关闭连接
            self->socket_.close(ec);
        } 
    });
}


/*
启动服务器，在浏览器输入`http://localhost:8080/get_test`

会看到服务器回包`receive get_test req`

如果我们输入带参数的url请求`http://localhost:8080/get_test?key1=value1&key2=value2`

会收到服务器反馈`url not found`

所以对于get请求带参数的情况我们要实现参数解析，我们可以自己实现简单的url解析函数

*/


// 将十进制字符转换为十六进制字符
// 参数:
//   x: 0-15之间的十进制数字
// 返回值:
//   对应的十六进制字符('0'-'9' 或 'A'-'F')
unsigned char toHex(unsigned char x)
{
    return  x > 9 ? x + 55 : x + 48; 
}

//将16进制转为十进制字符
unsigned char fromHex(unsigned char x)
{
    unsigned char y;
    if (x >= 'A' && x <= 'Z') y = x - 'A' + 10;
    else if (x >= 'a' && x <= 'z') y = x - 'a' + 10;
    else if (x >= '0' && x <= '9') y = x - '0';
    else assert(0);
    return y;
}

//url的编码和解码（规则是定好的一套）
std::string urlEncode(const std::string& str)
{
    std::string strTemp = "";
    size_t length = str.length();
    for (size_t i = 0; i < length; i++) //注意：Hello World! Length: 12  你好世界  Length: 12 
    {
        //判断是否仅有数字和字母构成
        if (isalnum((unsigned char)str[i]) ||
            (str[i] == '-') ||
            (str[i] == '_') ||
            (str[i] == '.') ||
            (str[i] == '~'))

            strTemp += str[i];  //原样拼接

        else if (str[i] == ' ') //为空字符拼成加号
            strTemp += "+";
        else
        {
            //其他字符需要提前加%并且高四位和低四位分别转为16进制这样拼接起来
            strTemp += '%';
             //汉字比如占两或三个字节，(unsigned char)会取低一个字节
            strTemp += toHex((unsigned char)str[i] >> 4); 
            strTemp += toHex((unsigned char)str[i] & 0x0F);

        }
    }
    return strTemp;
}

std::string urlDecode(const std::string& str)
{
    std::string strTemp = "";
    size_t length = str.length();
    for (size_t i = 0; i < length; i++)
    {
        //还原+为空
        if (str[i] == '+') strTemp += ' ';
        //遇到%将后面的两个字符从16进制转为char再拼接
        else if (str[i] == '%')
        {
            assert(i + 2 < length);
            unsigned char high = fromHex((unsigned char)str[++i]);
            unsigned char low = fromHex((unsigned char)str[++i]);
            strTemp += high * 16 + low;
        }
        else strTemp += str[i];
    }
    return strTemp;
}

void HttpConnection::preParseGetParam() {
    // 提取 URI  
    auto uri = request_.target();//_request.target()获取请求的uri
    // 查找查询字符串的开始位置（即 '?' 的位置）  
    auto query_pos = uri.find('?');
    if (query_pos == std::string::npos) {
        get_url_ = uri;
        return;
    }

    // 提取 get 请求的 url
    get_url_ = uri.substr(0, query_pos); //左闭右开
    // 提取 get 请求的参数
    std::string query_string = uri.substr(query_pos + 1);
    std::string key;
    std::string value;
    size_t pos = 0;
    while ((pos = query_string.find('&')) != std::string::npos) {
        auto pair = query_string.substr(0, pos);
        size_t eq_pos = pair.find('=');
        if (eq_pos != std::string::npos) {
            key = urlDecode(pair.substr(0, eq_pos));   
            value = urlDecode(pair.substr(eq_pos + 1));
            get_params_[key] = value;
        }
        query_string.erase(0, pos + 1); // 删除已处理的参数对
    }
    // 处理最后一个参数对 
    if (!query_string.empty()) {
        size_t eq_pos = query_string.find('=');
        if (eq_pos != std::string::npos) {
            key = urlDecode(query_string.substr(0, eq_pos));
            value = urlDecode(query_string.substr(eq_pos + 1));
            get_params_[key] = value;
        }
    }
}




