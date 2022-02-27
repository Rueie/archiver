#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

void dnp_include_file(char *arch_path,char *file_path,char *file_name){
    
    int size_of_file;
    struct stat fileStatBuf;

    int in,out;
    in=open(file_path,O_RDONLY);
    if(in==-1){
        write(1,"Ошибка открытия потока:",strlen("Ошибка открытия потока:"));
        write(1,file_path,strlen(file_path));
        write(1,"\n",1);
        return;
    }

    fstat(in,&fileStatBuf);
    size_of_file=fileStatBuf.st_size;

    out=open(arch_path,O_WRONLY|O_APPEND);
    if(out==-1){
        write(1,"Ошибка открытия потока:",strlen("Ошибка открытия потока:"));
        write(1,arch_path,strlen(arch_path));
        write(1,"\n",1);
        close(in);
        return;
    }
    char byte;
    char buffer[256]={0};
    char helper[50]={0};
    strcat(buffer,"<!#begin_of_file!#");
    strcat(buffer,file_name);
    sprintf(helper,"%d",size_of_file);
    strcat(buffer,"!#");
    strcat(buffer,helper);
    strcat(buffer,"#!>");
    write(out,buffer,strlen(buffer));
    while(read(in,&byte,1)==1)
        write(out,&byte,1);
    memset(buffer,0,strlen(buffer));
    strcat(buffer,"<!#end_of_file#!>");
    write(out,buffer,strlen(buffer));
    close(in);
    close(out);
    return;
}

void bypass(char *folder_path,char *arch_path){
    DIR* directory;
    directory=opendir(folder_path);
    struct dirent *entry;
    struct stat statbuf;
    char buffer[256]={0};
    chdir(folder_path);

    while((entry=readdir(directory))!=NULL){//пока есть добро в каталоге
        lstat(entry->d_name,&statbuf);//записали данные о файле, считанном выше
        getcwd(buffer,256);
        strcat(buffer,"/");
        strcat(buffer,entry->d_name);

        if(S_ISDIR(statbuf.st_mode)){

            if(strcmp(".",entry->d_name)==0||strcmp("..",entry->d_name)==0)
                continue;
            int out=open(arch_path,O_WRONLY|O_APPEND);
            write(out,"<!#begin_of_folder!#",strlen("<!#begin_of_folder!#"));
            write(out,entry->d_name,strlen(entry->d_name));
            write(out,"#!>",strlen("#!>"));
            bypass(buffer,arch_path);
            write(out,"<!#end_of_folder#!>",strlen("<!#end_of_folder#!>"));
            close(out);
        }
        else if(strcmp(buffer,arch_path)!=0){
            dnp_include_file(arch_path,buffer,entry->d_name);
        }
        memset(buffer,0,strlen(buffer));
    }
    chdir("..");
    closedir(directory);
return;
}

void dnp_create(char *name, char *path){
    DIR *directory;
    char buffer[256];
    if((directory=opendir(path))==NULL){
        write(1,"Не существует директории:",strlen("Не существует директории:"));
        write(1,path,strlen(path));
        write(1,"\n",1);
        return;
    }
    write(1,"Cуществует директория:",strlen("Cуществует директория:"));
    write(1,path,strlen(path));
    write(1,"\n|",1);
    chdir(path);
    getcwd(buffer,256);

    if(strcmp(buffer,path)!=0){
        write(1,"Не удалось перейти в директорию:",strlen("Не удалось перейти в директорию:"));
        write(1,path,strlen(path));
        write(1,"\n",1);
        return;
    }
    write(1,"Удалось перейти в директорию:",strlen("Удалось перейти в директорию:"));
    write(1,path,strlen(path));
    write(1,"\n",1);
    strcat(buffer,"/");
    strcat(buffer,name);
    strcat(buffer,".dnp");
    FILE *arch=fopen(buffer,"a");
    if(arch==NULL){
        write(1,"Не удалось создать файл:",strlen("Не удалось создать файл:"));
        write(1,buffer,strlen(buffer));
        write(1,"\n",1);
    }
    write(1,"Удалось создать файл:",strlen("Удалось создать файл:"));
    write(1,buffer,strlen(buffer));
    write(1,"\n",1);

    chdir(path);


    bypass(path,buffer);

    return;
}

int dnp_unpack_file(char* arch_path,char* file_path,int offset){
    int arch,file;
    int size_of_file;
    char buffer[256]={0};
    char second_buffer[256]={0};
    char byte;

    arch=open(arch_path,O_RDONLY);//открыли архив
    lseek(arch,offset,SEEK_SET);//дошли до названия
    int counter=0;
    while(read(arch,&byte,1)==1){
        offset++;
        if(byte=='!')
            break;
        buffer[counter]=byte;
        counter++;
    }
    
    strcat(second_buffer,file_path);
    strcat(second_buffer,"/");
    strcat(second_buffer,buffer);

    /*write(1,"\narch_path:",strlen("\narch_path:"));
    write(1,arch_path,strlen(arch_path));
    write(1,"\nfile_path:",strlen("\nfile_path:"));
    write(1,second_buffer,strlen(second_buffer));*/

    FILE* fl=fopen(second_buffer,"a");
    fclose(fl);

    file=open(second_buffer,O_WRONLY);
    read(arch,&byte,1);
    offset++;
    memset(buffer,0,strlen(buffer));
    counter=0;
    while(read(arch,&byte,1)==1){
        offset++;
        if(byte=='#')
            break;
        buffer[counter]=byte;
        counter++;
    }
    size_of_file=atoi(buffer);

    //write(1,"\nsize_of_file:",strlen("\nsize_of_file:"));
    //write(1,buffer,strlen(buffer));

    read(arch,&byte,1);
    read(arch,&byte,1);
    offset+=2;

    while(read(arch,&byte,1)==1){
        if(byte=='<')
            break;
        write(file,&byte,1);
        offset++;
    }

    close(arch);
    close(file);

    return offset;
}

int out_bypass(char *arch_path,char *unpack_path,int offset){
    int in=open(arch_path,O_RDONLY);
    char byte;
    char massive_of_bytes[3]={0};
    char flag[256]={0};
    lseek(in,offset,SEEK_SET);
    DIR* directory;
    directory=opendir(unpack_path);
    while(read(in,&byte,1)!=0){
        offset++;
        for(int i=0;i<2;i++)
            massive_of_bytes[i]=massive_of_bytes[i+1];
        massive_of_bytes[2]=byte;
        if(strcmp(massive_of_bytes,"<!#")==0){
            int counter=0;
            memset(flag,0,strlen(flag));
            while((read(in,&byte,1)!=0)&&(byte!='!')&&(byte!='#')){
                flag[counter]=byte;
                counter++;
                offset++;
            }
            flag[counter]='\0';
            read(in,&byte,1);
            offset+=2;
            if(strcmp(flag,"begin_of_file")==0){
                memset(flag,0,strlen(flag));
                getcwd(flag,256);
                offset=dnp_unpack_file(arch_path,flag,offset);
                lseek(in,offset,SEEK_SET);
            }
            else if(strcmp(flag,"begin_of_folder")==0){
                char dir_name[256]={0};
                counter=0;
                while((read(in,&byte,1)==1)&&(byte!='#')){
                    offset++;
                    dir_name[counter]=byte;
                    counter++;
                }
                read(in,&byte,1);
                read(in,&byte,1);
                offset+=2;
                memset(flag,0,strlen(flag));
                getcwd(flag,256);
                strcat(flag,"/");
                strcat(flag,dir_name);
                mkdir(flag,0777);
                chdir(dir_name);
                offset=out_bypass(arch_path,flag,offset);
                lseek(in,offset,SEEK_SET);
            } 
            else if(strcmp(flag,"end_of_folder")==0){
                chdir("..");
                return offset;
            }
            
            massive_of_bytes[3]='\0';
        }
    }
    return offset;
}

void dnp_unpack(char *arch_path,char* arch_name,char *unpack_path){
    DIR *directory;
    char buffer[256];
    if((directory=opendir(arch_path))==NULL){
        write(1,"Не существует директории:",strlen("Не существует директории:"));
        write(1,arch_path,strlen(arch_path));
        write(1,"\n",1);
        return;
    }
    chdir(arch_path);
    getcwd(buffer,256);

    if(strcmp(buffer,arch_path)!=0){
        write(1,"Не удалось перейти в директорию:",strlen("Не удалось перейти в директорию:"));
        write(1,arch_path,strlen(arch_path));
        write(1,"\n",1);
        return;
    }

    strcat(buffer,"/");
    strcat(buffer,arch_name);
    int in=open(arch_name,O_RDONLY);
    if(in==-1){
        write(1,"Не удалось открыть данный файл или его не существует\n",strlen("Не удалось открыть данный файл или его не существует\n"));
        write(1,buffer,strlen(buffer));
        write(1,"\n",1);
        return;
    }

    if((directory=opendir(unpack_path))==NULL){
        write(1,"Не существует директории:",strlen("Не существует директории:"));
        write(1,unpack_path,strlen(unpack_path));
        write(1,"\n",1);
        return;
    }
    chdir(unpack_path);
    char second_buffer[256]={0};
    getcwd(second_buffer,256);

    if(strcmp(second_buffer,unpack_path)!=0){
        write(1,"Не удалось перейти в директорию:",strlen("Не удалось перейти в директорию:"));
        write(1,unpack_path,strlen(unpack_path));
        write(1,"\n",1);
        return;
    }
    close(in);
    out_bypass(buffer,unpack_path,0);
    return;
}

void dnp_info(char *arch_path){
    char flag[256]={0};
    int in=open(arch_path,O_RDONLY);
    char byte;
    char massive_of_bytes[3]={0};
    int offset=0;
    int depth=0;
    while(read(in,&byte,1)!=0){
        offset++;
        for(int i=0;i<2;i++)
            massive_of_bytes[i]=massive_of_bytes[i+1];
        massive_of_bytes[2]=byte;
        if(strcmp(massive_of_bytes,"<!#")==0){
            int counter=0;
            memset(flag,0,strlen(flag));
            while((read(in,&byte,1)!=0)&&(byte!='!')&&(byte!='#')){
                flag[counter]=byte;
                counter++;
                offset++;
            }
            flag[counter]='\0';
            read(in,&byte,1);
            offset+=2;
            if(strcmp(flag,"begin_of_file")==0){
                memset(flag,0,strlen(flag));
                counter=0;
                while((read(in,&byte,1)==1)&&(byte!='!')){
                    flag[counter]=byte;
                    counter++;
                    offset++;
                }
                read(in,&byte,1);
                offset+=2;

                for(int i=0;i<depth;i++)
                    write(1,"\t",1);
                write(1,flag,strlen(flag));
                write(1,"\n",1);

                memset(flag,0,strlen(flag));
                counter=0;
                while((read(in,&byte,1)==1)&&(byte!='#')){
                    flag[counter]=byte;
                    counter++;
                    offset++;
                }
                read(in,&byte,1);
                offset+=2;
                offset+=atoi(flag);

                lseek(in,offset,SEEK_SET);
            }
            else if(strcmp(flag,"begin_of_folder")==0){
                memset(flag,0,strlen(flag));
                counter=0;
                while((read(in,&byte,1)==1)&&(byte!='#')){
                    flag[counter]=byte;
                    counter++;
                    offset++;
                }
                read(in,&byte,1);
                offset+=2;

                for(int i=0;i<depth;i++)
                    write(1,"\t",1);
                write(1,"/",1);
                write(1,flag,strlen(flag));
                write(1,"\n",1);
                depth++;
            } 
            else if(strcmp(flag,"end_of_folder")==0){
                depth--;
            }
            
            massive_of_bytes[3]='\0';
        }
    }
    return;
}