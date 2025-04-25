#include "utils.h"

void get_bin_str(Symbol* symb, char buffer[]){
	uint64_t value = symb->code.value;
	const int length = symb->code.length;
	buffer[length] = '\0';
	for(int i = 0; i < length; i++){
		if((value & 1) == 1){
			buffer[length - i - 1] = '1';
		}else{
			buffer[length - i - 1] = '0';
		}
		value >>= 1;
	}
}