// 定义TokenType枚举类型
enum class TokenType {
    KEYWORD,    // 保留字
    SYMBOL,     // 界符
    IDENTIFIER, // 标识符
    NUMBER,     // 无符号整数
    OPERATOR,   // 运算符
    EOF_TOKEN,   // 文件结束标记
    ILLEGIAL     //非法字符
};

// 定义Token结构体
struct Token {
    TokenType type;
    std::string value;
    int line;
    
    // 构造函数
    Token(TokenType t, const std::string& v, int l) : type(t), value(v), line(l) {}
    
    // 重载输出运算符，以便于打印Token信息
    friend std::ostream& operator<<(std::ostream& os, const Token& token);
};
std::vector<Token> tokens;
//std::string names[] = {"保留字", "界符", "标识符", "无符号整数", "运算符", "非法字符"};
