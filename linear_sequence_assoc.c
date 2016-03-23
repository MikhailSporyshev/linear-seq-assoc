
#include <stdlib.h>
#include <stdio.h>
#include "linear_sequence_assoc.h"

#define general_node 0
#define before_first_node 1
#define past_rear_node 2
#define root_node 3

typedef int LSQ_DifferenceT;
typedef int LSQ_NodeT;

typedef struct node {
	struct node* parent;
	struct node* left;
	struct node* right;
	LSQ_BaseTypeT data;
	LSQ_IntegerIndexT key;
	LSQ_DifferenceT diff;
	LSQ_NodeT type;
}node;

typedef struct iter {
	node* parent;
	node* current;
}iter;

typedef struct tree {
	node* beforeFirst;
	node* pastRear;
	node* root;
	LSQ_IntegerIndexT size;
}tree;

/*declaration of methods*/
static node* GrandParent(node* A);
static node* Uncle(node* A);
static node* Sibling(node* A);
 
static void TurnLeft(node* A, tree* treePtr);
static void TurnRight(node* A, tree* treePtr);

static node* SmallTurnLeft(node* A, tree* treePtr);
static node* BigTurnLeft(node* A, tree* treePtr);
static node* SmallTurnRight(node* A, tree* treePtr);
static node* BigTurnRight(node* A, tree* treePtr);
 
static LSQ_IteratorT PositioningByIndex(LSQ_HandleT handle, LSQ_IntegerIndexT index);

extern void GoDownLeft(iter* iterPtr);
extern void GoUpLeft(iter* iterPtr);
extern void GoDownRight(iter* iterPtr);
extern void GoUpRight(iter* iterPtr);
 
extern void DisplaySequence(LSQ_HandleT handle);
static void InOrderDisplay(iter* iterPtr);

static void InOrderDestroy(iter* iterPtr);

static void InsertTreeRetrieval(node* N, tree* treePtr);

static void DeleteTreeRetrieval(node* N, tree* treePtr);

/*realization of methods*/
static node* GrandParent(node* A) {
    return A->parent->parent;
}
 
static node* Uncle(node* A) {
    node* P = A->parent;
    if(P == P->parent->right) {
        return P->parent->left;
    }
    if(P == P->parent->left) {
        return P->parent->right;
    }
}
 
static node* Sibling(node* A) {
    if(A == A->parent->right) {
        return A->parent->left;
    }
    else {
        return A->parent->right;
    }  
}
 
static void TurnRight(node* A, tree* treePtr) {
    node* B = A->left;

    B->parent = A->parent;
    if(A->parent == NULL) {
        B->type = root_node;
        treePtr->root = B;
        A->type = general_node;
    }
    else {
        if(A == A->parent->right) {
            A->parent->right = B;
        }
        else {
            A->parent->left = B;
        }
    }

    A->left = B->right;
    if(B->right != NULL) {
        B->right->parent = A;
    }
 
    A->parent = B;
    B->right = A;
}
 
static void TurnLeft(node* A, tree* treePtr) {
    node* B = A->right;
    node* C = A->left;
 
    B->parent = A->parent;
    if(A->parent == NULL) {
        B->type = root_node;
        treePtr->root = B;
        A->type = general_node;
    }
    else {
        if(A == A->parent->right) {
            A->parent->right = B;
        }
        else {
            A->parent->left = B;
        }
    }

    A->right = B->left;
    if(B->left != NULL) {
        B->left->parent = A;
    }
 
    A->parent = B;
    B->left = A;
}


static node* SmallTurnLeft(node* N, tree* treePtr) {
    node* R = N->right;
    node* L = N->left;
    if(R->diff == -1) {
        TurnLeft(N, treePtr);
        N->diff = 0;
        R->diff = 0;
    }
    else if(R->diff == 0) {
        TurnLeft(N, treePtr);
        N->diff = -1;
        R->diff = 1;
    }
    N = R;
    return N;
}

static node* BigTurnLeft(node* N, tree* treePtr) {
    node* R = N->right;
    node* L = N->left;
    node* C = R->left;
    TurnRight(R, treePtr);
    TurnLeft(N, treePtr);
    if(C->diff == 1) {
        N->diff = 0;
        R->diff = -1;
        C->diff = 0;
    }
    else if(C->diff == -1) {
        N->diff = 1;
        R->diff = 0;
        C->diff = 0;
    }
    else if(C->diff == 0) {
        N->diff = 0;
        R->diff = 0;
        C->diff = 0;
    }

    N = C; 
    return N;
}

static node* SmallTurnRight(node* N, tree* treePtr) {
    node* R = N->right;
    node* L = N->left;
    if(L->diff == 1) {
        TurnRight(N, treePtr);
        N->diff = 0;
        L->diff = 0;
    }
    else if(L->diff == 0) {
        TurnRight(N, treePtr);
        N->diff = 1;
        L->diff = -1;
    }
    N = L;
    return N;
}

static node* BigTurnRight(node* N, tree* treePtr) {
    node* R = N->right;
    node* L = N->left;
    node* C = L->right;
    TurnLeft(L, treePtr);
    TurnRight(N, treePtr);
    if(C->diff == -1) {
        N->diff = 0;
        L->diff = 1;
        C->diff = 0;
    }
    else if(C->diff == 1) {
        N->diff = -1;
        L->diff = 0;
        C->diff = 0;
    }
    else if(C->diff == 0) {
        N->diff = 0;
        L->diff = 0;
        C->diff = 0;
    }

    N = C; 
    return N;
}

static LSQ_IteratorT PositioningByIndex(LSQ_HandleT handle, LSQ_IntegerIndexT index) {
    tree* treePtr = (tree*)handle;
    iter* iterPtr = malloc(sizeof(iter));
    iterPtr->current = treePtr->root;
    iterPtr->parent = NULL;
    while((LSQ_IsIteratorDereferencable(iterPtr)) && (iterPtr->current->key != index)) {
        iterPtr->parent = iterPtr->current;
        if(index > iterPtr->current->key) {
            iterPtr->current = iterPtr->current->right;
        }
        else {
            iterPtr->current = iterPtr->current->left;
        }
    }
    return iterPtr;
} 



/* Функция, создающая пустой контейнер. Возвращает назначенный ему дескриптор */
extern LSQ_HandleT LSQ_CreateSequence(void) {
    tree* treePtr = malloc(sizeof(tree));
    treePtr->beforeFirst = malloc(sizeof(node));
    treePtr->pastRear = malloc(sizeof(node));
    treePtr->root = NULL;
    treePtr->size = 0;
 
    treePtr->beforeFirst->right = NULL;
    treePtr->beforeFirst->left = NULL;
    treePtr->beforeFirst->parent = NULL;
    treePtr->beforeFirst->type = before_first_node;
    treePtr->beforeFirst->diff = 0;
 
    treePtr->pastRear->left = NULL;
    treePtr->pastRear->right = NULL;
    treePtr->pastRear->parent = NULL;
    treePtr->pastRear->type = past_rear_node;
    treePtr->pastRear->diff = 0;
 
    return treePtr;
}
/* Функция, уничтожающая контейнер с заданным дескриптором. Освобождает принадлежащую ему память */
extern void LSQ_DestroySequence(LSQ_HandleT handle) {
    if(handle == NULL) {
        return ;
    }
    tree* treePtr = (tree*)handle;
    iter* iterPtr = malloc(sizeof(iter));
    iterPtr->current = treePtr->root;
    iterPtr->parent = NULL;
    InOrderDestroy(iterPtr);
    free(iterPtr);
    free(treePtr->beforeFirst);
    free(treePtr->pastRear);
    free(treePtr);
}

static void InOrderDestroy(iter* iterPtr) {
    if(LSQ_IsIteratorDereferencable(iterPtr)) {
        iterPtr->parent = iterPtr->current;
        iterPtr->current = iterPtr->current->left;
       
        InOrderDestroy(iterPtr);
       
        iterPtr->current = iterPtr->parent;
        iterPtr->parent = iterPtr->parent->parent;
 
        iterPtr->parent = iterPtr->current;
        iterPtr->current = iterPtr->current->right;
       
        InOrderDestroy(iterPtr);
 
        iterPtr->current = iterPtr->parent;
        iterPtr->parent = iterPtr->parent->parent;
        free(iterPtr->current);
    }
}
/* Функция, возвращающая текущее количество элементов в контейнере */
extern LSQ_IntegerIndexT LSQ_GetSize(LSQ_HandleT handle) {
    if(handle != NULL) {
        tree* treePtr = (tree*)handle;
        return treePtr->size;
    }
    else return 0;

}

/* Функция, определяющая, может ли данный итератор быть разыменован */
extern int LSQ_IsIteratorDereferencable(LSQ_IteratorT iterator) {
    if(iterator == NULL) {
        return 0;
    }
    iter* iterPtr = (iter*)iterator;
    if((iterPtr->current == NULL) ||
     (LSQ_IsIteratorBeforeFirst(iterPtr)) ||
      (LSQ_IsIteratorPastRear(iterPtr))) {
        return 0;
    }
    else {
        return 1;
    }
}
/* Функция, определяющая, указывает ли данный итератор на элемент, следующий за последним в контейнере */
extern int LSQ_IsIteratorPastRear(LSQ_IteratorT iterator) {
	 if(iterator == NULL) {
        return 0;
    }
    iter* iterPtr = (iter*)iterator;
    if(iterPtr->current == NULL) {
        return 0;
    }
    if(iterPtr->current->type == past_rear_node) {
        return 1;
    }
    else {
        return 0;
    }
}



/* Функция, определяющая, указывает ли данный итератор на элемент, предшествующий первому в контейнере */
extern int LSQ_IsIteratorBeforeFirst(LSQ_IteratorT iterator) {
	    if(iterator == NULL) {
        return 0;
    }
    iter* iterPtr = (iter*)iterator;
    if(iterPtr->current == NULL) {
        return 0;
    }
    if(iterPtr->current->type == before_first_node) {
        return 1;
    }
    else {
        return 0;
    }
}

static int IsIteratorFirst(LSQ_IteratorT iterator) {
    if(iterator == NULL) {
        return 0;
    }
    iter* iterPtr = (iter*)iterator;
    if(iterPtr->current == NULL) {
        return 0;
    }
    node* L = iterPtr->current->left;
    if(L == NULL) {
        return 0;
    }
    if(L->type == before_first_node) {
        return 1;
    }
    else {
        return 0;
    }
}
 
static int IsIteratorRear(LSQ_IteratorT iterator) {
    if(iterator == NULL) {
        return 0;
    }
    iter* iterPtr = (iter*)iterator;
    if(iterPtr->current == NULL) {
        return 0;
    }
    node* R = iterPtr->current->right;
    if(R == NULL) {
        return 0;
    }
    if(R->type == past_rear_node) {
        return 1;
    }
    else {
        return 0;
    }
}
/* Функция разыменовывающая итератор. Возвращает указатель на значение элемента, на который ссылается данный итератор */
extern LSQ_BaseTypeT* LSQ_DereferenceIterator(LSQ_IteratorT iterator) {	
    if(iterator == NULL) {
        return;
    }
    iter* iterPtr = (iter*)iterator;
    if(LSQ_IsIteratorDereferencable(iterPtr)) {
        return &(iterPtr->current->data);
    }
    else {
        return NULL;
    }

}
/* Функция разыменовывающая итератор. Возвращает указатель на ключ элемента, на который ссылается данный итератор */
extern LSQ_IntegerIndexT LSQ_GetIteratorKey(LSQ_IteratorT iterator) {
	if(iterator != NULL) {
	    iter* iterPtr = (iter*)iterator;
	    if(LSQ_IsIteratorDereferencable(iterPtr)) {
	        return (iterPtr->current->key);
	    }
	}
}

/* Следующие три функции создают итератор в памяти и возвращают его дескриптор */
/* Функция, возвращающая итератор, ссылающийся на элемент с указанным ключом. Если элемент с данным ключом  *
 * отсутствует в контейнере, должен быть возвращен итератор PastRear.                                       */
extern LSQ_IteratorT LSQ_GetElementByIndex(LSQ_HandleT handle, LSQ_IntegerIndexT index) {
    if(handle == NULL) {
        return NULL;
    }
    tree* treePtr = (tree*)handle;
    iter* iterPtr = PositioningByIndex(handle, index);
    if(!LSQ_IsIteratorDereferencable(iterPtr)){
        iterPtr->current = treePtr->pastRear;
        iterPtr->parent = iterPtr->current->parent; 
    }
    return iterPtr;    
}
/* Функция, возвращающая итератор, ссылающийся на первый элемент контейнера */
extern LSQ_IteratorT LSQ_GetFrontElement(LSQ_HandleT handle) {
	if(handle == NULL) {
        return NULL;
    }
    tree* treePtr = (tree*)handle;
    if(treePtr->size == 0) {
        return NULL;
    }
    iter* iterPtr = malloc(sizeof(iter));
    iterPtr->current = treePtr->beforeFirst->parent;
    iterPtr->parent = iterPtr->current->parent;
    return iterPtr;
}
/* Функция, возвращающая итератор, ссылающийся на фиктивный элемент, следующий за последним элементом контейнера */
extern LSQ_IteratorT LSQ_GetPastRearElement(LSQ_HandleT handle) {
	if(handle == NULL) {
        return NULL;
    }
    tree* treePtr = (tree*)handle;
    iter* iterPtr = malloc(sizeof(iter));
    iterPtr->current = treePtr->pastRear;
    iterPtr->parent = iterPtr->current->parent;
    return iterPtr;
}

/* Функция, уничтожающая итератор с заданным дескриптором и освобождающая принадлежащую ему память */
extern void LSQ_DestroyIterator(LSQ_IteratorT iterator) {
    if(iterator != NULL) {
        free(iterator);
    }
}

/* Следующие функции позволяют реализовать итерацию по элементам. При этом осуществляется проход только  *
 * по тем ключам, которые есть в контейнере.                                                             */
/* Функция, перемещающая итератор на один элемент вперед */
extern void LSQ_AdvanceOneElement(LSQ_IteratorT iterator) {
    if((iterator == NULL) || (LSQ_IsIteratorPastRear(iterator)))  {
        return;
    }
    iter* iterPtr = (iter*)iterator;
    if(iterPtr->current == NULL) {
        return;
    }
    if(iterPtr->current->right != NULL) {
        iterPtr->parent = iterPtr->current;
        iterPtr->current = iterPtr->current->right;
        GoDownLeft(iterPtr);
    }
    else {
        GoUpLeft(iterPtr);
    }
    iterPtr->current = iterPtr->parent;    
    iterPtr->parent = iterPtr->parent->parent; 
}
/* Функция, перемещающая итератор на один элемент назад */
extern void LSQ_RewindOneElement(LSQ_IteratorT iterator) {

    if((iterator == NULL) || (LSQ_IsIteratorBeforeFirst(iterator)))  {
        return;
    }
    iter* iterPtr = (iter*)iterator;
    if(iterPtr->current == NULL) {
        return;
    }
    if(iterPtr->current->left != NULL) {
        iterPtr->parent = iterPtr->current;
        iterPtr->current = iterPtr->current->left;
        GoDownRight(iterPtr);
    }
    else {
        GoUpRight(iterPtr);
    }  
    iterPtr->current = iterPtr->parent;
    iterPtr->parent = iterPtr->parent->parent;
}
 
extern void GoDownLeft(iter* iterPtr) {
    if(iterPtr->current != NULL) {
        iterPtr->parent = iterPtr->current;
        iterPtr->current = iterPtr->current->left;
        GoDownLeft(iterPtr);
    }
}
 
extern void GoUpLeft(iter* iterPtr) {
    if(iterPtr->current == iterPtr->parent->right) {
        iterPtr->current = iterPtr->parent;
        iterPtr->parent = iterPtr->parent->parent;
        GoUpLeft(iterPtr);
    }
}
 
extern void GoDownRight(iter* iterPtr) {
    if(iterPtr->current != NULL) {
        iterPtr->parent = iterPtr->current;
        iterPtr->current = iterPtr->current->right;
        GoDownRight(iterPtr);
    }
}
 
extern void GoUpRight(iter* iterPtr) {
    if(iterPtr->current == iterPtr->parent->left) {
        iterPtr->current = iterPtr->parent;
        iterPtr->parent = iterPtr->parent->parent;
        GoUpRight(iterPtr);
    }
}

/* Функция, перемещающая итератор на заданное смещение со знаком */
extern void LSQ_ShiftPosition(LSQ_IteratorT iterator, LSQ_IntegerIndexT shift) {
    if(iterator == NULL) {
        return;
    }
    iter* iterPtr = (iter*)iterator;
    if(iterPtr->current == NULL) {
        return;
    }
    if(shift > 0) {
        while((shift > 0) && (!LSQ_IsIteratorPastRear(iterPtr))) {
            LSQ_AdvanceOneElement(iterPtr);
            shift--;
        }
    }
    else {
        while((shift < 0) && (!LSQ_IsIteratorBeforeFirst(iterPtr))) {
            LSQ_RewindOneElement(iterPtr);
            shift++;
        }
    }
}
/* Функция, устанавливающая итератор на элемент с указанным номером */
extern void LSQ_SetPosition(LSQ_IteratorT iterator, LSQ_IntegerIndexT pos) {
    if(iterator == NULL) {
        return;
    }
    else {
        iter* iterPtr = (iter*)iterator;
        if(iterPtr->current == NULL) {
            return;
        }
        if(LSQ_IsIteratorPastRear(iterator)) {
            LSQ_RewindOneElement(iterator);
        }
        else if(LSQ_IsIteratorBeforeFirst(iterator)) {
            LSQ_AdvanceOneElement(iterator);
        }
        while((iterPtr->current->key != pos) && (LSQ_IsIteratorDereferencable(iterPtr))) {
            if(pos > iterPtr->current->key) {
                LSQ_AdvanceOneElement(iterPtr);
            }
            else {
                LSQ_RewindOneElement(iterPtr);
            }
        }
    }
}

/* Функция, добавляющая новую пару ключ-значение в контейнер. Если элемент с данным ключом существует,  *
 * его значение обновляется указанным.                                                                  */
extern void LSQ_InsertElement(LSQ_HandleT handle, LSQ_IntegerIndexT key, LSQ_BaseTypeT value) {
	tree* treePtr = (tree*)handle;
    iter* iterPtr = PositioningByIndex(handle, key);
    if(!LSQ_IsIteratorDereferencable(iterPtr)) {
        node* nodePtr = malloc(sizeof(node));
        nodePtr->data = value;
        nodePtr->key = key;
        nodePtr->diff = 0;  
        nodePtr->type = general_node;
 
        nodePtr->parent = NULL;
        nodePtr->right = NULL;
        nodePtr->left = NULL;
           
        if(LSQ_IsIteratorBeforeFirst(iterPtr)) {
            iterPtr->parent->left = nodePtr;
            iterPtr->current->parent = nodePtr;
   
            nodePtr->parent = iterPtr->parent;
            nodePtr->left = iterPtr->current;
        }
        else if(LSQ_IsIteratorPastRear(iterPtr)) {
            iterPtr->parent->right = nodePtr;
            iterPtr->current->parent = nodePtr;
   
            nodePtr->parent = iterPtr->parent;
            nodePtr->right = iterPtr->current; 
        }
        else {
            if(treePtr->size == 0) {
                treePtr->root = nodePtr;
                nodePtr->type = root_node;
       
                nodePtr->left = treePtr->beforeFirst;
                nodePtr->right = treePtr->pastRear;
 
                treePtr->beforeFirst->parent = nodePtr;
                treePtr->pastRear->parent = nodePtr;
            }
            else {
                if(key > iterPtr->parent->key) {
                    iterPtr->parent->right = nodePtr;  
                    nodePtr->parent = iterPtr->parent;
                }
                else {
                    iterPtr->parent->left = nodePtr;
                    nodePtr->parent = iterPtr->parent;
                }
            }
        }
        InsertTreeRetrieval(nodePtr,treePtr);
    }
    else if(LSQ_GetIteratorKey(iterPtr) == key) {
        iterPtr->current->data = value;
    }
    free(iterPtr);
    treePtr->size++;
}

static void InsertTreeRetrieval(node* N, tree* treePtr) {
    node* P = N->parent;
    if(N->type != root_node) { 
        if(N == P->right) {
            P->diff--;
        }
        else {
            P->diff++;
        }
        N = P;
        P = N->parent;
    }
    node* R = N->right;
    node* L = N->left;
    while((N != NULL) && (N->diff != 0)) {
        R = N->right;
        L = N->left; 
        if(N->diff == -2) {
            if((R->diff == -1) || (R->diff == 0)) {
                N = SmallTurnLeft(N, treePtr);
            }
            else {
                N = BigTurnLeft(N, treePtr);
            }
        }
        else if(N->diff == 2) {
            if((L->diff == 1) || (L->diff == 0)) {
                N = SmallTurnRight(N, treePtr);
            }
            else {
                N = BigTurnRight(N, treePtr);
            }
        }  
        else { 
            if(P != NULL) { 
                if(N == P->right) {
                    P->diff--;
                }
                else {
                    P->diff++;
                }
                P = P->parent;
            }
            N = N->parent;
        }
    }
}

/* Функция, удаляющая первый элемент контейнера */
extern void LSQ_DeleteFrontElement(LSQ_HandleT handle) {
	tree* treePtr = (tree*)handle;
    node* N = treePtr->beforeFirst->parent;
    iter* iterPtr = LSQ_GetFrontElement(handle);
    if((N->right != NULL) && (N->left != NULL)) {
        if((IsIteratorRear(iterPtr)) && (IsIteratorFirst(iterPtr))) {
            free(iterPtr->current);
            treePtr->beforeFirst->parent = NULL;
            treePtr->pastRear->parent = NULL;
            treePtr->root = NULL;
            treePtr->size--;
            return;
        }
        else {
            LSQ_AdvanceOneElement(iterPtr);
            N->key = iterPtr->current->key;
            N->data = iterPtr->current->data;
        }    
    }
    DeleteTreeRetrieval(iterPtr->current, treePtr);
    if(iterPtr->current->right != NULL) {
        if(iterPtr->current = iterPtr->parent->right) {
            iterPtr->parent->right = iterPtr->current->right;
        }
        else {
            iterPtr->parent->left = iterPtr->current->right;
        }
        iterPtr->current->right->parent = iterPtr->parent;
    }
    else {
        if(iterPtr->current == iterPtr->parent->right) {
            iterPtr->parent->right = iterPtr->current->left;
        }
        else {
            iterPtr->parent->left = iterPtr->current->left;
        }
        if(iterPtr->current->left != NULL) {
            iterPtr->current->left->parent = iterPtr->parent;
        }
    }
    treePtr->size--;
    free(iterPtr->current);
    free(iterPtr); 
}

/* Функция, удаляющая последний элемент контейнера */
extern void LSQ_DeleteRearElement(LSQ_HandleT handle) {
	tree* treePtr = (tree*)handle;
    node* N = treePtr->beforeFirst->parent;
    iter* iterPtr = LSQ_GetPastRearElement(handle);
    LSQ_RewindOneElement(iterPtr);
    if((N->right != NULL) && (N->left != NULL)) {
        if((IsIteratorRear(iterPtr)) && (IsIteratorFirst(iterPtr))) {
            free(iterPtr->current);
            treePtr->beforeFirst->parent = NULL;
            treePtr->pastRear->parent = NULL;
            treePtr->root = NULL;
            treePtr->size--;
            return;
        }
        else {
            LSQ_RewindOneElement(iterPtr);
            N->key = iterPtr->current->key;
            N->data = iterPtr->current->data;
        }    
    }
    DeleteTreeRetrieval(iterPtr->current, treePtr);
    if(iterPtr->current->right != NULL) {
        if(iterPtr->current = iterPtr->parent->right) {
            iterPtr->parent->right = iterPtr->current->right;
        }
        else {
            iterPtr->parent->left = iterPtr->current->right;
        }
        iterPtr->current->right->parent = iterPtr->parent;
    }
    else {
        if(iterPtr->current == iterPtr->parent->right) {
            iterPtr->parent->right = iterPtr->current->left;
        }
        else {
            iterPtr->parent->left = iterPtr->current->left;
        }
        if(iterPtr->current->left != NULL) {
            iterPtr->current->left->parent = iterPtr->parent;
        }
    }
    treePtr->size--;
    free(iterPtr->current);
    free(iterPtr); 
}
/* Функция, удаляющая элемент контейнера, указываемый заданным ключом. */
extern void LSQ_DeleteElement(LSQ_HandleT handle, LSQ_IntegerIndexT key) {
	if(handle == NULL) {
        return;
    }
    tree* treePtr = (tree*)handle;
    iter* iterPtr = PositioningByIndex(handle, key);
    if(!LSQ_IsIteratorDereferencable(iterPtr)) {
        return;
    }
    node* N = iterPtr->current;
    node* P = iterPtr->parent;
    if((N->right != NULL) && (N->left != NULL)) {
        if((IsIteratorRear(iterPtr)) && (IsIteratorFirst(iterPtr))) {
            free(iterPtr->current);
            treePtr->beforeFirst->parent = NULL;
            treePtr->pastRear->parent = NULL;
            treePtr->root = NULL;
            treePtr->size--;
            return;
        }
        else {
            if(IsIteratorRear(iterPtr)) {
                LSQ_RewindOneElement(iterPtr);
            }
            else {
                LSQ_AdvanceOneElement(iterPtr);
            }
            N->key = iterPtr->current->key;
            N->data = iterPtr->current->data;
        }
    }

    DeleteTreeRetrieval(iterPtr->current, treePtr);
    
    if(iterPtr->current->right != NULL) {
        if(iterPtr->current = iterPtr->parent->right) {
            iterPtr->parent->right = iterPtr->current->right;
        }
        else {
            iterPtr->parent->left = iterPtr->current->right;
        }
        iterPtr->current->right->parent = iterPtr->parent;
    }
    else {
        if(iterPtr->current == iterPtr->parent->right) {
            iterPtr->parent->right = iterPtr->current->left;
        }
        else {
            iterPtr->parent->left = iterPtr->current->left;
        }
        if(iterPtr->current->left != NULL) {
            iterPtr->current->left->parent = iterPtr->parent;
        }
    }
    treePtr->size--;
    free(iterPtr->current);
    free(iterPtr); 
}



static void DeleteTreeRetrieval(node* N, tree* treePtr) {
    node* P = N->parent;
    if(N->type != root_node) { 
        if(N == P->right) {
            P->diff++;
        }
        else {
            P->diff--;
        }
        N = P;
        P = N->parent;
    }
    node* R = N->right;
    node* L = N->left;
    while((N != NULL) && (N->diff != 1) && (N->diff != -1)) {
        P = N->parent;
        R = N->right;
        L = N->left;
        if(N->diff == -2) {
            if((R->diff == -1) || (R->diff == 0)) {
                N = SmallTurnLeft(N, treePtr);
            }
            else {
                N = BigTurnLeft(N, treePtr);
            }
        }
        else if(N->diff == 2) {
            if((L->diff == 1) || (L->diff == 0)) {
                N = SmallTurnRight(N, treePtr);
            }
            else {
                N = BigTurnRight(N, treePtr);
            }
        } 
        else { 
            if(P != NULL) { 
                if(N == P->right) {
                    P->diff++;
                }
                else {
                    P->diff--;
                }
                P = P->parent;
            }
            N = N->parent;
        }
    }
}


extern void DisplaySequence(LSQ_HandleT handle){
    tree* treePtr = (tree*)handle;
    iter* iterPtr = malloc(sizeof(iter));
    iterPtr->current = treePtr->root;
    iterPtr->parent == NULL;
    InOrderDisplay(iterPtr);
}
 
static void InOrderDisplay(iter* iterPtr) {
    if(LSQ_IsIteratorDereferencable(iterPtr)) {
        iterPtr->parent = iterPtr->current;
        iterPtr->current = iterPtr->current->left;
       
        InOrderDisplay(iterPtr);
       
        iterPtr->current = iterPtr->parent;
        iterPtr->parent = iterPtr->parent->parent;
 
        printf("%d %d %d %d\n", iterPtr->current->key, iterPtr->current->data, iterPtr->current->diff, iterPtr->current->type);
 
        iterPtr->parent = iterPtr->current;
        iterPtr->current = iterPtr->current->right;
       
        InOrderDisplay(iterPtr);
 
        iterPtr->current = iterPtr->parent;
        iterPtr->parent = iterPtr->parent->parent;
    }
}

int main(void) {
    tree* seq = LSQ_CreateSequence();

    iter* iterPtr = PositioningByIndex(seq, 10);

    if(seq->size == 0) {
        if(!LSQ_IsIteratorDereferencable(iterPtr)) {
            printf("okokok\n");
        }
    }

    return 0;
}