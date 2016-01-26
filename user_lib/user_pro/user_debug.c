
#include "func.h"

void printd(char *p)
{
#ifdef DEBUGD
	char *str = p;
	int index = 0;
	
	while(*(str + index) != '\0' && *(str + index) != 'N')
	{
		USART1_send_char(*(str + index));
		index = index + 1;
	}
#else
	;
#endif
}
