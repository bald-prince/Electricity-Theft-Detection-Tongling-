#include "HEXSTR.h"

char Value2Hex(const int value)
{
    char Hex = NULL;
    if (value>=0 && value<=9)
    {
        Hex = (char)(value+'0');
    }
    else if (value>9 && value<16)
    {
        Hex = (char)(value-10+'A');
    }
    return Hex;
}

int Str2Hex(char *str,char *hex)
{
    int high = 0;
    int low = 0;
    int temp = 0;

    if (NULL==str || NULL==hex){
        //printf("the str or hex is wrong\n");
        return -1;
    }

    if (0==strlen(str)){
        //printf("the input str is wrong\n");
        return -2;
    }

    while(*str)
    {
        temp = (int)(*str);
        high = temp>>4;
        low = temp & 15;
        *hex = Value2Hex(high);
        hex++;
        *hex = Value2Hex(low);
        hex++;
        str++;
    }
    *hex = '\0';
    return 0;
}
