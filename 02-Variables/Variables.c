#include <stdio.h>

int main(void)
{
    // variable declarations
    int		BSM_i = 7;
    float	BSM_f = 5.9f;
    double	BSM_d = 2.041997;
    char	BSM_c = 'B';
    
	//code
	printf("\n");
    printf("BSM_i = %d \n", BSM_i);
    printf("BSM_f = %f \n", BSM_f);
    printf("BSM_d = %lf\n", BSM_d);
    printf("BSM_c = %c \n", BSM_c);

    printf("\n\n");

    BSM_i = 23;
	BSM_f = 5.54f;
	BSM_d = 96.1294;
	BSM_c = 'S';
    
    printf("BSM_i = %d \n", BSM_i);
    printf("BSM_f = %f \n", BSM_f);
    printf("BSM_d = %lf\n", BSM_d);
    printf("BSM_c = %c \n", BSM_c);

    return(0);
}
