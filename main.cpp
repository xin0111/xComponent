#include "TrialCheck.h"
#include "AngleTools.h"

int main(int argc, char*argv[])
{
	TrialCheck ckeck;
	bool isOk = ckeck.isUseful("20200601", "20200705");

	double dout;
	AngleTools::convert02n(360, 181, dout);
	AngleTools::convert02n(360, 170, dout);

	return 0;
}