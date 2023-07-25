#include "cachesim.h"

#define PENALTY 100
// offset array argumentos
#define TRAZA_FILE 1
#define CACHE_SIZE 2        // en bytes
#define ASSOCIATIVITY 3     // lineas por set
#define SETS_NUM 4
#define VERBOSE_START 6
#define VERBOSE_END 7

// ------------- CONTROL DE ARGUMENTOS ------------------- //
bool argsOK(int argc, char *argv[]){

    // el número de argumentos no es 4 o 
    if (argc != MIN_ARGS && argc != MAX_ARGS) {
        fprintf(stderr,"%s\n", "El número de argumentos no es 4 o 7");
        return false;
    } 

    int cacheSize = atoi(argv[CACHE_SIZE]);
    int cacheAsoc = atoi(argv[ASSOCIATIVITY]);
    int cacheSets = atoi(argv[SETS_NUM]);
    
    // alguno de los parámetros C, E o S no son potencia de dos
    if (!esPotenciaDeDos(cacheSize) || !esPotenciaDeDos(cacheAsoc) || !esPotenciaDeDos(cacheSets)){
        fprintf(stderr,"%s\n", "Algun argumento no es potencia de 2");
        return false;
    }

    // alguna combinación de parámetros de C, E y S es inválida
    if (cacheSize < (cacheAsoc * cacheSets)){
        fprintf(stderr,"%s\n", "Combinacion de parametros C, E y S invalida");
        return false;
    }

    // args modo verboso
    if (argc == MAX_ARGS) {

        int n = atoi(argv[VERBOSE_START]);
        int m = atoi(argv[VERBOSE_END]);
        if (n < 0|| m < 0 || n > m){
            fprintf(stderr,"%s\n", "Los argumentos del modo verboso no cumplen 0 ≤ n ≤ m");
            return false;
        }
    }

    return true;
}



// ------------- LEER LINEAS ----------------------------- //

line_s splitLine(char * line){
    line_s actLine;
    int cnt = 0;
    char * pch;
    pch = strtok (line,": ");
    while (pch != NULL)
    {
        switch (cnt) {
            case 1: 
                if (strcmp(pch, "W") == 0){
                    actLine.type = TypeWrite;
                    break;
                } 
                if (strcmp(pch, "R") == 0){
                    actLine.type = TypeRead;
                    break;
                }
            // https://en.cppreference.com/w/cpp/string/byte/strtoul
            case 2: actLine.address = strtoul(pch, NULL, 16); break;
            case 3: actLine.bytes = atoi(pch); break;
            default: break;
        }
        pch = strtok (NULL, ": ");
        cnt++;
    }

    return actLine;
}

void setActionSum(actSum_s* actionSum, int numLine){
    actionSum->caseId = "1";
    actionSum->opIndex=numLine;
    actionSum->lastTag=0;
    actionSum->lastUsed=0;
    actionSum->setIndex=0;
    actionSum->tag=0;
    actionSum->line=0;
    actionSum->dirtyBit=0;
    actionSum->validBit=0;

}

void readLines(FILE* traza, cache_s* cache, verboso_s * verboso){
    char* line = NULL;
    size_t bytesLinea = 0;
    int cntLine = 0;
    actSum_s * actionSum = malloc(sizeof(struct actionSummary));
    
    while(getline(&line, &bytesLinea, traza) != -1){ 
        
        line_s actual = splitLine(line);
        setActionSum(actionSum, cntLine);
        
        // acciones de cache
        switch (actual.type){
            case TypeWrite: writeCache(cache, &actual, actionSum); break;
            case TypeRead: readCache(cache, &actual, actionSum); break;
            default: cntLine--; break;
        }

        //verboso
        if (verboso != NULL && verbosoActive(verboso, cntLine)){
            printLineVerboso(actionSum, cache->asoc);
        }
        
        cntLine++;
    }
    free(line);
    free(actionSum);

}

// ------------- PRINTS ---------------------------------- //
// ------------- Summary:

int calculateTime(int dirtyMiss, int miss, int totals){
    int t_dirtyMiss = dirtyMiss * (2 * PENALTY);
    int t_cleanMiss = (miss -dirtyMiss) * PENALTY;
    int readTime = t_cleanMiss + t_dirtyMiss + totals;
}

float calculateMissRate(cache_s *cache){
    return ((double)(cache->read->rmiss + cache->write->wmiss)) / ((double)(cache->write->stores + cache->read->loads));
}

void printSummary(cache_s *cache, int size, int asoc,int sets){
 
    int readTime = calculateTime(cache->read->dirtyRMiss,cache->read->rmiss, cache->read->loads);
    int writeTime = calculateTime(cache->write->dirtWMiss, cache->write->wmiss, cache->write->stores);

    if (asoc == 1){
        fprintf(stdout, "direct-mapped, ");
    } else {
        fprintf(stdout, "%i-way, ",asoc);
    }
    fprintf(stdout, "%i sets, size = %iKB\n", sets, size/1024);
    fprintf(stdout, "loads %i stores %i total %i\n", cache->read->loads, cache->write->stores, cache->read->loads + cache->write->stores);
    fprintf(stdout, "rmiss %i wmiss %i total %i\n", cache->read->rmiss, cache->write->wmiss, cache->read->rmiss + cache->write->wmiss);
    fprintf(stdout, "dirty rmiss %i dirty wmiss %i\n", cache->read->dirtyRMiss, cache->write->dirtWMiss);
    fprintf(stdout, "bytes read %i bytes written %i\n", cache->read->bytesRead, cache->write->bytesWritten);
    fprintf(stdout, "read time %i write time %i\n", readTime, writeTime);
    fprintf(stdout, "miss rate %f\n", calculateMissRate(cache));

}   

int main (int argc, char *argv[]){ 
    
    // check de argumentos de tamanio, asociatividad y sets del cache pedidos
    if (!argsOK(argc,argv)){
        return EXIT_FAILURE;
    }
    
    // check de archivo de trazas a analizar
    char * fileName = argv[TRAZA_FILE];
    FILE* traza = fopen(fileName, "r");
    // archivo de trazas especificado no existe
    if (traza == NULL) {
        fprintf(stderr,"%s\n", "Archivo de traza no existe");
        exit(EXIT_FAILURE);
	}

    // parse args
    int cacheSize = atoi(argv[CACHE_SIZE]);
    int cacheAsoc = atoi(argv[ASSOCIATIVITY]);
    int cacheSets = atoi(argv[SETS_NUM]);
    // si existe creamos la estructura del cache
    cache_s * cache = setCacheStructure(cacheSize, cacheAsoc, cacheSets);

    // mantendremos lo relacionado al modo verboso aparte para tener un cache independiente
    // sea que se elimine esta funcion o se agreguen otras
    verboso_s * verboso;
    if (argc == MAX_ARGS){
        verboso = setVerbose(atoi(argv[VERBOSE_START]), atoi(argv[VERBOSE_END]));
    } else {
        verboso = NULL;
    }

    // empezamos a leer
    readLines(traza, cache, verboso);
    printSummary(cache, cacheSize, cacheAsoc, cacheSets);

    // cierre del programa
    fclose(traza);
    deleteCache(cache, verboso,cacheAsoc, cacheSets);
    return 0;
}


