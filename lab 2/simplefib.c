/* nth fibonacci finding program */
#include <stdio.h>
//recursive function
int fib(int n)
{
	if (n < 2)
		return n;
	return fib(n - 1) + fib(n - 2);
}
int main()
{
	int i;
	printf("Enter the term :");
	scanf("%d", &i);
	printf("Answer : %d\n", fib(i));
	return 0;
}