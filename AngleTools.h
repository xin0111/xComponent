#ifndef ANGLETOOLS_H__
#define ANGLETOOLS_H__



class AngleTools
{
public:
	AngleTools();
	~AngleTools();

	/*
		Method:    convert02n
		.......    (0~n) -> ת�����Ƕ�

		[IN]:  dN , ���Ƕ�
		[IN]:  dIn, ���Ƕȷ�Χ�ڽǶ�
		[OUT]: Out, �����Ƕ�
	*/
	static void convert02n(double dN, double dIn, double& dOut);
private:

};

#endif // ANGLETOOLS_H__
