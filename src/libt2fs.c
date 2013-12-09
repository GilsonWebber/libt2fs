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


// variável com os dados do disco
t2fs_control controlBlock;

/**************************** FUNCIONANDO **************************/


int 
t2fs_write (t2fs_file handle, char *buffer, int size) {
	
	if (size > 0) {
        
        // Gerencia o bitmap com os blocos do disco
        int __free_block_manager__ (unsigned int block, eAction action);
        
        INIT ();
        
		unsigned int toWrite = size;
		
        typedef enum eAction {CHECK, UNCHECK, WRITE, GET_FREE};
        
        // testar erros
        t2fs_handle *file = fileHandle(GET_HANDLE, handle);
        
        unsigned int currentIndex = file->currentPointer % controlBlock.blockSize;
        unsigned int currentBlock = (int) file->currentPointer / controlBlock.blockSize;
        
        if (currentBlock == 0) { // primeiro ponteiro direto
        
        }
        
        if (currentBlock == 1) { //segundo ponteiro direto
        
        }
        
        if (curretnBlock > 1
        
        
        
        
        
        
        
        
        
        
        
        
        
        
		int // Gerencia o bitmap com os blocos do disco
		__free_block_manager__ (unsigned int block, eAction action) {
			static unsigned char blockData [controlBlock.blockSize];
			typedef enum {LOADED, UNLOADED} eState;
			
			static eState state = UNLOADED;
 			
            // Lê o bloco com o bitmap caso este já não esteja na memória
			if (state == UNLOADED) {
				read_block (controlBlock.ctrlSize, blockData);
                state = LOADED;
			}
            
            // Aplica ação desejada
            switch (action) {
                case CHECK: // marca um bloco como ocupado
                    blockData[(int) block/8] |= (1 >> (block%8));
                    break;
                case UNCHECK: // marca um bloco como livre
                    blockData[(int) block/8] &= ~(1 >> (block%8));
                    break;
                case WRITE: // grava o bitmap no disco
                    white_block (controlBlock.ctrlSize, blockData);
                    break;
                case GET_FREE: // procura pelo bloco livre mais próximo ao atual fornecido
                {
                    int low = (int) (ctrlSize + freeBlockSize + rootSize)/8;
                    int high = (int) diskSize/8;
                    
                    int i_high;
                    if (block < low)
                        i_high = low/8;
                    else
                        i_high = (int) block/8;
                        
                    int i_low = i_high - 1;
                    
                    while (1)                    
                        if (i_high < high) {
                            if (dataBlock[i_high] != 0xFF){
                                int index = 0;                               
                                while (((dataBlock[i_high] << index) & 0x80) == 0) index ++;
                                return (index + i_high*8);
                            }
                            i_high++;
                        }
                        
                        if ( i_low > low) {
                            if (dataBlock[i_low] != 0xFF){
                                int index = 0;                               
                                while (((dataBlock[i_low] >> index) & 0x01) == 0) index ++;
                                return (index + i_low*8);
                            }                        
                            i_low--;
                        }
                        
                        if (( i_low < low) && (i_high > high))
                            return -1;
                    }
                }
                default
                    return -1;
            }
                
		}
        
	
	
	}

	
	
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



int 
t2fs_delete (char *nome) {

    /* Localiza registro do arquivo */

    t2fs_find findStruct;
    
    if(!t2fs_first (&findStruct)) { // inicializa estrutura de procura de arquivos
    
        t2fs_record rec;

        int index;
        unsigned int block;
        
        index = findStruct.index;
        block = findStruct.block;

        while (!t2fs_next(&findStruct, &rec)) { // procura por registros válidos
            if (strncmp (nome,(char*) rec.name, MAX_NAME_SIZE) == 0) { // compara o nome para saber se é o mesmo
                /* executar ação de arquivo existe */                
                if (rec.dataPtr[0] != 0) {
                    
                    unsigned int startBlock = controlBlock.ctrlSize;
                    unsigned int endBlock = controlBlock.freeBlockSize + startBlock;                                        
                    
                    unsigned char freeBlockData[controlBlock.blockSize*controlBlock.freeBlockSize];
                    
                    int i = 0;
                    
                    /* Lê do disco o bloco com o mapa de bits de blocos */
                    while (startBlock < endBlock) {
                        read_block (startBlock, (char*) freeBlockData + (i*controlBlock.blockSize));
                        i++;
                        startBlock++;
                    }
                    
                    freeBlockData[rec.dataPtr[0] / 8] &= ~(0x80 >> (rec.dataPtr[1] % 8)); 
                    
                    if (rec.dataPtr[1] != 0) {
                        freeBlockData[rec.dataPtr[1] / 8] &= ~(0x80 >> (rec.dataPtr[1] % 8)); 
                        
                        if (rec.singleIndPtr != 0) {
                            unsigned int singIndPtrBlock [(controlBlock.blockSize/4) + 1];
                            read_block (rec.singleIndPtr, (char*) singIndPtrBlock);

                            singIndPtrBlock[(controlBlock.blockSize/4)] = 0;
                            
                            i = 0;                                                        
                            while (singIndPtrBlock[i] != 0) {
                                freeBlockData[singIndPtrBlock[i] / 8] &= ~(0x80 >> (singIndPtrBlock[i] % 8));
                                i++;                           
                            }
                            
                            if (rec.doubleIndPtr != 0) {
                                read_block (rec.doubleIndPtr, (char*) singIndPtrBlock);
                                
                                unsigned int doubleIndPtrBlock [(controlBlock.blockSize/4) + 1];
                                doubleIndPtrBlock[(controlBlock.blockSize/4)] = 0;
                                
                                i = 0;
                                int j;
                                while (singIndPtrBlock[i] != 0) {
                                    read_block (rec.doubleIndPtr, (char*) doubleIndPtrBlock);
                                    
                                    j = 0;
                                    while (singIndPtrBlock[j] != 0) {
                                        freeBlockData[singIndPtrBlock[j] / 8] &= ~(0x80 >> (singIndPtrBlock[j] % 8));
                                        j++;                           
                                    }
                                    i++;                               
                                }
                            }
                        }                        
                    }
                    // Gravar bloco com bitmap de livres
                    startBlock = controlBlock.ctrlSize;
                    
                    i = 0;
                    while (startBlock < endBlock) {
                        write_block (startBlock, (char*) freeBlockData + (i*controlBlock.blockSize));
                        i++;
                        startBlock++;
                    }
                    
                    
                                    
                }
                
                // Altera nome da entrada e grava no disco
                unsigned char dirBlock[controlBlock.blockSize];
                read_block (block, (char*) dirBlock);
                memcpy (dirBlock + (index*controlBlock.fileEntrySize), &rec, controlBlock.fileEntrySize);
                write_block (block, (char*) dirBlock); 
                return 0;
            }
            else {
                /* Salva os dados do próximo indice válido que será lido */
                index = findStruct.index;
                block = findStruct.block;
            }
        }
    }
     return -1;
}





