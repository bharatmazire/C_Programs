#include<stdio.h>

// 529 = 1000010001
// 5   = 101
// 7   = 111
//     = 1010110000


void ShowBinary(int num)
{
	while (num)
	{
		if (num & 1)
			printf("1");
		else
			printf("0");
		num = num >> 1;
	}
}

int GetBinaryGap(int num)
{
	int max_bin_gap = 0;

	while (num)
	{
		while (num & 1)
		{
			int temp_max_bin_gap = 0;
			num = num >> 1;
			while (num && (num & 1) == 0)
			{
				temp_max_bin_gap++;
				num = num >> 1;
			}
			if (max_bin_gap < temp_max_bin_gap)
				max_bin_gap = temp_max_bin_gap;
		}
		num = num >> 1;
	}
	return(max_bin_gap);
}

int main()
{
	printf("Binary Gap\n");

	int num = 5;

	ShowBinary(num);

	int bin_gap = GetBinaryGap(num);

	printf("\nMax Binary Gap is : %d \n", bin_gap);

	return(0);
}
