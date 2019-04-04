/* localtime example */
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>

int main ()
{

	// int t = time(NULL);
	// printf("%d\n",t );
	// sleep(2);
	// t = time(NULL);
	// printf("%d\n",t );


	// struct timeval c;
	// gettimeofday(&c, NULL);
	// printf("%ld %d\n", c.tv_sec, c.tv_usec);

	// int i=1001;
	// char s[sizeof(i)+1], integer_string[sizeof(i)+1];
	// int j;
	// sprintf(s, "%d", i);
	// // memcpy(s, &i, sizeof(i));

	// memcpy(integer_string, s, sizeof(i));
	// // integer_string[32]='\0';
	// int l = atoi(integer_string);

	// printf("number is %d\n", l);
	// return 0;


		// int i=1011;
		// char s[sizeof(i)+1], integer_string[sizeof(i)+1];
		// int j;
		// memcpy(s, &i, sizeof(i));
		// memcpy(&j, s, sizeof(i));
		// printf("number is %d\n", j);

		float p = 0.5;
		float r;
		srand(time(NULL));

		while(1){
		r = (float)rand()/((float)RAND_MAX+1);
		int ret = r<p ? 1: 0;
		printf("%d ", ret);
		sleep(0.1);
		}
		return 0;

}
