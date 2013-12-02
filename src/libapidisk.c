#include <stdio.h>

int read_block (unsigned int bloco, char *buffer) {

    FILE *file = fopen ("t2fs_disk.dat", "r");
    
    if (file == NULL)
        return -1;
    
    if (fseek (file, bloco*256, SEEK_SET))
        return -1;
        
    if(fread (buffer, 1, 256, file) == 0)
        return -1;
    
    fclose (file);
    return 0;
}
int write_block (unsigned int bloco, char *buffer) {

    FILE *file = fopen ("t2fs_disk.dat", "r+");
    
    if (file == NULL)
        return -1;
        
    if (fseek (file, bloco*256, SEEK_SET))
        return -1;

    if(fwrite (buffer, 1, 256, file) == 0)
        return -1;
    
    fclose (file);
    return 0;
}
