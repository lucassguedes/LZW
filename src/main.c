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
		bool save_model = false;
		bool load_model = false;
		char* model_filepath = NULL;


		if(argc > 4){
			for(int i = 4; i < argc; i++){
				printf("argv[i] = %s\n", argv[i]);
				if(!strcmp(argv[i], "--save-model")){
					save_model = true;	
					continue;
				}

				if(!strcmp(argv[i], "--load-model") && !load_model){
					if(i + 1 < argc){
						printf("Carregando modelo...\n");
						load_model = true;
						model_filepath = (char*)malloc(sizeof(char)*(strlen(argv[i+1]) + 1));
						strcpy(model_filepath, argv[i+1]);
					}
					i++;
				}

			}
		}

		compress_file(argv[1], argv[2], model_filepath, save_model, load_model);
		free(model_filepath);
	}

	if(!strcmp(argv[3], "--decompress")){
		decompress_file(argv[1], argv[2]);
	}

	return 0;
}
