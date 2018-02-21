
#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void CHECK_error(const char *file, int line, const char *func){
	char tmp[1024];
	sprintf(tmp, "[CHECK] %s:%d (%s): ", file, line, func);
	perror(tmp);
	abort();
}

void ReadLine(char *dst, size_t limit){
	size_t pos = 0;
	
	fflush(stdin);
	while( pos < limit - 1 ){
		char curr = fgetc(stdin);
		
		if( curr == '\n' ){
			// Ignore extra '\n'
			if( pos == 0 )
				continue;
			
			break;
		}
		
		dst[pos++] = curr;
	}
	
	dst[pos] = '\0';
	fflush(stdin);
}
