
#pragma once
#include <iostream>
#include <float.h>
using namespace std;

#define LOG_INFO 1
#define MAX_ENCRYPT_LEN 1024
#define LEN_LIST 128

/*
函数功能 ：判断字符是否是空格
*/
inline bool isspace(char c)
{
	return (unsigned int)(c - 9) < 5u || c == ' ';
}

/*
函数功能：判断字符是否是数字 '0'---'9'
*/
inline bool isdigit(char c)
{
	return (unsigned int)(c - '0') < 10u;
}

//函数功能：将一个文件加密保存到另一个文件中
int EncryptFileToFile(const char* sourceFilePath, const char* targetFilePath);

//将目标文件解密并保存到另一个文件中
int DecryptFileToFile(const char* sourceFilePath, const char* targetFilePath);
