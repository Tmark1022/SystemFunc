/*************************************************************************
 @ Author	: tmark
 @ Created Time	: Thu 04 Jun 2020 02:45:07 PM CST
 @ File Name	: coroutine.c
 @ Description	: 
 ************************************************************************/
// C program to demonstrate how coroutines 
// can be implemented in C. 
#include<stdio.h> 
  
int range(int a, int b) 
{ 
    static long long int i; 
    static int state = 0; 
    switch (state) 
    { 
	case 0: /* start of function */
		state = 1; 
        	for (i = a; i < b; i++) 
        	{ 
        	    return i; 
  
        	/* Returns control */
	case 1:; /* resume control straight 
        	            after the return */
        	} 
    } 
    state = 0; 
    return 0; 
} 

int range2(int a, int b) 
{ 
	static long long int i; 
	static int state = 0; 
	if (state)
		goto RESUME;

	state = 1; 
	for (i = a; i < b; i++) 
	{ 
		return i; 

		/* Returns control */
		/* resume control straight after the return */
RESUME :;
	} 
	state = 0; 
	return 0; 
} 


// Driver code 
int main() 
{ 
    int i; //For really large numbers 
  
    for (; (i=range2(1, 5));) 
        printf("control at main :%d\n", i); 
  
    return 0; 
}  
