#include "cachesim.h"

#define NONE_EMPTY -1
#define NOT_USED 0

// ***** Limpiar direccion de memoria:
int getTag(cache_s *cache, int dir)
{
    int mask = bitMask(31 - cache->setBits - cache->indexBits, 0);
    int tag = (dir >> (cache->setBits + cache->indexBits)) & mask;
    return tag;
}

int getSet(cache_s *cache, int dir)
{
    int mask = bitMask(31 - cache->tagBits - cache->indexBits, 0);
    int shiftedDir = dir >> cache->indexBits;
    return shiftedDir & mask;
}

void splitAddress(cache_s *cache, int dir, actSum_s *sum)
{
    sum->setIndex = getSet(cache, dir);
    sum->tag = getTag(cache, dir);
}


// ------------- ACCIONES DE CACHE ------------- //

block_s *getLRU(cache_s *cache, actSum_s *sum, int way)
{
    return cache->sim[(sum->setIndex * cache->asoc) + way];
}

// Asegura: devolver un entero a la linea k del set que corresponda al bloque
// menos recientemente usado
int LRU(cache_s *cache,actSum_s *sum)
{
    uint32_t min;
    uint32_t minLastUsed;
    for (uint32_t i = 0; i < cache->asoc; i++)
    {
        block_s *blockActual = cache->sim[sum->setIndex * cache->asoc + i];
        if (i == 0){
            minLastUsed = blockActual->lastUsed;
            min =  i;
        } else if (blockActual->lastUsed < minLastUsed) {
            minLastUsed = blockActual->lastUsed;
            min =  i;
        }
    }
    return min;
}

block_s * blockInit(){
    block_s * block = malloc(sizeof(block_s));
    block->dirty = false;
    block->lastUsed = NOT_USED;
    block->valid = false;
    block->tag = -1;

    return block;
}

int getEmptyWay(cache_s *cache, actSum_s *sum){

    for (uint32_t i = 0; i < cache->asoc; i++)
    {
        block_s *blockActual = cache->sim[(sum->setIndex * cache->asoc) + i];
        if (!blockActual->valid) {
            sum->lastTag = -1; 
            return i;
        }
    }

    return NONE_EMPTY;

}

// Requiere: puntero a estructura de cache_s y struct de sumario de acciones
// Asegura: Devolver línea k dentro del set donde alojar el bloque que cumpla ser
// (a) la línea no válida de menor índice, (b la línea con menor valor de last-used
int getNewWay(cache_s *cache, actSum_s *sum){
    int newWay = getEmptyWay(cache, sum);

    if (newWay == NONE_EMPTY){
        newWay = LRU(cache, sum);
    }
    return newWay;
}

// Asegura: Si la linea a reemplazar era una linea con datos no validos, 
// el campo lastTag de sum sera -1
// Requiere: block es un puntero a un struct valido
// Asegura: guardar en sum si el bloque estuviera previamente sincronizado 
// con memoria principal o no
// Asegura: El bloque pasara a no estar en dirty, a ser valido y a alojar el tag asociado
// a los nuevos datos leidos
// Asegura: Preservar en sum el lastTag y lastUsed del bloque antes de ser desalojado
void setNewWay(block_s * block, actSum_s *sum){
    // guardar info del bloque antes de reemplazarla
    sum->lastTag = block->tag;
    if (sum->lastTag != -1) {
        sum->validBit = true;
    } else{
        sum->validBit = false;
    }
    sum->lastUsed = block->lastUsed;
    
    block->dirty = false;
    block->valid=true;
    block->tag=sum->tag;
}

// Asegura: Devolver un puntero a un bloque valido con los datos ya modificados
// por los asociados con la linea que esta siendo procesada
// Preservar los datos que se necesitan guardar de la linea a ser desalojada
// El campo caseId de sum guardara el identificador de operacion ya sea un 
// DirtyMiss (2b) o un Clean Miss (2a)
// Se sumaran los bytes del bloque a bytesWritten en el caso de un dirty miss
// Se sumara al contador de dirtyRmiss y dirtyWmiss segun sea el caso de esta operacion
block_s * manageMiss(cache_s *cache, line_s *line, actSum_s *sum){
    int newWay = getNewWay(cache, sum);
    block_s * bloque = getLRU(cache, sum, newWay);
    if (bloque == NULL){
        fprintf(stderr, "%s\n", "Error obteniendo nuevo bloque desde LRU");
        exit(EXIT_FAILURE);
    }

    if (bloque->dirty){ 
        // DIRTY MISS
        sum->caseId = "2b";
        
        cache->write->bytesWritten += cache->blockSize;

        switch (line->type)
        {
        case TypeRead:
            cache->read->dirtyRMiss++;
            break;
        case TypeWrite:
            cache->write->dirtWMiss++;
            break;
        default:
            break;
        } 
    } else { 
        // CLEAN MISS
        sum->caseId = "2a"; 
           
    }

    sum->dirtyBit = bloque->dirty;
    sum->line = newWay;
    setNewWay(bloque, sum);
    
    return bloque;
    
}

// Asegura: En el caso de no haber coincidencias de tag, devolver un puntero a NULL. 
// Caso contrario devolvera un puntero valido al bloque con el tag coincidente. 
// En este ultimo caso se registrara en sum el numero de linea del bloque, la validez
// de los datos, la informacion del bloque (tag y last Used).
block_s * checkTagInBlock(cache_s *cache, int dir, actSum_s *sum)
{
    // "Now that we have selected some set i in the previous step, the next step is to
    // determine if a copy of the word w is stored in one of the cache lines contained in
    // set i" (Computer Systems: A Programmer’s Perspective (3.ª ed., pag. 646))
    splitAddress(cache, dir, sum);

    for (uint32_t i = 0; i < cache->asoc; i++)
    {
        block_s *blockActual = cache->sim[(sum->setIndex * cache->asoc) + i];
        if (blockActual->valid && blockActual->tag == sum->tag)
        {
            sum->line = i;
            sum->validBit = true;
            sum->lastTag = blockActual->tag;
            sum->lastUsed = blockActual->lastUsed;
            return blockActual;
        }
    }

    return NULL;
}

void readCache(cache_s *cache, line_s *line, actSum_s *sum)
{
    cache->read->loads++;
    block_s * block = checkTagInBlock(cache, line->address, sum);
    if (block != NULL)
    {
        // HIT
        block->lastUsed = sum->opIndex;

        // dirty bit verboso
        sum->dirtyBit = block->dirty;
        return;
    }

    // MISS
    block = manageMiss(cache, line, sum);
    block->lastUsed = sum->opIndex;
    cache->read->bytesRead += cache->blockSize;
    cache->read->rmiss++;
    return;
}

void writeCache(cache_s *cache, line_s *line, actSum_s *sum)
{
    cache->write->stores++;
    block_s *block = checkTagInBlock(cache, line->address, sum);
    if (block != NULL)
    {
        // HIT
        sum->dirtyBit = block->dirty;
    } else {
        // MISS
        block = manageMiss(cache, line, sum);
        cache->read->bytesRead += cache->blockSize;
        cache->write->wmiss++;
    }

    block->lastUsed = sum->opIndex;
    // poner dirty bit en 1
    block->dirty = true;
}




// -------------INIT CACHE ------------------- //

cache_s *newCache(int asociatividad, int sets)
{
    cache_s *cache = malloc(sizeof(struct cachesim));
    cache->sim = malloc(sizeof(block_s) * asociatividad * sets);
    cache->asoc = asociatividad;

    // Llenamos el cache con los respectivos bloques para evitar que saltos
    // dependan de valores no inicializados
    for (int i=0; i < (asociatividad * sets); i++){
        cache->sim[i] = blockInit();
    }
    return cache;
}

reads_s *setReads()
{
    reads_s *reads = malloc(sizeof(reads_s));
    reads->bytesRead = 0;
    reads->dirtyRMiss = 0;
    reads->loads = 0;
    reads->rmiss = 0;
    return reads;
}

writes_s *setWrites()
{
    writes_s *writes = malloc(sizeof(writes_s));
    writes->bytesWritten = 0;
    writes->dirtWMiss = 0;
    writes->stores = 0;
    writes->wmiss = 0;
    return writes;
}

uint32_t calculateTag(int indexBits, int setsBits)
{
    return ADRESS_SIZE - indexBits - setsBits;
}

uint32_t calculateIndex(int tamanio, int asociatividad, int sets)
{
    return bitsEquivalent(getBlockSize(tamanio, asociatividad, sets));
}

// getBlockSize recibe el tamanho del cache, su asociatividad en int mayor o igual a cero 
// y la cantidad de sets y devuelve la cantidad de bytes por bloque en uint32
uint32_t getBlockSize(int tamanio, int asociatividad, int sets){
    return tamanio / (asociatividad * sets);
}

uint32_t calculateSets(int n)
{
    return bitsEquivalent(n);
}

void setOffset(cache_s *cachesim, int tamanio, int asociatividad, int sets)
{ 
    cachesim->indexBits = calculateIndex(tamanio, asociatividad, sets);
    cachesim->setBits = calculateSets(sets);
    cachesim->tagBits = calculateTag(cachesim->indexBits, cachesim->setBits);
}

cache_s *setCacheStructure(int tamanio, int asociatividad, int sets)
{
    cache_s *cache = newCache(asociatividad, sets);
    cache->blockSize = getBlockSize(tamanio, asociatividad, sets);
    setOffset(cache, tamanio, asociatividad, sets);

    cache->read = setReads();
    cache->write = setWrites();

    return cache;
}

void deleteCache(cache_s * cache, verboso_s * verboso, int asoc, int sets) {
    for (int i=0; i < (asoc * sets); i++){
        free(cache->sim[i]);
    }
    free(cache->sim);

    free(cache->read);
    free(cache->write); 
    free(cache);
    free(verboso);  
}
