2100720101编译原理
递归向下算法的语法分析器（没有集成版）
能够实现头歌过关项目
完成token PL/0编译语言的识别

### 主要数据结构和变量
- `charTable`: 字符表，用于存储输入程序的所有字符。
- `wordTable`: 单词表，用于存储输入程序的所有单词。
- `word`: 用于存储单个字符，添加到`wordTable`中。
- `lineNumber`: 当前行号。
- `reservedWord`: 保留字数组。
- `errorNum`和`errorLine`: 语法错误的计数和行号。

### 主要函数
- `LexicalAnalysis()`: 词法分析函数，它扫描`charTable`，识别不同类型的单词（如标识符、数字、运算符等），并将它们存储在`wordTable`中。
- `GrammaticalAnalysis()`: 语法分析函数，它根据词法分析的结果（`wordTable`中存储的单词）进行语法检查。
- `StrConnect(char* str, char ch)`: 用于拼接字符串。
- `StringLength(char* str)`: 计算字符串长度。
- `IsChar(char ch)`, `IsOperator(char ch)`, `IsDigit(char ch)`, `IsBoundary(char ch)`: 用于判断字符类型的函数。
- `IsReservedWord(char* str)`: 判断字符串是否为保留字。
- `ProcessString(char ch)` 和 `ProcessNumber(char ch)`: 处理字符串和数字。
- `PrintError(int type)`: 打印语法错误。

### 词法分析
词法分析部分通过`LexicalAnalysis()`函数实现，它遍历字符表，识别出各种类型的单词，如：
- 字符和字符串（`ProcessString()`）
- 数字（`ProcessNumber()`）
- 注释（单行`//`和多行`/* ... */`）
- 保留字和标识符
- 运算符和界符

### 语法分析
语法分析部分通过`GrammaticalAnalysis()`函数实现，它使用词法分析的结果进行语法检查，包括：
- 常量定义（`constant()`）
- 变量定义（`variable()`）
- 子程序定义（`subroutine()`）
- 语句分析（`sentence()`），包括`if`、`while`、`call`、`read`、`write`、`begin` ... `end`等
- 条件、表达式、项、因子、标识符和数字的分析

### 主函数（`main()`）
- 程序从用户那里读取一个字符串，直到遇到第一个句点（`.`）为止，表示程序结束。
- 调用`LexicalAnalysis()`进行词法分析。
- 调用`GrammaticalAnalysis()`进行语法分析。

### 错误处理
- 程序在识别到语法错误时，会调用`PrintError()`函数打印错误信息。

### 程序流程
1. 读取程序字符串并存储在`charTable`。
2. 进行词法分析，将识别出的单词存储在`wordTable`。
3. 进行语法分析，检查程序结构的正确性。
4. 打印语法分析结果，包括可能的错误。
