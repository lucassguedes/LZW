#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file_formatter.h"
#include "hash.h"
#include "lzw.h"
#include "utils.h"



char utf8_ascii_table[256][256];

int main(int argc, char** argv){

	
	if(argc < 4){
		printf("\033[0;31mError:\033[0m Insufficient parameters.\n");
		return -1;
	}
	

	if(!strcmp(argv[3], "--format")){
		printf("Formatting...\n");
		initialize_convertion_table(utf8_ascii_table);
		format_file(argv[1], argv[2], utf8_ascii_table);
		return 0;
	}

	if(!strcmp(argv[3], "--compress")){
		if(argc > 4 && !strcmp(argv[4], "--save-model")){
			compress_file(argv[1], argv[2], 1);
			return 0;
		}
		compress_file(argv[1], argv[2], 0);
	}

	if(!strcmp(argv[3], "--decompress")){
		decompress_file(argv[1], argv[2]);
	}

	return 0;
}
