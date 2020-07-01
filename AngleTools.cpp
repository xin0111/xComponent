#include "AngleTools.h"


AngleTools::AngleTools()
{
}


AngleTools::~AngleTools()
{
}

void AngleTools::convert02n(double dN, double dIn, double& dOut)
{
	double dhlf_n = dN * 0.5;
	dOut = dIn;
	if (dIn > dhlf_n)
	{
		dOut -= dN;
	}
}
