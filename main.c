#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "function.h"

int main(int argc,char *argv[]){

    if(strcmp("-create",argv[1])==0){
        write(1,"\n",1);
        dnp_create(argv[2],argv[3]);
    }
    else if(strcmp("-unpack",argv[1])==0){
        write(1,"\n",1);
        dnp_unpack(argv[2],argv[3],argv[4]);
    }
    else if(strcmp("-info",argv[1])==0){
        dnp_info(argv[2]);
        write(1,"\n",1);
    }
    else{
        char message[]="Данная команда не известна архиватору\n";
        write(1,message,strlen(message));
        write(1,"\n",1);
    }

    return 0;
}
