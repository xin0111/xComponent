#ifndef ANGLETOOLS_H__
#define ANGLETOOLS_H__



class AngleTools
{
public:
	AngleTools();
	~AngleTools();

	/*
		Method:    convert02n
		.......    (0~n) -> 转正负角度

		[IN]:  dN , 最大角度
		[IN]:  dIn, 最大角度范围内角度
		[OUT]: Out, 正负角度
	*/
	static void convert02n(double dN, double dIn, double& dOut);
private:

};

#endif // ANGLETOOLS_H__
