#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

#include "../include/t2fs.h"
#include "../include/apidisk.h"

/**
Exemplo de rotina usada para listar todos os nomes de arquivos existentes na área de diretório do T2FS
Utiliza as funções t2fs_first e t2fs_next
*/
void    Directory(void)
{
    int i;
    int count;
    t2fs_find dir;
    t2fs_record rec;
    
    
    if (t2fs_first (&dir))
    {
        printf ("Erro ao abrir o diretorio.\n");
        return;
    }

    char *name = malloc(sizeof(rec.name)+1);
    if (name==NULL)
    {
        printf ("Erro ao ler o diretorio.\n");
        return;
    }

    count=0;
    while ( (i=t2fs_next(&dir, &rec)) == 0 )
    {
        printf ("%s\n", (char *)rec.name);
        count++;
    }

    free(name);

    if (i!=1)
    {
        printf ("Erro ao ler o diretorio.\n");
        return;
    }
    if (count==0)
        printf ("Diretorio vazio.\n");
    else
        printf ("\nEncontrados %d arquivos.\n\n", count);
}

int main(void) {
    INIT();
    //Directory();
    t2fs_file file = t2fs_create ("gilson");
    
    char buff[256] = {'\0'};
    
    t2fs_seek (file, 5);
    
    t2fs_read (file, buff, 220);
    
    printf (buff);
    printf ("%s\n", t2fs_identify());
    
    return 0;
}
