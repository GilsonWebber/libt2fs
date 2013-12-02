#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include "../include/t2fs.h"
#include "../include/apidisk.h"

#define MAX_NAME_SIZE   40
#define OPEN_HANDLE     1
#define CLOSE_HANDLE    2
#define GET_HANDLE      3


/* Estrutura que armazena as informações do disco lógico, 
 * deve ser instanciado uma única vez no inicio da aplicação
 */

typedef struct
{
    unsigned char   id[4];
    uint8_t         version;
    uint8_t         ctrlSize;
    uint32_t        diskSize;
    uint16_t        blockSize;   
    uint16_t        freeBlockSize;  
    uint16_t        rootSize;  
    uint16_t        fileEntrySize;  
} __attribute__((packed)) t2fs_control;



/*
 * Handle de arquivo, armazena a entrada no diretório e um offset em bytes
 * relativo ao inicio do arquivo
 */

typedef struct
{
    t2fs_record rec;
    t2fs_file   file;
    unsigned int currentPointer;    
} t2fs_handle;


/****************** FUNCIONANDO ************************************/
t2fs_handle* 
fileHandle(const unsigned int action, const t2fs_file handle) // retorna um ponteiro para geração, nulo para descarte ou um inteiro para criação
{ 
    unsigned int static filesFlag = 0;
    unsigned int flag; // manipulador temporário
    
    t2fs_handle static openedFiles[32]; // vetor com as informações dos arquivos abertos
    
    switch (action) {
        case OPEN_HANDLE:
        {
            unsigned int cnt = 0;
            
            while (((filesFlag >> cnt) & 1) && (cnt++ < 32)); // retorna a primeira posição livre;
            
            if (cnt < 32)
                filesFlag |= (1 << cnt);
                openedFiles[cnt].file = cnt;
                return &openedFiles[cnt];                            
            return NULL;
        }    
        case CLOSE_HANDLE:
            if (handle < 32) {
                filesFlag &= ~(1 << handle);
                return (t2fs_handle*) 1;
            }
            return NULL;
        case GET_HANDLE:
            if (handle < 32){
                flag = 1 << handle;
                return (filesFlag & flag) ? &openedFiles[handle] : NULL;           
            }
            return NULL;
        default:
            return NULL;
    }   
}


t2fs_control controlBlock;

/**************************** FUNCIONANDO **************************/


int 
t2fs_write (t2fs_file handle, char *buffer, int size) {

return 0;
}

int 
t2fs_delete (char *nome) {

    t2fs_file file = t2fs_open (nome);
    
    if (file < 0)
        return -1;
    
    char blockData[controlBlock.blockSize];
    
   int blockQtd = controlBlock.freeBlockSize;
    
    read_block (controlBlock.ctrlSize, blockData);
    
    

    
    
    
    /* 
     * desmarcar blocos no bitmap
     * alterar nome da estrutura
     * salvar no disco
     */
     
     return 0;
}

/***************** TERMINADAS - FUNCIONANDO ***********************/


void 
INIT (void) {
    char blockData[8096];

    read_block (0, blockData);
    memcpy (&controlBlock, blockData, sizeof(t2fs_control));
}


char 
*t2fs_identify(void) {
    return "Gilson Webber - 192529";
}


t2fs_file 
t2fs_create (char *nome) {
    
    //t2fs_delete (nome);
    
    char blockData [controlBlock.blockSize];
    
    unsigned int startBlock = controlBlock.ctrlSize + controlBlock.freeBlockSize;
    
    unsigned int qtdBlock = controlBlock.rootSize + startBlock;
    
    unsigned int qtdRec = (int) controlBlock.blockSize/controlBlock.fileEntrySize;

    unsigned int found = 0;
 
    int i;

    while ((startBlock < qtdBlock) && (!found)) {
        read_block (startBlock, blockData);
        
        
        // localiza se existe entrada livre
        i = 0;
        while ((i < qtdRec) && (blockData[i*controlBlock.fileEntrySize] & 0x80)) i++;
        
        if (i < qtdRec)
            found = !found;
        else
            startBlock++;
    }

    if (found) {    
        t2fs_record newRecord;
        strncpy ((char*)newRecord.name, nome, MAX_NAME_SIZE);
        newRecord.name[0] |= 0x80; // marca bit 7 para indicar que entrada é válida   
        newRecord.blocksFileSize = 0;
        newRecord.bytesFileSize = 0;
        newRecord.dataPtr[0] = 0;
        newRecord.dataPtr[1] = 0;
        newRecord.singleIndPtr = 0;
        newRecord.doubleIndPtr = 0;
        
        memcpy (blockData + (i*controlBlock.fileEntrySize), &newRecord, controlBlock.fileEntrySize);
        write_block (startBlock, blockData);

        return t2fs_open (nome);
    }
    return -1;
}


t2fs_file
t2fs_open (char *nome) {

    t2fs_find findStruct;
    t2fs_record rec;
    
    if(!t2fs_first (&findStruct)) { // inicializa estrutura de procura de arquivos
        while (!t2fs_next(&findStruct, &rec)) { // procura o por registros válidos
            if (strncmp (nome,(char*) rec.name, MAX_NAME_SIZE) == 0) { // compara o nome para saber se é o mesmo
                
                t2fs_handle* handle = fileHandle (OPEN_HANDLE, 0);
                if (handle != NULL) {

                    handle->rec = rec;
                    handle->currentPointer = 0;
                
                    return handle->file;
                }
            }
        }
    }

    return -1; // não encontrou o arquivo
}


int 
t2fs_close (t2fs_file handle) {
    if (fileHandle (CLOSE_HANDLE, handle) == (void*) 1)
        return 0;
    return -1;
}

int 
t2fs_seek (t2fs_file handle, unsigned int offset) {

    t2fs_handle *file = fileHandle(GET_HANDLE, handle);
    
    if (file == NULL)
        return -1;
    if (file->rec.bytesFileSize < offset)
        return -1;
        
    file->currentPointer = offset;
    return 0;
}


int 
t2fs_first (t2fs_find *findStruct) {
    
    if (findStruct == NULL)
        return -1;
    
    unsigned char blockData [controlBlock.blockSize];
    
    unsigned int startBlock = controlBlock.ctrlSize + controlBlock.freeBlockSize;
    
    unsigned int qtdBlock = controlBlock.rootSize + startBlock;
    
    unsigned int qtdRec = (int) controlBlock.blockSize/controlBlock.fileEntrySize;

    unsigned int found = 0;
 
    int i;

    while ((startBlock < qtdBlock) && (!found)) {
        read_block (startBlock, (char*)blockData);

        i = 0;
        while ((i < qtdRec) && !(blockData[i*controlBlock.fileEntrySize] & 0x80)) i++;
        
        if (i < qtdRec)
            found = !found;
        else
            startBlock++;
    }
    
    if (!found) {
        findStruct->index = -1;
        return -1;
    }
    
    findStruct->index = i;
    findStruct->block = startBlock;
    return 0;     
}


int 
t2fs_next (t2fs_find *findStruct, t2fs_record *dirFile) {

    if ((findStruct == NULL) || (dirFile == NULL))
        return -1;
    
    if (findStruct->index == -1)
        return 1;    
        
    unsigned char blockData[controlBlock.blockSize];
    
    read_block(findStruct->block, (char*)blockData);    
    memcpy (dirFile, blockData + (findStruct->index*controlBlock.fileEntrySize), controlBlock.fileEntrySize);
    dirFile->name[0] &= 0x7F;
    
    unsigned int startBlock = findStruct->block;
    
    unsigned int qtdBlock = controlBlock.rootSize + controlBlock.ctrlSize + controlBlock.freeBlockSize;
    
    unsigned int qtdRec = (int) controlBlock.blockSize/controlBlock.fileEntrySize;

    unsigned int found = 0;
 
    int i = findStruct->index + 1;
    

    while ((startBlock < qtdBlock) && (!found)) {
        read_block (startBlock, (char*) blockData);
    
        while ((i < qtdRec) && !(blockData[i*controlBlock.fileEntrySize] & 0x80)) { 
            i++;
        }
        
        if (i < qtdRec)
            found = !found;
        else {
            i = 0;
            startBlock++;
        }
    }
    
    if (found) {
        findStruct->index = i;
        findStruct->block = startBlock;
    }
    else
        findStruct->index = -1;
    
    return 0;
}

/****************** TERMINADA - TESTAR MAIS ******************************/

int 
t2fs_read (t2fs_file handle, char *buffer, int size) {

    t2fs_handle* file = fileHandle (GET_HANDLE, handle);
    
    if (file == NULL)
        return -1;
    
    t2fs_record rec = file->rec;
    
    /*************************************************/
    /* Verificar se offset não é maior que o arquivo */
    /*************************************************/

    int result;
    if ((file->currentPointer + size) > rec.bytesFileSize) {
        size = rec.bytesFileSize - file->currentPointer;
        result = 0;
    }
    else {
        result = size;
    }
    
        
    // localiza o primeiro e ultimo bloco que deve ser lido
    int currentBlock = (int) file->currentPointer/controlBlock.blockSize;
    int lastBlock = (int) (size+file->currentPointer)/controlBlock.blockSize;
    
    int currentBytes = (int) file->currentPointer % controlBlock.blockSize;
    int firstBytes = controlBlock.blockSize - currentBytes;
    int lastBytes;
    
    if (firstBytes > size) {
        firstBytes = size;
        lastBytes = 0; 
    }
    else
        lastBytes = (int) (size + currentBytes) % controlBlock.blockSize;
    
    // calcula quantos indices cabem em cada bloco    
    unsigned int qtdIndice = (int) controlBlock.blockSize/4;
    
    // buffer de leitura temporária
    char blockData[controlBlock.blockSize];
    char blockData_2[controlBlock.blockSize];

    /* carrega o primeiro trecho dados em bloco quebrado */
    if (firstBytes > 0) {
        switch (currentBlock) {
            case 0: read_block (rec.dataPtr[0], blockData); break;
            case 1: read_block (rec.dataPtr[1], blockData); break;
            default:
                if (currentBlock < (qtdIndice + 2)) { // indireção simples
                    read_block (rec.singleIndPtr, blockData);
                    read_block (blockData[currentBlock-2], blockData);
                }
                else { // indireção dupla
                    read_block (rec.doubleIndPtr, blockData);
                    read_block (blockData[(int) (currentBlock-qtdIndice-2) / qtdIndice], blockData);
                    read_block (blockData[(int) (currentBlock-qtdIndice-2) % qtdIndice], blockData);
                }
        }
        
        memcpy (buffer, blockData + currentBytes, firstBytes);
        
        buffer += firstBytes;
        currentBlock++;
    }
    
    
    // Carrega blocos inteiros
    while (currentBlock < lastBlock) {
        if (currentBlock == 1) {
            read_block (rec.dataPtr[1], buffer);
            buffer += controlBlock.blockSize;
            currentBlock++;
        }
        else if (currentBlock < (qtdIndice+2)){
            read_block (rec.singleIndPtr, blockData);
            
            int i = 0;
            int end = (lastBlock < (qtdIndice+2)) ? lastBlock : (qtdIndice+2);
            
            while (currentBlock < end) {
                read_block (blockData[i++], buffer);
                buffer += controlBlock.blockSize;
                currentBlock++;                   
            }
        }
        else {
            read_block (rec.doubleIndPtr, blockData);

            int i = 0;
            
            while (currentBlock < lastBlock) {
            
                read_block (blockData[i++], blockData_2);
                
                int j = 0;
                int end = ((lastBlock - currentBlock) < qtdIndice) ? lastBlock : (qtdIndice + currentBlock);
                while (currentBlock < end) {
                    read_block (blockData_2[j++], buffer);
                    buffer += controlBlock.blockSize;
                    currentBlock++;
                }
            }        
        }
    }
    
    // Lêr ultimo bloco quebrado
    if (lastBytes > 0) {
        switch (currentBlock) {
            case 1: read_block (rec.dataPtr[1], blockData); break;
            default:
                if (currentBlock < (qtdIndice + 2)) { // indireção simples
                    read_block (rec.singleIndPtr, blockData);
                    read_block (blockData[currentBlock-2], blockData);
                }
                else { // indireção dupla
                    read_block (rec.doubleIndPtr, blockData);
                    read_block (blockData[(int) (currentBlock-qtdIndice-2) / qtdIndice], blockData);
                    read_block (blockData[(int) (currentBlock-qtdIndice-2) % qtdIndice], blockData);
                }
        }        
        memcpy (buffer, blockData, lastBytes);
    }

    file->currentPointer += size;
    return result;
}


