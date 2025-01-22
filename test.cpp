#include <iostream>
#include <string>

int main() {
    // 普通ASCII字符串
    std::string ascii_str = "Hello World!";
    std::cout << "ASCII string: " << ascii_str << std::endl;
    std::cout << "Length: " << ascii_str.length() << std::endl;
    
    // 中文字符串 
    std::string chinese_str = "你好世界";
    std::cout << "Chinese string: " << chinese_str << std::endl; 
    std::cout << "Length: " << chinese_str.length() << std::endl;

    // 混合字符串
    std::string mixed_str = "Hello 世界";
    std::cout << "Mixed string: " << mixed_str << std::endl;
    std::cout << "Length: " << mixed_str.length() << std::endl;

    // 输出每个字符的数值
    std::cout << "\nBytes in Chinese string:" << std::endl;
    for(size_t i = 0; i < chinese_str.length(); i++) {
        std::cout << "Byte " << i << ": " << (int)(unsigned char)chinese_str[i] << std::endl;
    }

    return 0;
}
