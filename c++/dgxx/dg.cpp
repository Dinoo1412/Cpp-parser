#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<string.h>
#include <iostream>
#include <sstream>
using namespace std;
 
int lineNumber = 1; // ��ǰ�к�
 
char charTable[2000];				// �ַ��� ���� �洢�������� �����ַ��������ո񡢻��е�
string wordTable[200] = { "\0" };	// ���ʱ� ���� �洢�������� ���е��ʣ���������
char word[88] = "\0";				// �洢�����ַ�����ӵ� wordTable ��
 
string ENDsign = "end.";	// ������ʶ��
int charIndex = 0;			// ��¼��ǰ charTable �ַ�����
int wordIndex = 0;			// ��¼��ǰ wordTable ���ʸ���
 
int errorNum = 0;			// �﷨�������
int errorLine = 0;			// �﷨������
 
string reservedWord[20] = { // ������
	"begin","call", "const","do","end","if",
	"odd","procedure", "read","then",
	"var","while","write"
};
 
 

void LexicalAnalysis();		// �ʷ�����
void GrammaticalAnalysis();	// �﷨����
 
void StrConnect(char* str, char ch);	// ƴ���ַ���
int StrLength(char* str);			// �����ַ�������
 
int IsChar(char ch);			// �ж��Ƿ����ַ�
int IsOperator(char ch);		// �ж��Ƿ��������
int IsDigit(char ch);			// �ж��Ƿ�������
int IsBoundary(char ch);		// �ж����Ƿ���
int IsReservedWord(char* str);	// �ж��Ƿ��Ǳ�����
 
void ProcessString(char ch);	// �ж��ַ����Ƿ�Ϸ����Ϸ��򱣴浽���ʱ���
void ProcessNumber(char ch);	// �ж����ִ��Ƿ�Ϸ����Ϸ��򱣴浽���ʱ���
 
void PrintError(int type);		//�﷨�������
 
int id();					// �����������ж�
void subroutine();			// �ӳ���
void constant();		// ��������
void variable();		// ��������
void sentence();			// ������
void annotation();			// ע��
void condition();			// ��������
void expression();			// ���ʽ
void term();				// ��
void factor();				// ����
void identifier();			// ��ʶ��
void number();				// ����
 
 
 
/* ------------ �������� ------------ */
// �ʷ�����
void LexicalAnalysis() {
	while (charTable[charIndex] != '\0') { // ��ǰ�ַ���δʶ�𵽽����� '\0'
		if (charTable[charIndex] == '\0')
			break;
		if (charTable[charIndex] == ' ') {
			charIndex++;
			continue;
		}
		if (charTable[charIndex] == '\n') {
			wordTable[wordIndex++] = "\n";  // �� ���з� ���浽���ʱ� wordTable ��
			charIndex++;
			lineNumber++;	// �����У�������һ
		}
		// ʶ���ַ�
		else if (IsChar(charTable[charIndex])) {
			ProcessString(charTable[charIndex]); // ������ַ�֮����ַ���
			continue;
		}
		// ʶ������
		else if (IsDigit(charTable[charIndex])) {
			ProcessNumber(charTable[charIndex]); // ���������֮������ִ�
			continue;
		}
		// ʶ��'/' ��
		else if (charTable[charIndex] == '/') {
			// ֮���� '/' ��˵����ע�� //
			if (charTable[charIndex + 1] == '/') {
				wordTable[wordIndex++] = "//";
				charIndex = charIndex + 2;
				word[0] = '\0';
 
				while (charTable[charIndex] != '\n') // ��δ���е��ַ�ȫ��ʶ��Ϊע��
					StrConnect(word, charTable[charIndex++]);
				wordTable[wordIndex++] = word;
				wordTable[wordIndex++] = "\n";
				charIndex++;
				continue;
			}
			// ֮���� '*' ��˵����ע�� /*
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
			// �ǷǷ��ַ�(��)
			else {
				while (charTable[charIndex] != ' ' && IsBoundary(charTable[charIndex]) != 1 && charTable[charIndex] != '\0') {
					StrConnect(word, charTable[charIndex]);
					charIndex++;
				}
				printf("(�Ƿ��ַ�(��),%s,�к�:%d)\n", word, lineNumber);
				word[0] = '\0';
				break;
			}
		}
		// ʶ�𵽱ȽϷ�
		else if (charTable[charIndex] == '<' || charTable[charIndex] == '>') {
			charIndex++;
			// �� <= ���� >= ��
			if (charTable[charIndex] == '=') {
				if (charTable[charIndex - 1] == '<')
					wordTable[wordIndex] = "<="; // ���浥�ʱ�
				else
					wordTable[wordIndex] = ">="; // ���浥�ʱ�
				wordIndex++;
				charIndex++;
				continue;
			}
			// �� < ���� > ��
			else {
				if (charTable[charIndex - 1] == '<')
					wordTable[wordIndex] = "<"; // ���浥�ʱ�
				else
					wordTable[wordIndex] = ">"; // ���浥�ʱ�
				wordIndex++;
				continue;
			}
		}
		// ʶ��':'
		else if (charTable[charIndex] == ':') {
			charIndex++;
			if (charTable[charIndex] == '=') {
				wordTable[wordIndex++] = ":="; // ���浥�ʱ�
				charIndex++;
				continue;
			}
			else {
				printf(":���治��=��\n");
				continue;
			}
		}
		// ʶ�������
		else if (IsOperator(charTable[charIndex])) {
			wordTable[wordIndex++] = charTable[charIndex]; // ���浥�ʱ�
			charIndex++;
			continue;
		}
		// ʶ�𵽽��
		else if (IsBoundary(charTable[charIndex])) {
			wordTable[wordIndex++] = charTable[charIndex]; // ���浥�ʱ�
			charIndex++;
			continue;
		}
		// ʶ�𵽷Ƿ��ַ�(��)
		else {
			printf("(�Ƿ��ַ�(��),%c,�к�:%d)\n", charTable[charIndex], lineNumber);
			charIndex++;
		}
	}
}
// �﷨����
void GrammaticalAnalysis() {
	subroutine();
	if (!wordTable[wordIndex].compare(".")) {
		if (errorNum == 0)
			printf("�﷨��ȷ\n");
	}
	else {
		PrintError(0); // ����﷨�����Լ��к�
		return;
	}
}
// ��ӡ�﷨����
void PrintError(int type) {
	if (type == 1) {
		if (errorLine != lineNumber) {
			errorLine = lineNumber;
			printf("(�﷨����,�к�:%d)\n", lineNumber);
			errorNum++;
		}
	}
	else {
		while (wordTable[wordIndex].compare("\n") != 0) 
			wordIndex++;
		if (errorLine != lineNumber) {
			errorLine = lineNumber;
			printf("(�﷨����,�к�:%d)\n", lineNumber);
			errorNum++;
		}
	}
}
 
 
// ƴ���ַ���
void StrConnect(char* str, char ch) {        
	int i = 0;
	while (str[i] != '\0') i++;
	str[i] = ch;
	str[i + 1] = '\0';
}
// �����ַ�������
int StrLength(char* str) {     
	int i = 0;
	while (str[i] != '\0') 
		i++;
	return i;
}
 
// �ж��Ƿ����ַ�
int IsChar(char ch) {
	if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'))
		return 1;
	else
		return 0;
}
// �ж��Ƿ��������
int IsOperator(char ch) {                    
	if (ch == '>' || ch == '<' || ch == '=' || ch == '*' || ch == '-' || ch == '+' || ch == '#')
		return 1;
	else
		return 0;
}
// �ж��Ƿ�������
int IsDigit(char ch) {               
	if ((ch >= '0' && ch <= '9'))
		return 1;
	else
		return 0;
}
// �ж����Ƿ���
int IsBoundary(char ch) {          
	if (ch == ';' || ch == '.' || ch == ',' || ch == '(' || ch == ')')
		return 1;
	else
		return 0;
}
// �ж��Ƿ��Ǳ�����
int IsReservedWord(char* str) {          
	for (int i = 0; i < 20; i++) 
		if (reservedWord[i].compare(str) == 0) 
			return 1;
	return 0;
}
 
// �ж��ַ����Ƿ�Ϸ����Ϸ��򱣴浽���ʱ���
void ProcessString(char ch) {
	// �ַ��� �� ����
	if (IsChar(ch) || IsDigit(ch)) { 
		StrConnect(word, ch);
		charIndex++;
		ProcessString(charTable[charIndex]); //������һ���ַ�
	}
	// ����������ȣ���ʾ��ǰ����ֹͣ
	else if (ch == ' ' || ch == '\n' || ch == '\0' || IsOperator(ch) || IsBoundary(ch)) {
		// �ж�Ϊ������
		if (IsReservedWord(word)) {
			wordTable[wordIndex++] = word; // ���浽���ʱ� wordTable ��
			word[0] = '\0';	// word������ʼ��
		}
		// �����ж�Ϊ��ʶ��
		else {
			if (StrLength(word) >= 8) 
				printf("(��ʶ�����ȳ���,%s,�к�:%d)\n", word, lineNumber);
			else 
				wordTable[wordIndex++] = word; 
			word[0] = '\0';
		}
	}
	// �Ƿ��ַ�(��)
	else {
		StrConnect(word, ch);
		printf("(�Ƿ��ַ�(��),%s,�к�:%d)\n", word, lineNumber);
		charIndex++;
		word[0] = '\0';
	}
}
// �ж����ִ��Ƿ�Ϸ����Ϸ��򱣴浽���ʱ���
void ProcessNumber(char ch) {
	// ����
	if (IsDigit(ch)) {
		StrConnect(word, charTable[charIndex]);
		charIndex++;
		ProcessNumber(charTable[charIndex]);
	}
	// �޷�������
	else if (!IsChar(ch)) {
		if (StrLength(word) >= 6) 
			printf("(�޷�������Խ��,%s,�к�:%d)\n", word, lineNumber);
		else 
			wordTable[wordIndex++] = word; // ���浽���ʱ� wordTable ��
		word[0] = '\0';
	}
	// �Ƿ��ַ�(��)
	else {
		while (charTable[charIndex] != ' ' && IsBoundary(charTable[charIndex]) != 1 && charTable[charIndex] != '\0') {
			StrConnect(word, charTable[charIndex]);
			charIndex++;
		}
		printf("(�Ƿ��ַ�(��),%s,�к�:%d)\n", word, lineNumber);
		word[0] = '\0';
	}
}
 
// �����������ж�
int id() {
	// ������ǰ��ȡ�ĵ��ʵ�word������
	int i = 0;
	for (i = 0; i < wordTable[wordIndex].length(); i++) 
		word[i] = wordTable[wordIndex][i];
	word[i] = '\0';
	// �жϸõ��ʲ��Ǳ������Ҳ�Ϊ��
	if (!IsReservedWord(word) && word[0] != '.') {
		wordIndex++;
	}
	else PrintError(1);
}
// �ӳ���
void subroutine() {
	// ��ǰ���ʱ�ƥ�� const ���� ��������
	if (!wordTable[wordIndex].compare("const")) {
		wordIndex++;
		constant();
		// ƥ�� "," ˵�����ڶ���������� 
		while (!wordTable[wordIndex].compare(",")) {
			wordIndex++;
			constant(); // �����ж���һ������
		}
		// ƥ�� ";" ˵��������������
		if (!wordTable[wordIndex].compare(";")) {
			wordIndex++;
			// ƥ�任�з���������һ�е�ʶ��
			if (!wordTable[wordIndex].compare("\n")) {
				wordIndex++;
				lineNumber++; // ������һ
				annotation();
			}
			subroutine();
		}
		// �������
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
	// ��ǰ���ʱ�ƥ�� var
	else if (!wordTable[wordIndex].compare("var")) {
		wordIndex++;
		variable(); // ����һ�����ʣ����� �﷨�ж�
		// ƥ�� "," ˵���ж������
		while (!wordTable[wordIndex].compare(",")) {
			wordIndex++;
			variable();
		}
		// ƥ�� ";" ˵��������������
		if (!wordTable[wordIndex].compare(";")) {
			wordIndex++;
			while (!wordTable[wordIndex].compare("\n")) {
				wordIndex++;
				lineNumber++;
			}
			annotation();
			subroutine();
		}
		// �������
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
	// ��ǰ���ʱ�ƥ�� procedure
	else if (!wordTable[wordIndex].compare("procedure")) {
		wordIndex++;
		id();
		// ƥ�� ";" ˵��������������
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
		// �������
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
	// ��ǰ���ʱ�ƥ�� ���з�
	if (!wordTable[wordIndex].compare("\n")) {
		wordIndex++;
		lineNumber++;
	}
 
	sentence(); // �����ж�
}
// ����
void constant() {    
	// ��ʶ�������ж�
	identifier();
	// ƥ��Ⱥ� = 
	if (!wordTable[wordIndex].compare("=")) {
		wordIndex++;
		number(); // ������һ�����ֽ����ж�
	}
	// ��ӡ����
	else PrintError(0);
}
// ����
void variable() {    
	id();
}
// ������
void sentence() {
	// ƥ�� if ���
	if (!wordTable[wordIndex].compare("if")) {
		wordIndex++;
		condition(); // ����һ�����ʣ����� �﷨�ж�
		// �����жϺ�ƥ�� then ���
		if (!wordTable[wordIndex].compare("then")) {
			wordIndex++;
			// ������У�������һ
			if (!wordTable[wordIndex].compare("\n")) {
				wordIndex++;
				lineNumber++;
			}
			sentence(); // ����һ�����ʣ���� �﷨�ж�
		}
		// ��ƥ�䣬����
		else {
			PrintError(0);
			sentence();
		}
		if (!wordTable[wordIndex].compare("\n")) {
			wordIndex++;
			lineNumber++;
		}
	}
	// ƥ�� while ���
	else if (!wordTable[wordIndex].compare("while")) {
		wordIndex++;
		condition(); // ����һ�����ʣ����� �﷨�ж�
		// �����жϺ�ƥ�� do ���
		if (!wordTable[wordIndex].compare("do")) {
			wordIndex++;
			if (!wordTable[wordIndex].compare("\n")) {
				wordIndex++;
				lineNumber++;
			}
			sentence(); // ����һ�����ʣ���� �﷨�ж�
		}
		// ��ƥ�䣬����
		else {
			PrintError(1);
			if (!wordTable[wordIndex].compare("\n")) {
				wordIndex++;
				lineNumber++;
			}
			sentence();
		}
	}
	// ƥ�� call ���
	else if (!wordTable[wordIndex].compare("call")) {
		wordIndex++;
		id();
	}
	// ƥ�� read ���
	else if (!wordTable[wordIndex].compare("read")) {
		wordIndex++;
		if (!wordTable[wordIndex].compare("(")) {
			wordIndex++;
			id(); 
			// ƥ�� "," ˵���ж������
			while (!wordTable[wordIndex].compare(",")) {
				wordIndex++;
				id();
			}
			// "(" ƥ�� ")" �������ж�
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
	// ƥ�� write ���
	else if (!wordTable[wordIndex].compare("write")) {
		wordIndex++;
		if (!wordTable[wordIndex].compare("(")) {
			wordIndex++;
			expression(); // ����һ������ ���ʽ�﷨�ж�
			// ƥ�� "," ˵���ж�����ʽ
			while (!wordTable[wordIndex].compare(",")) {
				wordIndex++;
				expression();
			}
			// "(" ƥ�� ")" �������ж�
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
	// ƥ�� begin ���
	else if (!wordTable[wordIndex].compare("begin")) {
		wordIndex++;
		if (!wordTable[wordIndex].compare("\n")) {
			wordIndex++;
			lineNumber++;
		}
		sentence();  // ����һ������ ����﷨�ж�
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
		// ƥ�� end ��䣬˵������жϽ���
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
	// ƥ�� ���з�
	else if (!wordTable[wordIndex].compare("\n")) {
		wordIndex++;
		lineNumber++;
	}
	// ƥ�� ע��
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
// ע��
void annotation() {
	// ƥ�� "\n" ������һ
	if (!wordTable[wordIndex].compare("\n")) {
		wordIndex++;
		lineNumber++;
	}
	// ƥ�� "//" Ϊ����ע��
	if (!wordTable[wordIndex].compare("//")) {
		wordIndex++;
		wordIndex++;
		while (!wordTable[wordIndex].compare("\n")) {
			wordIndex++;
			lineNumber++;
		}
		annotation();
	}
	// ƥ�� "/*" Ϊ����ע��
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
	// һֱ����������һֱ��1
	while (!wordTable[wordIndex].compare("\n")) {
		wordIndex++;
		lineNumber++;
	}
}
// ��������
void condition() {        
	expression(); // �Ƚ��� ���ʽ �ж�
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
// ���ʽ
void expression() {       
	term(); // �Ƚ��� �� �ж�
	if (!wordTable[wordIndex].compare("+") || !wordTable[wordIndex].compare("-")) {
		wordIndex++;
		term();
		while (!wordTable[wordIndex].compare("+") || !wordTable[wordIndex].compare("-")) {
			wordIndex++;
			term();
		}
 
	}
}
// ��
void term() {
	factor();// �Ƚ��� ���� �ж�
	while (!wordTable[wordIndex].compare("*") || !wordTable[wordIndex].compare("/")) {
		wordIndex++;
		factor();
	}
}
// ����
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
// ����
void number() {
	wordIndex++;
}
// ��ʶ��
void identifier() {
	wordIndex++;
}
 
 
int main() {
	// �����ַ�������
	charTable[0] = '\0';
	scanf("%[^\.]", charTable); // ��ȡ�������е��ַ���ֱ��������һ����㣨'.'��Ϊֹ
	charTable[StrLength(charTable)] = '.'; // �ڳ����ַ�����β���'.'��Ϊ��ʶ��
 
	// ���дʷ�����
	LexicalAnalysis();
 
	// �����﷨����
	charIndex = 0;
	wordIndex = 0;
	lineNumber = 1;
	GrammaticalAnalysis();
 
	return 0;
}