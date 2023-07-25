#include "lib.h"

funcCmp_t *getCompareFunction(type_t t)
{
    switch (t)
    {
    case TypeInt:
        return (funcCmp_t *)&intCmp;
        break;
    case TypeString:
        return (funcCmp_t *)&strCmp;
        break;
    case TypeCard:
        return (funcCmp_t *)&cardCmp;
        break;
    default:
        break;
    }
    return 0;
}
funcClone_t *getCloneFunction(type_t t)
{
    switch (t)
    {
    case TypeInt:
        return (funcClone_t *)&intClone;
        break;
    case TypeString:
        return (funcClone_t *)&strClone;
        break;
    case TypeCard:
        return (funcClone_t *)&cardClone;
        break;
    default:
        break;
    }
    return 0;
}
funcDelete_t *getDeleteFunction(type_t t)
{
    switch (t)
    {
    case TypeInt:
        return (funcDelete_t *)&intDelete;
        break;
    case TypeString:
        return (funcDelete_t *)&strDelete;
        break;
    case TypeCard:
        return (funcDelete_t *)&cardDelete;
        break;
    default:
        break;
    }
    return 0;
}
funcPrint_t *getPrintFunction(type_t t)
{
    switch (t)
    {
    case TypeInt:
        return (funcPrint_t *)&intPrint;
        break;
    case TypeString:
        return (funcPrint_t *)&strPrint;
        break;
    case TypeCard:
        return (funcPrint_t *)&cardPrint;
        break;
    default:
        break;
    }
    return 0;
}

/** Int **/

int32_t intCmp(int32_t *a, int32_t *b)
{
    if (*a < *b)
    {
        return 1;
    }

    if (*a > *b)
    {
        return -1;
    }

    return 0;
}

void intDelete(int32_t *a)
{
    free(a);
}

// doc streams = https://www.gnu.org/software/libc/manual/html_node/Descriptors-and-Streams.html#Descriptors-and-Streams
void intPrint(int32_t *a, FILE *pFile)
{
    fprintf(pFile, "%i", *a);
}

/** String **/

char *strClone(char *a)
{

    // Calcular la longitud de la cadena original
    uint32_t len = strLen(a);

    // Reservar memoria para la copia
    char *copy = malloc((len + 1) * sizeof(char));

    // Copiar los caracteres uno por uno
    for (size_t i = 0; i <= len; i++)
    {
        copy[i] = a[i];
    }

    return copy;
}

void strPrint(char *a, FILE *pFile)
{
    if (a != NULL)
    {
        if (strlen(a) > 0)
        {
            fprintf(pFile, "%s", a);
        }
        else
        {
            fprintf(pFile, "NULL");
        }
    }
    else
    {
        fprintf(pFile, "NULL");
    }
}

/** Card **/

card_t *cardNew(char *suit, int32_t *number)
{
    card_t *newCard = (card_t *)malloc(sizeof(card_t));
    list_t *newStacked = (list_t *)listNew(TypeCard);

    newCard->number = intClone(number);
    newCard->suit = strClone(suit);
    newCard->stacked = newStacked;
    return newCard;
}

char *cardGetSuit(card_t *c)
{
    return c->suit;
}

int32_t *cardGetNumber(card_t *c)
{
    return c->number;
}

list_t *cardGetStacked(card_t *c)
{
    return c->stacked;
}

void cardPrint(card_t *c, FILE *pFile)
{
    if (c == NULL || pFile == NULL)
    {
        return;
    }

    fprintf(pFile, "{%s-%i", cardGetSuit(c), *cardGetNumber(c));
    if (listGetSize(cardGetStacked(c)) != 0)
    {
        fprintf(pFile, "-");
        listPrint(cardGetStacked(c), pFile);
    }
    else
    {
        fprintf(pFile, "-[]");
    }
    fprintf(pFile, "}");
}

/** Array **/

array_t *arrayNew(type_t t, uint8_t capacity)
{
    array_t *newArray = malloc(sizeof(array_t));

    newArray->type = t;
    newArray->size = 0;
    newArray->capacity = capacity;
    newArray->data = malloc(capacity * sizeof(void *));
    return newArray;
}

/*
 * NOTA -- pointers to void:
 * memset(&a[++j], 0x0, sizeof(a[++j]));
 * ((void*)0);
 * NULL;
 * Averiguar bien porque no deberian ser equivalentes en todas las arquitecturas
 *
 * */

void replaceArrayWithoutIElement(array_t *a, uint8_t i)
{
    uint8_t counter = i;

    while (counter < arrayGetSize(a) - 1)
    {
        // copy all the values after the position eliminated moving them forward one index
        a->data[counter] = a->data[counter + 1];
        ++counter;
    }

    // dynamic memory management https://en.cppreference.com/w/c/memory
    // doc alloc: https://en.cppreference.com/w/c/memory/realloc
    // a= (array_t *) realloc (a, sizeof(substituteArray));
}

void *arrayRemove(array_t *a, uint8_t i)
{

    if (arrayGetSize(a) <= i)
    {
        return 0;
    }
    void *elemDeleted = a->data[i];
    replaceArrayWithoutIElement(a, i);
    a->size = arrayGetSize(a) - 1;
    return elemDeleted;
}

void arraySwap(array_t *a, uint8_t i, uint8_t j)
{
    size_t sizeArray = a->size;
    if (i < sizeArray && j < sizeArray)
    {
        void *aux = a->data[i];
        a->data[i] = a->data[j];
        a->data[j] = aux;
    }
}

void arrayDelete(array_t *a)
{
    for (size_t i = 0; i < a->size; i++)
    {
        switch (a->type)
        {
        case TypeInt:
            intDelete(a->data[i]);
            break;
        case TypeString:
            strDelete(a->data[i]);
            break;
        case TypeCard:
            cardDelete(a->data[i]);
            break;
        default:
            break;
        }
    }
    free(a->data);
    free(a);
}

void arrayPrint(array_t *a, FILE *pFile)
{
    if (a == NULL || pFile == NULL)
    {
        return;
    }

    fprintf(pFile, "%s", "[");

    for (uint8_t i = 0; i < a->size; i++)
    {
        void *data = a->data[i];
        // hago un switch segun el tipo de dato
        switch (a->type)
        {
        case TypeInt:
            intPrint(data, pFile);
            // fprintf(pFile, "%d", *(int*)data);
            break;
        case TypeString:
            strPrint(data, pFile);
            // fprintf(pFile, "%s", (char*)data);
            break;
        case TypeCard:
            cardPrint(data, pFile);
            break;
        default:
            fprintf(pFile, "Type error");
            break;
        }
        if (i < a->size - 1)
        {
            fprintf(pFile, ",");
        }
    }
    fprintf(pFile, "%s", "]");
}

/** Lista **/
bool isEmpty(list_t *l)
{
    return listGetSize(l) == 0;
}

struct s_listElem *createNewNode(void *data, list_t *l)
{

    struct s_listElem *node = malloc(sizeof(struct s_listElem));
    switch (l->type)
    {
    case TypeCard:
        node->data = cardClone(data);
        break;
    case TypeString:
        node->data = strClone(data);
        break;
    case TypeInt:
        node->data = intClone(data);
        break;
    default:
        node->data = NULL;
        break;
    }
    return node;
}

void listAddLast(list_t *l, void *data)
{

    struct s_listElem *newNode = createNewNode(data, l);
    if (isEmpty(l))
    {
        l->first = newNode;
    }
    else
    {
        l->last->next = newNode;
    }
    newNode->prev = l->last;
    newNode->next = NULL;
    l->last = newNode;
    l->size = listGetSize(l) + 1;
}

void *iterateTillN(list_t *l, uint8_t n)
{
    struct s_listElem *element = l->first;

    for (uint8_t i = 0; i < n; i++)
    {
        element = element->next;
    }

    return element;
}

void *listRemove(list_t *l, uint8_t i)
{
    if (i >= listGetSize(l))
    {
        return 0;
    }

    struct s_listElem *act;
    void *eliminado;
    if (listGetSize(l) == 1)
    {
        act = l->first;
        eliminado = act->data;
        l->first = NULL;
        l->last = NULL;
    }
    else
    {
        act = iterateTillN(l, i);
        eliminado = act->data;

        if (act == l->last)
        {
            l->last = act->prev;
            act->prev->next = act->next;
        }
        else if (act == l->first)
        {
            l->first = act->next;
            act->next->prev = act->prev;
        }
        else
        {
            act->prev->next = act->next;
            act->next->prev = act->prev;
        }
    }

    l->size = listGetSize(l) - 1;
    free(act);
    return eliminado;
}

void listSwap(list_t *l, uint8_t i, uint8_t j)
{
    // check if i and j are valid indexes
    if (i <= listGetSize(l) && j <= listGetSize(l))
    {
        // get pointers for j and i
        struct s_listElem *elem_i = iterateTillN(l, i);
        struct s_listElem *elem_j = iterateTillN(l, j);

        // swap data
        void *temp = elem_i->data;
        elem_i->data = elem_j->data;
        elem_j->data = temp;
    }
}

list_t *listClone(list_t *l)
{

    list_t *copy = listNew(l->type);
    if (listGetSize(l) == 0)
    {
        return copy;
    }

    struct s_listElem *elem = l->first;
    for (uint8_t i = 0; i < listGetSize(l); i++)
    {
        listAddLast(copy, elem->data);
        elem = elem->next;
    }
    return copy;
}

void listDelete(list_t *l)
{

    struct s_listElem *elem = l->first;
    for (uint8_t i = 0; i < listGetSize(l); i++)
    {
        struct s_listElem *act = elem;
        elem = elem->next;

        switch (l->type)
        {
        case TypeCard:
            cardDelete(act->data);
            break;
        case TypeString:
            strDelete(act->data);
            break;
        case TypeInt:
            intDelete(act->data);
            break;
        default:
            return;
        }

        free(act);
    }
    free(l);
}

void listPrint(list_t *l, FILE *pFile)
{
    if (l == NULL || pFile == NULL)
    {
        return;
    }

    fprintf(pFile, "%s", "[");

    listElem_t *actual = l->first;
    for (uint8_t i = 0; i < listGetSize(l); i++)
    {
        void *data = actual->data;
        actual = actual->next;
        switch (l->type)
        {
        case TypeInt:
            intPrint(data, pFile);
            // fprintf(pFile, "%d", *(int*)data);
            break;
        case TypeString:
            strPrint(data, pFile);
            // fprintf(pFile, "%s", (char*)data);
            break;
        case TypeCard:
            cardPrint(data, pFile);
            break;
        default:
            fprintf(pFile, "Type error");
            break;
        }

        if (i < listGetSize(l) - 1)
        {
            fprintf(pFile, "%s", ",");
        }
    }
    fprintf(pFile, "%s", "]");
}

/** Game **/

game_t *gameNew(void *cardDeck, funcGet_t *funcGet, funcRemove_t *funcRemove, funcSize_t *funcSize, funcPrint_t *funcPrint, funcDelete_t *funcDelete)
{
    game_t *game = (game_t *)malloc(sizeof(game_t));
    game->cardDeck = cardDeck;
    game->funcGet = funcGet;
    game->funcRemove = funcRemove;
    game->funcSize = funcSize;
    game->funcPrint = funcPrint;
    game->funcDelete = funcDelete;
    return game;
}
int gamePlayStep(game_t *g)
{
    int applied = 0;
    uint8_t i = 0;
    while (applied == 0 && i + 2 < g->funcSize(g->cardDeck))
    {
        card_t *a = g->funcGet(g->cardDeck, i);
        card_t *b = g->funcGet(g->cardDeck, i + 1);
        card_t *c = g->funcGet(g->cardDeck, i + 2);
        if (strCmp(cardGetSuit(a), cardGetSuit(c)) == 0 || intCmp(cardGetNumber(a), cardGetNumber(c)) == 0)
        {
            card_t *removed = g->funcRemove(g->cardDeck, i);
            cardAddStacked(b, removed);
            cardDelete(removed);
            applied = 1;
        }
        i++;
    }
    return applied;
}
uint8_t gameGetCardDeckSize(game_t *g)
{
    return g->funcSize(g->cardDeck);
}
void gameDelete(game_t *g)
{
    g->funcDelete(g->cardDeck);
    free(g);
}
void gamePrint(game_t *g, FILE *pFile)
{
    g->funcPrint(g->cardDeck, pFile);
}
