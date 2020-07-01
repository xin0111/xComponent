#include "TrialCheck.h"
#include <io.h>
#include <time.h>
#include <fstream>
#include <sstream>
#include <mutex>

#ifdef _WIN32
#pragma warning(disable:4996)  
#endif

#define LEN_LIST 128

std::string int2string(int n)
{
	std::stringstream ss;
	std::string s;
	ss << n;
	ss >> s;
	return s;
}

int string2int(const std::string& s)
{
	return atoi(s.c_str());
}

void swap_byte(char*x, char* y)
{//数据交换
	*x = (*x) ^ (*y);
	*y = (*x) ^ (*y);
	*x = (*x) ^ (*y);
}

void CreateKeylist(const char* key, int Lenkey, char* state)
{//state是生成的密匙序列
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

void CreateCipher(char* buffer, int Lenbuffer, char* state)
{//
	unsigned char x = 0;
	unsigned char y = 0;
	unsigned char x_or = 0;
	int counter;

	for (counter = 0; counter < Lenbuffer; counter++)
	{
		x = (x + 1) % LEN_LIST;
		y = (state[x] + y) % LEN_LIST;
		swap_byte(&state[x], &state[y]);
		x_or = (state[x] + state[y]) % LEN_LIST;
		buffer[counter] ^= state[x_or];
	}
}

void xorEncryptDecrypt(std::string &data, char key)
{
	//采用异或加密	
	int size = data.size();
	for (int i = 0; i < size; i++) {
		data[i] = data[i] ^ key;
	}
}

int getToday()
{
	time_t timep;
	time(&timep);
	char tmp[32];
	strftime(tmp, sizeof(tmp), "%Y%m%d", localtime(&timep));
	return string2int(tmp);
}
std::string keyOut(char sKeyOut[LEN_LIST],int today)
{
	std::string skey = "hympsdk";

	skey.append(int2string(today));
	//
	xorEncryptDecrypt(skey, 100);
	CreateKeylist(skey.c_str(), skey.size(), sKeyOut);

	return skey;
}

void encryptTime(const char* sTime, int nSize,
	char sKeyOutCopy[LEN_LIST], std::string &sEncrypt)
{
	char* sbuffer = new char[nSize];
	memcpy(sbuffer, sTime, nSize*sizeof(char));
	CreateCipher(sbuffer, nSize, sKeyOutCopy);

	sEncrypt.append(std::string(sbuffer,nSize));

	delete[] sbuffer;
}

void decryptTime(const std::string &sKey, const std::string& data, std::string& sOut)
{
	char sKeyOut[LEN_LIST];
	CreateKeylist(sKey.c_str(), sKey.size(), sKeyOut);

	int nSize = data.size();
	char* sbuffer = new char[nSize];
	memcpy(sbuffer, data.c_str(), nSize*sizeof(char));
	CreateCipher(sbuffer, nSize, sKeyOut);

	sOut.append(std::string(sbuffer, nSize));
	delete[] sbuffer;
}

//////////////////////////////////////////////////////////////////////////
TrialCheck::TrialCheck()
{
	_isUseful = true;
	_bChecked = false;
#ifdef _WIN32
	_filepath = "C:/windows/hkey.db";
#else
	_filepath = "/usr/bin/hkey.db";
#endif // _WIN32
}

TrialCheck::~TrialCheck()
{
}

bool TrialCheck::isUseful(const std::string &sStart, const std::string &sEnd)
{
	if (_bChecked)	return _isUseful;

	std::mutex mt;
	mt.lock();
	//文件判断
	if ((_access(_filepath.c_str(), 0)) != -1)
	{
		//读取存储
		std::fstream fin(_filepath);
		std::string sLine;
		int index = 0;
		std::string skey, sLastUsedKey;
		while (getline(fin,sLine))
		{
			if (index == 0)
			{
				skey = sLine;
			}
			else if (index == 1)
			{
				sLastUsedKey = sLine;
			}
			++index;
		}
		fin.close();
		if (skey.empty()&& sLastUsedKey.empty())
		{
			_isUseful = false;
		}
		else
		{
			//判断最后使用日期
			std::string sLastUsedDay;
			decryptTime(skey, sLastUsedKey, sLastUsedDay);
			xorEncryptDecrypt(skey, 100);//key:hympsdk+date
			if (skey.find(sLastUsedDay) != -1)
			{
				int lastUsedDay = string2int(sLastUsedDay);
				int startDay = string2int(sStart);
				int endDay = string2int(sEnd);
				int today = getToday();

				//存储时间(today)在日期内
				_isUseful = (lastUsedDay >= startDay 
					&& lastUsedDay <= endDay && 
					today >= startDay && today<=endDay);
			}
			else
			{
				_isUseful = false;
			}
		}
	}
	
	writeUsdDay(sStart, sEnd);
	mt.unlock();
	_bChecked = true;
	return _isUseful;
}

bool TrialCheck::writeUsdDay(const std::string &sStart, const std::string &sEnd)
{
	char sKeyOut[LEN_LIST];
	int today = getToday();
	std::string skey = keyOut(sKeyOut, today);
	
	int startDay = string2int(sStart);
	int endDay = string2int(sEnd);

	_isUseful = (today >= startDay && today <= endDay);

	std::string sToday = int2string(today);

	std::string sTodayKey;
	encryptTime(sToday.c_str(), sToday.length(), sKeyOut, sTodayKey);
	//写入文件
	std::fstream out;
	out.open(_filepath, std::ios::out | std::ios::binary);
	if (!out.is_open())
	{
		return false;
	}
	out.write(skey.c_str(), skey.size());
	out << '\n';
	out.write(sTodayKey.c_str(), sTodayKey.size());
	out.close();

	return _isUseful;
}
