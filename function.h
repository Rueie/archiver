void dnp_create(char *name, char *path);
void dnp_include_file(char *arch_path,char *file_path,char *file_name);
void bypass(char *folder_path,char *arch_path);
void dnp_unpack(char *arch_path,char* arch_name,char *unpack_path);
int out_bypass(char *arch_path,char *unpack_path,int offset);
int dnp_unpack_file(char* arch_path,char* file_path,int offset);
void dnp_info(char *arch_path);