#include "cachesim.h"


verboso_s * setVerbose(int start, int end){
    verboso_s * verboso = malloc(sizeof(struct verboso));
    verboso->active = false;
    verboso->start = (uint32_t)start;
    verboso->end = (uint32_t)end;
    return verboso;
}

void printLineVerboso(actSum_s * sum, int asoc){
    fprintf(stdout, "%i %s %x %x %i ", sum->opIndex, sum->caseId, sum->setIndex, 
            sum->tag, sum->line);
    if (sum->lastTag != -1) {
        fprintf(stdout, "%x ",sum->lastTag);
    } else {
        fprintf(stdout, "%i ",sum->lastTag);
    }
    fprintf(stdout, "%i %i",sum->validBit, sum->dirtyBit);
    if (asoc > 1){
        fprintf(stdout, " %i",sum->lastUsed);
    }
    fprintf(stdout,"\n");
}

bool verbosoActive(verboso_s * verboso, int cntLine){
    if (verboso->active){
        if (verboso->end == cntLine){
            verboso->active = false;
        }
        return true;
    }
    if (verboso->start == cntLine){
        verboso->active = true;
        return true;
    }
    return false;            
}