#include <stdio.h>
int main(void)
{
	//code
    printf("\n");
	printf("-----------------------------------------\n");
    printf("\n");

    printf("\t Hello World !!!\n\n");

	int BSM_a = 13;

	printf("Integer Decimal Value Of 'BSM_a'											= %d\n", BSM_a);
	printf("Integer Octal Value Of 'BSM_a'												= %o\n", BSM_a);
	printf("Integer Hexadecimal Value Of 'BSM_a' (Hexadecimal Letters In Lower Case)	= %x\n", BSM_a);
	printf("Integer Hexadecimal Value Of 'BSM_a' (Hexadecimal Letters In Lower Case)	= %X\n", BSM_a);

	char BSM_ch = 'A';
	printf("Character BSM_ch = %c\n", BSM_ch);

	char BSM_str[] = "My Name is Bharat Shrikant Mazire !!";
	printf("String BSM_str = %s\n", BSM_str);

	long BSM_num = 30114421995L;
	printf("Long Integer BSM_num = %ld\n", BSM_num);

	unsigned int BSM_b = 7;
	printf("Unsigned Integer 'BSM_b' = %u\n", BSM_b);

	float BSM_f_num = 3012.1995f;
	printf("Floating Point Number With Just %%f 'BSM_f_num'		= %f	\n",	BSM_f_num);
	printf("Floating Point Number With %%4.2f	'BSM_f_num'		= %4.2f	\n",	BSM_f_num);
	printf("Floating Point Number With %%6.5f	'BSM_f_num'		= %6.5f	\n",	BSM_f_num);
	printf("Floating Point Number With %%11.5f	'BSM_f_num'		= %11.5f\n",BSM_f_num);

	double BSM_d_pi = 3.14159265358979323846;
	printf("Double Precision Floating Point Number Without Exponential					= %g \n", BSM_d_pi);
	printf("Double Precision Floating Point Number With Exponential (Lower Case)		= %e \n", BSM_d_pi);
	printf("Double Precision Floating Point Number With Exponential (Upper Case)		= %E \n", BSM_d_pi);
	printf("Double Hexadecimal Value Of 'BSM_d_pi' (Hexadecimal Letters In Lower Case)	= %x \n", BSM_d_pi);
	printf("Double Hexadecimal Value Of 'BSM_d_pi' (Hexadecimal Letters In Upper Case)	= %X \n", BSM_d_pi);
    
    printf("-----------------------------------------\n");
    
	return(0);
}
