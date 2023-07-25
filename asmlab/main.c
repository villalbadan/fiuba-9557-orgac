#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "lib.h"

#define MAX_LINE_LENGTH 100
#define SMALL_TEST 10
#define SHORT_TEST_SIZE 5

// ************* PRUEBAS CORTAS ************************************ //
// ------------- Caso ARRAY >>>>>

void fillArrayDeck(array_t* deck, uint8_t deckSize) {
    char * str[5] = {"Basto", "Espada","Copa", "Oro", "Basto"};
    card_t * testCard;
    for (size_t i = 0; i < SHORT_TEST_SIZE; i++) {
        int32_t num = i+1;
        testCard = cardNew(str[i], &num); arrayAddLast(deck, testCard); cardDelete(testCard);
    }
}

int shortTestArray(){
    // Crear un mazo con 5 cartas sobre un arreglo
    array_t * deck = arrayNew(TypeCard, SHORT_TEST_SIZE);
    fillArrayDeck(deck, SHORT_TEST_SIZE);

    // Imprimir el mazo.
    FILE *pf = fopen("shortArray.txt", "a");
    arrayPrint(deck, pf);

    // Apilar una carta cualquiera del mazo sobre otra carta cualquiera.
    card_t* card2 = arrayGet(deck, 2);
    card_t* card0 = arrayGet(deck, 0);
    cardAddStacked(card2, card0);

    // Imprimir nuevamente el mazo.
    arrayPrint(deck, pf);
    fclose(pf);

    // Borrar el mazo.
    arrayDelete(deck);
    return 0;
}

// ------------- Caso LIST >>>>>>
// Hacemos pruebas aniadiendo cartas con listAddFirst y listAddLast


void fillListFromFirst(list_t* deck, uint8_t deckSize) {
    char * str[5] = {"Basto", "Espada","Copa", "Oro", "Basto"};
    card_t * testCard;
    for (size_t i = 0; i < SHORT_TEST_SIZE; i++) {
        int32_t num = i+1;
        testCard = cardNew(str[i], &num); listAddFirst(deck, testCard); cardDelete(testCard);
    }
}

void fillListFromLast(list_t* deck, uint8_t deckSize) {
    char * str[5] = {"Basto", "Espada","Copa", "Oro", "Basto"};
    card_t * testCard;
    for (size_t i = 0; i < SHORT_TEST_SIZE; i++) {
        int32_t num = i+1;
        testCard = cardNew(str[i], &num); listAddLast(deck, testCard); cardDelete(testCard);
    }
}

int shortTestList(){
    // Crear un mazo con 5 cartas sobre una lista
    list_t * deckLast = listNew(TypeCard);
    list_t * deckFirst = listNew(TypeCard);
    fillListFromLast(deckLast, SHORT_TEST_SIZE);
    fillListFromFirst(deckFirst, SHORT_TEST_SIZE);

    // Imprimir el mazo.
    FILE *pf = fopen("shortTestList.txt", "a");
    listPrint(deckLast, pf);
    listPrint(deckFirst, pf);

    // Apilar una carta cualquiera del mazo sobre otra carta cualquiera.
    card_t* card2 = listGet(deckFirst, 2);
    card_t* card0 = listGet(deckFirst, 0);
    cardAddStacked(card2, card0);

    card_t* card1 = listGet(deckFirst, 1);
    card_t* card3 = listGet(deckFirst, 3);
    cardAddStacked(card1, card3);

    // Imprimir nuevamente el mazo.
    listPrint(deckLast, pf);
    listPrint(deckFirst, pf);
    fclose(pf);

    // Borrar el mazo.
    listDelete(deckFirst);
    listDelete(deckLast);
    return 0;
}



int main (void){

    // Pruebas cortas obligatorias:
    shortTestArray();
    shortTestList();
    return 0;
}


