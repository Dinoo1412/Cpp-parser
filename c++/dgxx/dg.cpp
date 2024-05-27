#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<string.h>
#include <iostream>
#include <sstream>
using namespace std;
 
int lineNumber = 1; // 当前行号
 
char charTable[2000];				// 字符表 —— 存储输入程序的 所有字符，包括空格、换行等
string wordTable[200] = { "\0" };	// 单词表 —— 存储输入程序的 所有单词，包括换行
char word[88] = "\0";				// 存储单个字符，添加到 wordTable 中
 
string ENDsign = "end.";	// 结束标识符
int charIndex = 0;			// 记录当前 charTable 字符个数
int wordIndex = 0;			// 记录当前 wordTable 单词个数
 
int errorNum = 0;			// 语法错误个数
int errorLine = 0;			// 语法错误行
 
string reservedWord[20] = { // 保留字
	"begin","call", "const","do","end","if",
	"odd","procedure", "read","then",
	"var","while","write"
};
 
 

void LexicalAnalysis();		// 词法分析
void GrammaticalAnalysis();	// 语法分析
 
void StrConnect(char* str, char ch);	// 拼接字符串
int StrLength(char* str);			// 计算字符串长度
 
int IsChar(char ch);			// 判断是否是字符
int IsOperator(char ch);		// 判断是否是运算符
int IsDigit(char ch);			// 判断是否是数字
int IsBoundary(char ch);		// 判断是是否界符
int IsReservedWord(char* str);	// 判断是否是保留字
 
void ProcessString(char ch);	// 判断字符串是否合法，合法则保存到单词表中
void ProcessNumber(char ch);	// 判断数字串是否合法，合法则保存到单词表中
 
void PrintError(int type);		//语法错误输出
 
int id();					// 变量名声明判断
void subroutine();			// 子程序
void constant();		// 常量定义
void variable();		// 变量定义
void sentence();			// 语句分析
void annotation();			// 注释
void condition();			// 条件分析
void expression();			// 表达式
void term();				// 项
void factor();				// 因子
void identifier();			// 标识符
void number();				// 数字
 
 
 
/* ------------ 函数定义 ------------ */
// 词法分析
void LexicalAnalysis() {
	while (charTable[charIndex] != '\0') { // 当前字符表未识别到结束符 '\0'
		if (charTable[charIndex] == '\0')
			break;
		if (charTable[charIndex] == ' ') {
			charIndex++;
			continue;
		}
		if (charTable[charIndex] == '\n') {
			wordTable[wordIndex++] = "\n";  // 将 换行符 保存到单词表 wordTable 中
			charIndex++;
			lineNumber++;	// 程序换行，行数加一
		}
		// 识别到字符
		else if (IsChar(charTable[charIndex])) {
			ProcessString(charTable[charIndex]); // 处理该字符之后的字符串
			continue;
		}
		// 识别到数字
		else if (IsDigit(charTable[charIndex])) {
			ProcessNumber(charTable[charIndex]); // 处理该数字之后的数字串
			continue;
		}
		// 识别到'/' 符
		else if (charTable[charIndex] == '/') {
			// 之后是 '/' ，说明是注释 //
			if (charTable[charIndex + 1] == '/') {
				wordTable[wordIndex++] = "//";
				charIndex = charIndex + 2;
				word[0] = '\0';
 
				while (charTable[charIndex] != '\n') // 将未换行的字符全部识别为注释
					StrConnect(word, charTable[charIndex++]);
				wordTable[wordIndex++] = word;
				wordTable[wordIndex++] = "\n";
				charIndex++;
				continue;
			}
			// 之后是 '*' ，说明是注释 /*
			else if (charTable[charIndex + 1] == '*') {
				wordTable[wordIndex++] = "/*";
				charIndex = charIndex + 2;
				while (!(charTable[charIndex] == '*' && charTable[charIndex + 1] == '/')) {
					if (charTable[charIndex] == '\n') {
						wordTable[wordIndex++] = "\n";
						charIndex++;
						continue;
					}
					word[0] = '\0';
					while (charTable[charIndex] != '\n') StrConnect(word, charTable[charIndex++]);
					wordTable[wordIndex++] = word;
					wordTable[wordIndex++] = "\n";
					charIndex++;
				}
				wordTable[wordIndex++] = "*/";
				charIndex = charIndex + 2;
				word[0] = '\0';
				continue;
			}
			// 是非法字符(串)
			else {
				while (charTable[charIndex] != ' ' && IsBoundary(charTable[charIndex]) != 1 && charTable[charIndex] != '\0') {
					StrConnect(word, charTable[charIndex]);
					charIndex++;
				}
				printf("(非法字符(串),%s,行号:%d)\n", word, lineNumber);
				word[0] = '\0';
				break;
			}
		}
		// 识别到比较符
		else if (charTable[charIndex] == '<' || charTable[charIndex] == '>') {
			charIndex++;
			// 是 <= 或者 >= 符
			if (charTable[charIndex] == '=') {
				if (charTable[charIndex - 1] == '<')
					wordTable[wordIndex] = "<="; // 保存单词表
				else
					wordTable[wordIndex] = ">="; // 保存单词表
				wordIndex++;
				charIndex++;
				continue;
			}
			// 是 < 或者 > 符
			else {
				if (charTable[charIndex - 1] == '<')
					wordTable[wordIndex] = "<"; // 保存单词表
				else
					wordTable[wordIndex] = ">"; // 保存单词表
				wordIndex++;
				continue;
			}
		}
		// 识别到':'
		else if (charTable[charIndex] == ':') {
			charIndex++;
			if (charTable[charIndex] == '=') {
				wordTable[wordIndex++] = ":="; // 保存单词表
				charIndex++;
				continue;
			}
			else {
				printf(":后面不是=号\n");
				continue;
			}
		}
		// 识别到运算符
		else if (IsOperator(charTable[charIndex])) {
			wordTable[wordIndex++] = charTable[charIndex]; // 保存单词表
			charIndex++;
			continue;
		}
		// 识别到界符
		else if (IsBoundary(charTable[charIndex])) {
			wordTable[wordIndex++] = charTable[charIndex]; // 保存单词表
			charIndex++;
			continue;
		}
		// 识别到非法字符(串)
		else {
			printf("(非法字符(串),%c,行号:%d)\n", charTable[charIndex], lineNumber);
			charIndex++;
		}
	}
}
// 语法分析
void GrammaticalAnalysis() {
	subroutine();
	if (!wordTable[wordIndex].compare(".")) {
		if (errorNum == 0)
			printf("语法正确\n");
	}
	else {
		PrintError(0); // 输出语法错误以及行号
		return;
	}
}
// 打印语法错误
void PrintError(int type) {
	if (type == 1) {
		if (errorLine != lineNumber) {
			errorLine = lineNumber;
			printf("(语法错误,行号:%d)\n", lineNumber);
			errorNum++;
		}
	}
	else {
		while (wordTable[wordIndex].compare("\n") != 0) 
			wordIndex++;
		if (errorLine != lineNumber) {
			errorLine = lineNumber;
			printf("(语法错误,行号:%d)\n", lineNumber);
			errorNum++;
		}
	}
}
 
 
// 拼接字符串
void StrConnect(char* str, char ch) {        
	int i = 0;
	while (str[i] != '\0') i++;
	str[i] = ch;
	str[i + 1] = '\0';
}
// 计算字符串长度
int StrLength(char* str) {     
	int i = 0;
	while (str[i] != '\0') 
		i++;
	return i;
}
 
// 判断是否是字符
int IsChar(char ch) {
	if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'))
		return 1;
	else
		return 0;
}
// 判断是否是运算符
int IsOperator(char ch) {                    
	if (ch == '>' || ch == '<' || ch == '=' || ch == '*' || ch == '-' || ch == '+' || ch == '#')
		return 1;
	else
		return 0;
}
// 判断是否是数字
int IsDigit(char ch) {               
	if ((ch >= '0' && ch <= '9'))
		return 1;
	else
		return 0;
}
// 判断是是否界符
int IsBoundary(char ch) {          
	if (ch == ';' || ch == '.' || ch == ',' || ch == '(' || ch == ')')
		return 1;
	else
		return 0;
}
// 判断是否是保留字
int IsReservedWord(char* str) {          
	for (int i = 0; i < 20; i++) 
		if (reservedWord[i].compare(str) == 0) 
			return 1;
	return 0;
}
 
// 判断字符串是否合法，合法则保存到单词表中
void ProcessString(char ch) {
	// 字符串 或 数字
	if (IsChar(ch) || IsDigit(ch)) { 
		StrConnect(word, ch);
		charIndex++;
		ProcessString(charTable[charIndex]); //处理下一个字符
	}
	// 运算符或界符等，表示当前单词停止
	else if (ch == ' ' || ch == '\n' || ch == '\0' || IsOperator(ch) || IsBoundary(ch)) {
		// 判断为保留字
		if (IsReservedWord(word)) {
			wordTable[wordIndex++] = word; // 保存到单词表 wordTable 中
			word[0] = '\0';	// word变量初始化
		}
		// 否则判断为标识符
		else {
			if (StrLength(word) >= 8) 
				printf("(标识符长度超长,%s,行号:%d)\n", word, lineNumber);
			else 
				wordTable[wordIndex++] = word; 
			word[0] = '\0';
		}
	}
	// 非法字符(串)
	else {
		StrConnect(word, ch);
		printf("(非法字符(串),%s,行号:%d)\n", word, lineNumber);
		charIndex++;
		word[0] = '\0';
	}
}
// 判断数字串是否合法，合法则保存到单词表中
void ProcessNumber(char ch) {
	// 数字
	if (IsDigit(ch)) {
		StrConnect(word, charTable[charIndex]);
		charIndex++;
		ProcessNumber(charTable[charIndex]);
	}
	// 无符号整数
	else if (!IsChar(ch)) {
		if (StrLength(word) >= 6) 
			printf("(无符号整数越界,%s,行号:%d)\n", word, lineNumber);
		else 
			wordTable[wordIndex++] = word; // 保存到单词表 wordTable 中
		word[0] = '\0';
	}
	// 非法字符(串)
	else {
		while (charTable[charIndex] != ' ' && IsBoundary(charTable[charIndex]) != 1 && charTable[charIndex] != '\0') {
			StrConnect(word, charTable[charIndex]);
			charIndex++;
		}
		printf("(非法字符(串),%s,行号:%d)\n", word, lineNumber);
		word[0] = '\0';
	}
}
 
// 变量名声明判断
int id() {
	// 拷贝当前读取的单词到word变量中
	int i = 0;
	for (i = 0; i < wordTable[wordIndex].length(); i++) 
		word[i] = wordTable[wordIndex][i];
	word[i] = '\0';
	// 判断该单词不是保留字且不为空
	if (!IsReservedWord(word) && word[0] != '.') {
		wordIndex++;
	}
	else PrintError(1);
}
// 子程序
void subroutine() {
	// 当前单词表匹配 const —— 常量定义
	if (!wordTable[wordIndex].compare("const")) {
		wordIndex++;
		constant();
		// 匹配 "," 说明存在多个变量定义 
		while (!wordTable[wordIndex].compare(",")) {
			wordIndex++;
			constant(); // 处理判断下一个常量
		}
		// 匹配 ";" 说明变量声明结束
		if (!wordTable[wordIndex].compare(";")) {
			wordIndex++;
			// 匹配换行符，进入下一行的识别
			if (!wordTable[wordIndex].compare("\n")) {
				wordIndex++;
				lineNumber++; // 函数加一
				annotation();
			}
			subroutine();
		}
		// 否则出错
		else {
			PrintError(0);
			if (!wordTable[wordIndex].compare("\n")) {
				wordIndex++;
				lineNumber++;
				annotation();
			}
			subroutine();
		}
		if (!wordTable[wordIndex].compare("\n")) {
			wordIndex++;
			lineNumber++;
			annotation();
		}
	}
	// 当前单词表匹配 var
	else if (!wordTable[wordIndex].compare("var")) {
		wordIndex++;
		variable(); // 读下一个单词，变量 语法判断
		// 匹配 "," 说明有多个变量
		while (!wordTable[wordIndex].compare(",")) {
			wordIndex++;
			variable();
		}
		// 匹配 ";" 说明变量声明结束
		if (!wordTable[wordIndex].compare(";")) {
			wordIndex++;
			while (!wordTable[wordIndex].compare("\n")) {
				wordIndex++;
				lineNumber++;
			}
			annotation();
			subroutine();
		}
		// 否则出错
		else {
			PrintError(0);
			if (!wordTable[wordIndex].compare("\n")) {
				wordIndex++;
				lineNumber++;
				annotation();
			}
			subroutine();
		}
 
	}
	// 当前单词表匹配 procedure
	else if (!wordTable[wordIndex].compare("procedure")) {
		wordIndex++;
		id();
		// 匹配 ";" 说明变量声明结束
		if (!wordTable[wordIndex].compare(";")) {
			wordIndex++;
			subroutine();
			if (!wordTable[wordIndex].compare(";")) {
				annotation();
				while (!wordTable[wordIndex].compare("\n")) {
					wordIndex++;
					lineNumber++;
					annotation();
				}
				wordIndex++;
			}
			else {
				PrintError(1);
 
			}
		} 
		// 否则出错
		else {
			PrintError(0);
			subroutine();
		}
		while (!wordTable[wordIndex].compare("procedure")) {
			wordIndex++;
			id();
			if (!wordTable[wordIndex].compare(";")) {
				wordIndex++;
				subroutine();
				if (!wordTable[wordIndex].compare(";")) wordIndex++;
				else  PrintError(1);
			}
			else {
				PrintError(0);
				subroutine();
			}
		}
	}
	// 当前单词表匹配 换行符
	if (!wordTable[wordIndex].compare("\n")) {
		wordIndex++;
		lineNumber++;
	}
 
	sentence(); // 迭代判断
}
// 常量
void constant() {    
	// 标识符处理判断
	identifier();
	// 匹配等号 = 
	if (!wordTable[wordIndex].compare("=")) {
		wordIndex++;
		number(); // 读入下一个数字进行判断
	}
	// 打印错误
	else PrintError(0);
}
// 变量
void variable() {    
	id();
}
// 语句分析
void sentence() {
	// 匹配 if 语句
	if (!wordTable[wordIndex].compare("if")) {
		wordIndex++;
		condition(); // 读下一个单词，条件 语法判断
		// 条件判断后，匹配 then 语句
		if (!wordTable[wordIndex].compare("then")) {
			wordIndex++;
			// 如果换行，行数加一
			if (!wordTable[wordIndex].compare("\n")) {
				wordIndex++;
				lineNumber++;
			}
			sentence(); // 读下一个单词，语句 语法判断
		}
		// 不匹配，报错
		else {
			PrintError(0);
			sentence();
		}
		if (!wordTable[wordIndex].compare("\n")) {
			wordIndex++;
			lineNumber++;
		}
	}
	// 匹配 while 语句
	else if (!wordTable[wordIndex].compare("while")) {
		wordIndex++;
		condition(); // 读下一个单词，条件 语法判断
		// 条件判断后，匹配 do 语句
		if (!wordTable[wordIndex].compare("do")) {
			wordIndex++;
			if (!wordTable[wordIndex].compare("\n")) {
				wordIndex++;
				lineNumber++;
			}
			sentence(); // 读下一个单词，语句 语法判断
		}
		// 不匹配，报错
		else {
			PrintError(1);
			if (!wordTable[wordIndex].compare("\n")) {
				wordIndex++;
				lineNumber++;
			}
			sentence();
		}
	}
	// 匹配 call 语句
	else if (!wordTable[wordIndex].compare("call")) {
		wordIndex++;
		id();
	}
	// 匹配 read 语句
	else if (!wordTable[wordIndex].compare("read")) {
		wordIndex++;
		if (!wordTable[wordIndex].compare("(")) {
			wordIndex++;
			id(); 
			// 匹配 "," 说明有多个变量
			while (!wordTable[wordIndex].compare(",")) {
				wordIndex++;
				id();
			}
			// "(" 匹配 ")" ，结束判断
			if (!wordTable[wordIndex].compare(")")) 
				wordIndex++;
			else  
				PrintError(1);
		}
		else if (!wordTable[wordIndex].compare("\n")) {
			wordIndex++;
			lineNumber++;
		}
		else  PrintError(0);
	}
	// 匹配 write 语句
	else if (!wordTable[wordIndex].compare("write")) {
		wordIndex++;
		if (!wordTable[wordIndex].compare("(")) {
			wordIndex++;
			expression(); // 读下一个单词 表达式语法判断
			// 匹配 "," 说明有多个表达式
			while (!wordTable[wordIndex].compare(",")) {
				wordIndex++;
				expression();
			}
			// "(" 匹配 ")" ，结束判断
			if (!wordTable[wordIndex].compare(")")) 
				wordIndex++;
			else if (!wordTable[wordIndex].compare("\n")) {
				wordIndex++;
				lineNumber++;
			}
			else PrintError(0);
		}
		else {
			PrintError(1);
			expression();
			while (!wordTable[wordIndex].compare(",")) {
				wordIndex++;
				expression();
			}
			if (!wordTable[wordIndex].compare(")")) wordIndex++;
			else if (!wordTable[wordIndex].compare("\n")) {
				wordIndex++;
				lineNumber++;
			}
			else PrintError(0);
		}
	}
	// 匹配 begin 语句
	else if (!wordTable[wordIndex].compare("begin")) {
		wordIndex++;
		if (!wordTable[wordIndex].compare("\n")) {
			wordIndex++;
			lineNumber++;
		}
		sentence();  // 读下一个单词 语句语法判断
		if (!wordTable[wordIndex].compare(";")) {
			wordIndex++;
			annotation();
			if (!wordTable[wordIndex].compare("\n")) {
				wordIndex++;
				lineNumber++;
			}
			sentence();
		}
		else {
			PrintError(0);
			sentence();
		}
 
		while (!wordTable[wordIndex].compare(";")) {
			wordIndex++;
			annotation();
			if (!wordTable[wordIndex].compare("\n")) {
				wordIndex++;
				lineNumber++;
			}
			sentence();
		}
		// 匹配 end 语句，说明语句判断结束
		if (!wordTable[wordIndex].compare("end")) {
			wordIndex++;
			if (!wordTable[wordIndex].compare("\n")) {
				wordIndex++;
				lineNumber++;
			}
		} else {
			PrintError(0);
			return;
		}
		if (!wordTable[wordIndex].compare("\n")) {
			wordIndex++;
			lineNumber++;
		}
	}
	// 匹配 换行符
	else if (!wordTable[wordIndex].compare("\n")) {
		wordIndex++;
		lineNumber++;
	}
	// 匹配 注解
	else if (!wordTable[wordIndex].compare("//") || !wordTable[wordIndex].compare("/*")) {
		annotation();
		subroutine();
	}
	else {
		int tt = 0;
		for (tt = 0; tt < wordTable[wordIndex].length(); tt++) word[tt] = wordTable[wordIndex][tt];
		word[tt] = '\0';
		if (!IsReservedWord(word) && word[0] != '.') {
			id();
			if (!wordTable[wordIndex].compare(":=")) wordIndex++;
			expression();
		}
	}
}
// 注释
void annotation() {
	// 匹配 "\n" 行数加一
	if (!wordTable[wordIndex].compare("\n")) {
		wordIndex++;
		lineNumber++;
	}
	// 匹配 "//" 为单行注解
	if (!wordTable[wordIndex].compare("//")) {
		wordIndex++;
		wordIndex++;
		while (!wordTable[wordIndex].compare("\n")) {
			wordIndex++;
			lineNumber++;
		}
		annotation();
	}
	// 匹配 "/*" 为多行注解
	else if (!wordTable[wordIndex].compare("/*")) {
		wordIndex++;
		while (wordTable[wordIndex].compare("*/")) {
			if (!wordTable[wordIndex].compare("\n")) {
				wordIndex++;
				lineNumber++;
			}
			else {
				wordIndex++;
			}
		}
		wordIndex++;
	}
	// 一直换行则行数一直加1
	while (!wordTable[wordIndex].compare("\n")) {
		wordIndex++;
		lineNumber++;
	}
}
// 条件分析
void condition() {        
	expression(); // 先进行 表达式 判断
	if (!wordTable[wordIndex].compare("=") || !wordTable[wordIndex].compare("#") || !wordTable[wordIndex].compare("<") || !wordTable[wordIndex].compare("<=") || !wordTable[wordIndex].compare(">") || !wordTable[wordIndex].compare(">=")) {
		wordIndex++;
		expression();
	}
	else if (!wordTable[wordIndex].compare("odd")) {
		wordIndex++;
		expression();
	}
	else 
		PrintError(0);
}
// 表达式
void expression() {       
	term(); // 先进行 项 判断
	if (!wordTable[wordIndex].compare("+") || !wordTable[wordIndex].compare("-")) {
		wordIndex++;
		term();
		while (!wordTable[wordIndex].compare("+") || !wordTable[wordIndex].compare("-")) {
			wordIndex++;
			term();
		}
 
	}
}
// 项
void term() {
	factor();// 先进行 因子 判断
	while (!wordTable[wordIndex].compare("*") || !wordTable[wordIndex].compare("/")) {
		wordIndex++;
		factor();
	}
}
// 因子
void factor() {       
	if (!wordTable[wordIndex].compare("(")) {
		wordIndex++;
		expression();
		if (!wordTable[wordIndex].compare(")")) {
			wordIndex++;
		}
		else PrintError(0);
	}
	else if (!wordTable[wordIndex].compare("\n")) {
		wordIndex++;
		lineNumber++;
	}
	else wordIndex++;
}
// 数字
void number() {
	wordIndex++;
}
// 标识符
void identifier() {
	wordIndex++;
}
 
 
int main() {
	// 程序字符串输入
	charTable[0] = '\0';
	scanf("%[^\.]", charTable); // 读取输入流中的字符，直到遇到第一个句点（'.'）为止
	charTable[StrLength(charTable)] = '.'; // 在程序字符串结尾添加'.'作为标识符
 
	// 进行词法分析
	LexicalAnalysis();
 
	// 进行语法分析
	charIndex = 0;
	wordIndex = 0;
	lineNumber = 1;
	GrammaticalAnalysis();
 
	return 0;
}