#include "Jiami.h"
#include <time.h>
#include <stdio.h>
#include <fstream>
#include <stdarg.h>

void swap_byte(unsigned char*x, unsigned char* y)
{//数据交换
	*x = (*x) ^ (*y);
	*y = (*x) ^ (*y);
	*x = (*x) ^ (*y);
}

void CreateKeylist(unsigned char* key, int Lenkey, unsigned char* state)
{//根据密码生成密匙序列。其中key可以是用户自定义，或随机生成的密码；state是生成的密匙序列
	unsigned char x = 0;
	unsigned char y = 0;
	unsigned char counter;

	for (counter = 0; counter < LEN_LIST; counter++)
		state[counter] = counter;

	for (counter = 0; counter < LEN_LIST; counter++)
	{
		y = (key[x] + state[counter] + y) % LEN_LIST;
		swap_byte(&state[counter], &state[y]);
		x = (x + 1) % Lenkey;
	}
}

void CreateCipher(unsigned char* buffer, int Lenbuffer, unsigned char* state)
{//接收数据后进行解密/发送数据前进行加密
	unsigned char x = 0;
	unsigned char y = 0;
	unsigned char xor = 0;
	int counter;

	for (counter = 0; counter < Lenbuffer; counter++)
	{
		x = (x + 1) % LEN_LIST;
		y = (state[x] + y) % LEN_LIST;
		swap_byte(&state[x], &state[y]);
		xor = (state[x] + state[y]) % LEN_LIST;
		buffer[counter] ^= state[xor];
	}
}

int EncryptBufferToFile(char* buffer, int size, const char* filePath)
{//将内存中的一块数据加密并保存到文件中，返回值：1 函数执行成功，-1执行失败，文件打开失败
	unsigned char keylist[LEN_LIST];
	unsigned char tmp_Keylist[LEN_LIST];
	unsigned char key[8];
	int rtdata[24];
	srand((unsigned)time(NULL));

	for (int i = 0; i < 24; i++)
	{
		rtdata[i] = rand() & 0xffff;//生成随机数，并用随机数填充“1000”数据包其余的数据
	}
	key[0] = (rtdata[4] >> 8) & 0xff; /*第5个字的高8位对应密码的第1个字节，低8位对应密码的第2个字节*/
	key[1] = (rtdata[4]) & 0xff;
	key[2] = (rtdata[5] >> 8) & 0xff; /*第6个字的高8位对应密码的第3个字节，低8位对应密码的第4个字节*/
	key[3] = (rtdata[5]) & 0xff;
	key[4] = (rtdata[10] >> 8) & 0xff; /*第11个字的高8位对应密码的第5个字节，低8位对应密码的第6个字节*/
	key[5] = (rtdata[10]) & 0xff;
	key[6] = (rtdata[21] >> 8) & 0xff; /*第22个字的高8位对应密码的第7个字节，低8位对应密码的第8个字节*/
	key[7] = (rtdata[21]) & 0xff;
	CreateKeylist(key, 8, keylist); //生成密匙序列

	//将buffer每MAX_ENCRYPT_LEN为一段加密
	int  i = 0;
	while ((i + 1)*MAX_ENCRYPT_LEN < size)
	{
		memcpy(tmp_Keylist, keylist, LEN_LIST*sizeof(char));
		CreateCipher((unsigned char*)&buffer[i*MAX_ENCRYPT_LEN], MAX_ENCRYPT_LEN, tmp_Keylist);
		i++;
	}
	CreateCipher((unsigned char*)&buffer[i*MAX_ENCRYPT_LEN], size-MAX_ENCRYPT_LEN*i, keylist);

	//写入文件
	fstream out;
	out.open(filePath, ios::out | ios::binary);
	if (!out.is_open())
	{
#if LOG_INFO
		cout << "EncryptBufferToFile函数中打开\t" << filePath << "\t失败" << endl;
#endif
		return -1;
	}

	//密码
	out.write((char*)&rtdata, 24 * sizeof(int));
	//内容
	out.write(buffer, size);
	out.close();
	return 1;
}

int EncryptFileToFile(const char* sourceFilePath, const char* targetFilePath)
{//函数功能：将一个文件加密保存到另一个文件中
	fstream in;
	in.open(sourceFilePath, ios::in | ios::binary);
	if (!in.is_open())
	{
#if LOG_INFO
		cout << "EncryptFileToFile函数中打开\t" << sourceFilePath << "\t失败" << endl;
#endif
		return -1;
	}

	int fileSize, flag;
	char *content;
	in.seekg(0, ios::end);
	fileSize = (int)in.tellg();
	in.seekg(0, ios::beg);
	content = new char[fileSize];
	if (content)
	{
		in.read(content, fileSize);
		flag = EncryptBufferToFile(content, fileSize, targetFilePath);
		delete[] content;
		content = NULL;
	}
	return flag;
}

int DecryptFileToBuffer(const char* filePath, char*& buffer, int&size)
{//将文件解密读取到内存中
	fstream in;
	in.open(filePath, ios::in | ios::binary);
	if (!in.is_open())
	{
#if LOG_INFO
		cout << "DecryptFileToBuffer函数中打开\t" << filePath << "\t失败" << endl;
#endif
		return -1;
	}

	unsigned char keylist[LEN_LIST];
	unsigned char tmp_Keylist[LEN_LIST];
	unsigned char key[8];
	int rtdata[24];
	in.seekg(0, ios::end);
	size = (int)in.tellg() - 24 * sizeof(int);
	in.seekg(0, ios::beg);
	in.read((char*)&rtdata, 24 * sizeof(int));

	key[0] = (rtdata[4] >> 8) & 0xff; /*第5个字的高8位对应密码的第1个字节，低8位对应密码的第2个字节*/
	key[1] = (rtdata[4]) & 0xff;
	key[2] = (rtdata[5] >> 8) & 0xff; /*第6个字的高8位对应密码的第3个字节，低8位对应密码的第4个字节*/
	key[3] = (rtdata[5]) & 0xff;
	key[4] = (rtdata[10] >> 8) & 0xff; /*第11个字的高8位对应密码的第5个字节，低8位对应密码的第6个字节*/
	key[5] = (rtdata[10]) & 0xff;
	key[6] = (rtdata[21] >> 8) & 0xff; /*第22个字的高8位对应密码的第7个字节，低8位对应密码的第8个字节*/
	key[7] = (rtdata[21]) & 0xff;
	CreateKeylist(key, 8, keylist); //生成密匙序列
	
	buffer = new char[size];
	in.read(buffer, size);

	//将buffer每MAX_ENCRYPT_LEN为一段进行解密
	int i = 0;
	while ((i + 1)*MAX_ENCRYPT_LEN < size)
	{
		memcpy(tmp_Keylist, keylist, LEN_LIST*sizeof(char));
		CreateCipher((unsigned char*)&buffer[i*MAX_ENCRYPT_LEN], MAX_ENCRYPT_LEN, tmp_Keylist);
		i++;
	}
	CreateCipher((unsigned char*)&buffer[i*MAX_ENCRYPT_LEN], size-MAX_ENCRYPT_LEN*i, keylist);

	return 1;
}

int DecryptFileToFile(const char* sourceFilePath, const char* targetFilePath)
{//将目标文件解密并保存到另一个文件中
	char* buffer;
	int fileSize;
	int flag = DecryptFileToBuffer(sourceFilePath, buffer, fileSize);
	if (flag == -1)
	{
#if LOG_INFO
		cout << "DecryptFileToFile函数中\tDecryptFileToBuffer\t函数执行失败"  << endl;
#endif
		return -1;
	}

	fstream out;
	out.open(targetFilePath, ios::out | ios::binary);
	if (!out.is_open())
	{
#if LOG_INFO
		cout << "DecryptFileToFile函数中打开\t" << targetFilePath << "\t失败" << endl;
#endif
		return -1;
	}
	out.write(buffer, fileSize);
	out.close();
	return 1;
}
