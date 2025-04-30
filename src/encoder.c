#include "encoder.h"
#include "utils.h"

void write_code_to_file(FILE *outfile, Token *sb, int code_length, uint8_t *outbuffer, int *outbuffer_length)
{

    int bits_to_write = code_length;
    uint64_t code = sb->code.value;

    char bin_str[100];

    sb->code.length = code_length;

    get_bin_str(sb, bin_str);

    printf("Escrevendo \033[0;32m%s (\"%s\")\033[0m na saída - %d bits\n", bin_str, sb->repr, code_length);

    int bits_to_ignore;
    while (bits_to_write > *outbuffer_length)
    {
        bits_to_ignore = bits_to_write - *outbuffer_length; // bits_to_ignore -> número de bits que serão deixados para o próximo byte

        // printf("\033[0;31mPrecisamos de %d bits, mas só temos %d bits...\n", bits_to_write, *outbuffer_length);

        *outbuffer = *outbuffer << *outbuffer_length; /*Liberando espaço para inserir um pedaço do código*/

        uint64_t code_slice = code >> bits_to_ignore; /*Extraindo o pedaço do código que cabe no byte atual*/
        *outbuffer = *outbuffer | code_slice;

        // printf("\t\033[0;31mNeste byte, vamos colocar apenas %d bits mais significativos do código, que terá valor %d\033[0m\n",*outbuffer_length, code_slice);

        code = code & ((int)pow(2, bits_to_ignore) - 1); /*Removendo a parte do código que está para ser escrita*/
        bits_to_write -= *outbuffer_length;

        /*Escrevendo na saída*/
        fputc(*outbuffer, outfile);
        *outbuffer = 0; /*Limpando o buffer*/
        *outbuffer_length = 8;
        // printf("\t\033[0;31mAgora restam %d bits, e o código restante é %d\033[0m\n",bits_to_write, code);
    }

    // printf("\033[0;32mFinalmente temos espaço o suficiente! Escreveremos %d bits restantes, com valor %d\033[0m\n",bits_to_write,code);
    *outbuffer = *outbuffer << bits_to_write;
    *outbuffer = *outbuffer | code;
    *outbuffer_length -= bits_to_write;

    if (*outbuffer_length == 0)
    {
        fputc(*outbuffer, outfile);
        *outbuffer = 0;
        *outbuffer_length = 8;
    }
}