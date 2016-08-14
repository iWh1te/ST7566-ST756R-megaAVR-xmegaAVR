/*
 * FloatToString.h
 *
 * Created: 11.02.2016 12:31:19
 *  Author: Белолипецкий Андрей Александрович
 *  Модуль для конвертации из flaot в string
 */ 


#ifndef FLOATTOSTRING_H_
#define FLOATTOSTRING_H_

//-------------------------Для flaot------------------------------------------
int gpow(int n, int power)
{
	int res = 1;
	while(power--) res *= n;
	return res;
}

char *gftoa(double f, int dec)
{
	static char buf[16];
	char *p = buf + 15;
	int i = f * gpow(10, dec);
	int sign = i < 0 ? -1 : 1;
	i *= sign;
	do {
		*--p = '0' + (i % 10);
		i /= 10;
		if (--dec == 0) *--p = '.';
	}
	while (i != 0);

	if (dec > 0) {
		while (dec-- > 0)
		*--p = '0';
		*--p = '.';
	}
	
	if (*p == '.') *--p = '0';

	if (sign < 0) *--p = '-';

	return p;
}



#endif /* FLOATTOSTRING_H_ */