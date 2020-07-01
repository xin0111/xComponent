#pragma once
#include <string>

class TrialCheck
{
public:
	TrialCheck();
	~TrialCheck();

	bool isUseful(const std::string &sStart, const std::string &sEnd);

private:
	bool writeUsdDay(const std::string &sStart, const std::string &sEnd);
	bool _isUseful;
	bool _bChecked;
	std::string _filepath;
};

