#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>
#include <stdarg.h>

// arguments
#define MIN_ARGS 5
#define MAX_ARGS 8

#define ADRESS_SIZE 32 // bits
#define MAX_LINE_CHARS 38 // includes /n

#define OUTPUT_COPY "outputPrueba.txt"

typedef enum e_type {
	TypeWrite = 0,
	TypeRead = 1
} type_;

typedef struct actionSummary {
    char * caseId;
    int opIndex;
    int lastTag;
    int lastUsed;
    uint32_t setIndex;
    uint32_t tag;
    uint32_t line;
    uint8_t validBit;
    uint8_t dirtyBit;
} actSum_s;

// MODO VERBOSO ****************************************************************
typedef struct verboso{
    bool active;
    uint32_t  start;
    uint32_t  end;
} verboso_s;

verboso_s * setVerbose(int start, int end);
bool verbosoActive(verboso_s * verboso, int cntLine);
void printLineVerboso(actSum_s * sum, int asoc);

// CACHE INIT ******************************************************************
typedef struct reads{
    uint32_t loads;
    uint32_t rmiss;
    uint32_t dirtyRMiss;
    uint32_t bytesRead;
} reads_s;

typedef struct writes{
    uint32_t stores;
    uint32_t wmiss;
    uint32_t dirtWMiss;
    uint32_t bytesWritten;
} writes_s;

// Bloque/way en el cache:
typedef struct block{
    bool valid;
    bool dirty;
    uint32_t tag;
    uint32_t lastUsed;
} block_s;

typedef struct cachesim{
    uint32_t        indexBits;
    uint32_t        setBits;
    uint32_t        tagBits;
    uint32_t        blockSize;
    uint32_t        asoc;
    block_s**       sim;
    struct reads*   read;
    struct writes*  write;
} cache_s;

cache_s *newCache(int asociatividad, int sets);
reads_s *setReads();
writes_s *setWrites();
uint32_t getBlockSize(int tamanio, int asociatividad, int sets);
uint32_t calculateTag(int indexBits, int setsBits);
uint32_t calculateIndex(int tamanio, int asociatividad, int sets);
uint32_t calculateSets(int n);
void setOffset(cache_s *cachesim, int tamanio, int asociatividad, int sets);
cache_s *setCacheStructure(int tamanio, int asociatividad, int sets);

// TRAZA***********************************************************************
//Control: 
bool argsOK(int argc, char *argv[]);


// Interpretar:
typedef struct line {
    int type;
    unsigned long address;
    int bytes;
} line_s;

void readLines(FILE* traza, cache_s* cache, verboso_s * verboso);
line_s splitLine(char * line);

// LEER Y ESCRIBIR CACHE********************************************************
void readCache(cache_s *cache, line_s *line, actSum_s *sum);
void writeCache(cache_s *cache, line_s *line, actSum_s *sum);

block_s * blockInit();
block_s * checkTagInBlock(cache_s *cache, int dir, actSum_s *sum);
void splitAddress(cache_s *cache, int dir, actSum_s *sum);
int getSet(cache_s *cache, int dir);
int getTag(cache_s *cache, int dir);

// MISS ************************************************************************
block_s * manageMiss(cache_s *cache, line_s *line, actSum_s *sum);
void setNewWay(block_s * block, actSum_s *sum);
int getEmptyWay(cache_s *cache, actSum_s *sum);
// POLITICAS DE REEMPLAZO: least recently used
block_s *getLRU(cache_s *cache, actSum_s *sum, int way);
int LRU(cache_s *cache, actSum_s *sum);

// SUMMARY *********************************************************************

void printSummary(cache_s *cache, int size, int asoc,int sets);
int calculateTime(int dirtyMiss, int miss, int totals);
float calculateMissRate(cache_s *cache);

void deleteCache(cache_s * cache, verboso_s * verboso, int asoc, int sets);

// AUXILIARES GENERALES ********************************************************
bool esPotenciaDeDos(int n);
int bitsEquivalent(int n);
int bitMask(int highbit, int lowbit);
