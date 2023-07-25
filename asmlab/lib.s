
	.text
	
.globl intClone
.globl intCmp

.globl strCmp
.globl strLen
.globl strDelete
.globl strClone

.globl arrayGetSize
.globl arrayAddLast
.globl arrayGet

.globl listNew
.globl listGetSize
.globl listGet
.globl listAddFirst
.globl listClone
.globl createNewNode

.globl cardCmp
.globl cardClone
.globl cardAddStacked
.globl cardDelete


.equ OFFSET_CARD_SUIT, 0
.equ OFFSET_CARD_NUMBER, 8
.equ OFFSET_CARD_STACK, 16

.equ OFFSET_LIST_TYPE, 0
.equ OFFSET_LIST_SIZE, 4
.equ OFFSET_LIST_FIRST, 8
.equ OFFSET_LIST_LAST, 16

.equ OFFSET_ELEM_DATA, 0
.equ OFFSET_NEXT, 8
.equ OFFSET_PREV, 16

.equ OFFSET_ARRAY_TYPE, 0
.equ OFFSET_ARRAY_SIZE, 4
.equ OFFSET_ARRAY_CAPACITY, 5
.equ OFFSET_ARRAY_DATA, 8

.equ SIZE_STRUCT_CARD, 24
.equ SIZE_STRUCT_LIST, 24
.equ SIZE_STRUCT_ELEM, 24
.equ SIZE_INT, 4 
.equ SIZE_POINTER, 8



/*
NOTA
Pasaje de parametros:
	1 RDI
	2 RSI
	3 RDX
	4 RCX
	5 R8
	6 R9
*/
# ** Int **

# int32_t* intClone(int32_t* a rdi)
intClone:
	push %rbp
	mov %rsp, %rbp 
	push %r12 # r12 es no volatil :) 
	sub $8, %rsp 
	# doc sobre memory allocation en c:
	# https://www.cs.uaf.edu/2010/fall/cs301/lecture/10_04_malloc.html#:~:text=Calling%20Malloc%20from%20Assembly%20Language,allocated%20bytes%20returned%20in%20rax.
	mov %rdi, %r12 # movemos el puntero en rdi a r12 para preservarlo 
	mov $SIZE_INT, %rdi # movemos el tamanio del int a rdi para que lo reciba malloc
	call malloc # pointer generado va a volver en rax
	mov (%r12), %ecx # int apuntado lo copiamos en rcx
	mov %ecx, (%rax) # escribimos en rax el int que teniamos guardado

	add $8, %rsp
	pop %r12 
	pop %rbp
	ret

# ** String **

# int32_t strCmp(char* a, char* b)
strCmp: 
	push %rbp 
	mov %rsp, %rbp 
	xor %rax, %rax # me aseguro acumulador en 0
	xor %rcx, %rcx # index == 0
	xor %rdx, %rdx # donde vamos a guardar string a en cero

	# 0 si son iguales
	# 1 si a < b
	# −1 si b < a

	.cmp_loop:
		mov (%rdi, %rcx,1), %al # a en acumulador rax 8 bytes, eax 4 bytes, al es 1 byte
		mov (%rsi, %rcx,1), %dl # b en otro registro (como son chars basta con escalar de a 1) 
		inc %rcx # incrementas el indice
		cmp %al, %dl # comparas chars 
		jl .a_greater # jump si menor
		jg .b_greater # jump si mayor
		cmp $0, %dl # son iguales, son cero?
		je .exit # si son iguales y son cero, terminamos de recorrer el string
		jmp .cmp_loop

	.exit: 
		pop %rbp
		ret
		
	.a_greater:
		mov $-1, %rax
		jmp .exit

	.b_greater:
		mov $1, %rax 
		jmp .exit


# void strDelete(char* a)
strDelete:
	call free 
	ret

# uint32_t strLen(char* a)
strLen:
	xor %eax, %eax # vamos a usarlo como contador del length
	
	.len_loop:
		movb (%rdi,%rax,1), %cl # movemos el string a rcx para poder comparar con cl, solo el ultimo byte
		inc %eax # incrementamos el contador
		cmp $0, %cl # ver si no es cero (termino el string)
		jne .len_loop # si no es cero, seguimos recorriendo el string
		dec %eax # contador -1 para no tomar en cuenta el null
		ret

# ** Array **

# uint8_t  arrayGetSize(array_t* a)
arrayGetSize:
	mov OFFSET_ARRAY_SIZE(%rdi), %al # offset + rsi, rsi+1
	/*
	typedef struct s_array {
	type_t  type; enum de 4b          p
	uint8_t size; uint8 de 1b		p+4
	uint8_t capacity; uint8 de 1b	p+5
	PADDING 						2B 
	void** data; ptr de 8b			p+8
	tamanio total 16B
} array_t;
	*/
	ret


# void  arrayAddLast(array_t* a [rdi], void* data [rsi])
arrayAddLast:
	push %rbp
	mov %rsp, %rbp 
	push %r13
	sub $8, %rsp

	mov OFFSET_ARRAY_CAPACITY(%rdi), %dl # ARRAY CAPACITY en registro rdx
	mov OFFSET_ARRAY_SIZE(%rdi), %al # ARRAY SIZE en registro rax
	cmp %al, %dl # comparamos la capacidad con el size actual
	je .exitAddLast # si el size es igual a la capacidad, termina
	
	mov %rdi, %r13 # preservamos array_t*
	# copiar datos
	mov %rsi, %rdi # movemos la carta a rdi
	
	mov OFFSET_ARRAY_TYPE(%r13), %edx
	cmp $3,%edx
	ja .exitAddLast
	jmp *.switch_addLast(,%edx,8)
	
	.switch_addLast:
		.quad .exitAddLast
		.quad .addInt
		.quad .addString
		.quad .addCard

	.addString:
		call strClone
		jmp .continue
	.addCard:
		call cardClone # clon de carta va a volver en rax
		jmp .continue
	.addInt: 
		call intClone
		jmp .continue

	# rax pointer al dato clonado
	# rdx al array data
	# rcx array size
	# rbx
	.continue:
		xor %rcx, %rcx
		mov OFFSET_ARRAY_SIZE(%r13), %cl # array size
		mov OFFSET_ARRAY_DATA(%r13), %rdx # **data
		leaq (%rdx, %rcx, SIZE_POINTER), %rbx
		mov %rax, (%rbx) # movemos el pointer en rax al ultimo lugar del array 
		incb OFFSET_ARRAY_SIZE(%r13) # sumamos uno al size actual por el nuevo dato agregado

	.exitAddLast:	
		add $8, %rsp 
		pop %r13 
		pop %rbp
		ret



# void* arrayGet(array_t* a [rdi], uint8_t i [rsi])
arrayGet:
	xor %rax, %rax
	mov OFFSET_ARRAY_SIZE(%rdi), %al # ARRAY SIZE en registro rax
	cmp %rax, %rsi # comparamos el size con el indice dado
	jge .out_of_bounds
	mov OFFSET_ARRAY_DATA(%rdi), %rcx 
	mov (%rcx, %rsi, SIZE_POINTER), %rax # me muevo a ese espacio en el index y lo mando a rax [data + i*bytes]
	# recordar que data es un void** asi que lo que tenemos es un array de punteros
	ret 

	.out_of_bounds:
	xor %rax, %rax
	ret     


# ** Lista **

# list_t* listNew(type_t t)
listNew:
	push %rbp
	mov %rsp, %rbp 
	push %r12 					# type_t
	sub $8, %rsp
	mov %rdi, %r12 				# movemos type_t a r12 para preservarlo 

	# pedidos de memoria
	mov $SIZE_STRUCT_LIST, %rdi # movemos el tamanio del struct a rdi para que lo reciba malloc
	call malloc 				# pedir 24b para allocar el struct list (lo pedimos ultimo para que ya quede en el rax)
	
	# inicializamos los elementos del struct lista
	mov %r12, OFFSET_LIST_TYPE(%rax) 			# movemos el type_t al primer lugar 
	mov $0, OFFSET_LIST_SIZE(%rax) 			# size de la lista en cero
	movq $0, OFFSET_LIST_FIRST(%rax)			# ponemos el mismo puntero a null en el primer y ultimo lugar
	movq $0, OFFSET_LIST_LAST(%rax)			
	
	add $8, %rsp
	pop %r12
	pop %rbp
	ret

# uint8_t  listGetSize(list_t* l)
listGetSize:
	mov OFFSET_LIST_SIZE(%rdi), %al
	ret
	# nos desplazamos al seg elemento del struct list y lo llevamos al acumulador para retornarlo
	# el primer elemento es un enum de 0 a 3, asi que el tama;o es 4b
	# typedef struct s_list {
	# type_t   type;  >>>>>>>>>>>>>>> 4 bytes (p)
	# uint8_t  size;  >>>>>>>>>>>>>>> 1 byte (p + 4)
	# PADDING         >>>>>>>>>>>>>>> 3 bytes
	# struct s_listElem* first; >>>>> 8 bytes (p + 8)
	# struct s_listElem* last;  >>>>> 8 bytes (p + 16)
	# } list_t;
	# total = 24B, multiplo de 8
	# notar que el struct list no esta ordenado de forma optima por lo que el padding esta entre los dif elementos
	# y el elemento mas grande es un puntero, por lo que debe estar alineado a 64(8)
	

# void listAddFirst(list_t* l, void* data)
listAddFirst:
	push %rbp
	mov %rsp, %rbp 
	push %r12					# ptr l
	push %r13 					# ptr data
	push %r14 					# ptr newNode
	sub $8, %rsp

	movq %rdi, %r12 			# preservamos pointer a la lista
	movq %rsi, %r13 			# preservamos pointer a los datos

	mov %r12, %rsi				# list type para ser usado en el switch
	mov %r13, %rdi 				# pointer a datos en rdi
	call createNewNode 				
	mov %rax, %r14 

	# inicializamos elementos del nuevo nodo
	movq $0, OFFSET_PREV(%r14)
	# isEmpty ==
	cmp $0, OFFSET_LIST_SIZE(%r12)
	je .empty_list
	mov OFFSET_LIST_FIRST(%r12), %rdx
	movq %rdx, OFFSET_NEXT(%r14) # direccion del que era el primer nodo en el next del nuevo nodo 
	movq %r14, OFFSET_PREV(%rdx) # si la lista no esta vacia, el nuevo nodo es el prev del nodo que estaba primero

	.exit_addfirst:
		movq %r14, OFFSET_LIST_FIRST(%r12) # direccion del nuevo nodo en el first del struct list
		incb OFFSET_LIST_SIZE(%r12)

		add $8, %rsp
		pop %r14
		pop %r13
		pop %r12
		pop %rbp
		ret

	.empty_list:
		movq $0, OFFSET_NEXT(%r14)
		movq %r14, OFFSET_LIST_LAST(%r12) # si la lista esta vacia, el ultimo elemento es nuestro nuevo nodo
		jmp .exit_addfirst


# void* listGet(list_t* l [rdi], uint8_t i[rsi])
listGet:
	xor %rax, %rax
	mov OFFSET_LIST_SIZE(%rdi), %al			# tamanio de la lista en rax temporalmente
	cmp %ax, %si
	jge .invalid_node 							# si esta fuera de rango, terminamos								# rcx sera el contador
	xor %ecx, %ecx
	movq OFFSET_LIST_FIRST(%rdi), %rdx 			# nos mov al primer elem de la lista y lo guardamos en rdx

	# rdx puntero a nodo
	# rcx contador de posiciones/nodos
	# rsi nodo al que tenemos que ir
	.loop_list:
		cmp %cx, %si							# i = nro de elemento de la lista en el que estamos?
		je .get_node
		inc %ecx
		movq OFFSET_NEXT(%rdx), %rbx
		movq %rbx, %rdx 						# guardar la direccion en next
		jmp .loop_list

	.get_node:
		movq OFFSET_ELEM_DATA(%rdx), %rax
		ret 

	.invalid_node:
		xor %rax, %rax
		ret


# ** Card **

# int32_t cardCmp(card_t* a, card_t* b)
cardCmp: 
	push %rbp
	mov %rsp, %rbp 
	push %r12					# ptr card a
	push %r13 					# ptr card b

	# preservo parametros de la funcion 
	movq %rdi, %r12       
    movq %rsi, %r13       

    movq OFFSET_CARD_SUIT(%r12), %rdi 
	movq OFFSET_CARD_SUIT(%r13), %rsi 
	call strCmp 
	cmp $0, %rax
	jnz .exit_cardCmp

	sub $8, %rsp        
    movq OFFSET_CARD_NUMBER(%r12), %rdi
	movq OFFSET_CARD_NUMBER(%r13), %rsi
	call intCmp
	add $8, %rsp

	.exit_cardCmp:
		pop %r13
		pop %r12
		pop %rbp
		ret

# card_t* cardClone(card_t* c)
cardClone:
	push %rbp
    mov %rsp, %rbp
	push %r12 # pointer a card a clonar pasado por rdi
	push %r13 # pointer a suit
	push %r14 # pointer a number
	push %r15 # pointer a stack

    movq %rdi, %r12
	# clonamos cada elemento de la carta y lo guardamos en el registro reservamo para eso
    movq OFFSET_CARD_SUIT(%r12), %rdi
    call strClone
	movq %rax, %r13
	sub $8, %rsp
	movq OFFSET_CARD_NUMBER(%r12), %rdi
    call intClone
	movq %rax, %r14
	sub $8, %rsp
    movq OFFSET_CARD_STACK(%r12), %rdi
    call listClone
	movq %rax, %r15
	sub $8, %rsp
	movq $SIZE_STRUCT_CARD, %rdi
    call malloc

	movq %r13, OFFSET_CARD_SUIT(%rax)
	movq %r14, OFFSET_CARD_NUMBER(%rax)
	movq %r15, OFFSET_CARD_STACK(%rax)

	add $24, %rsp
	pop %r15
	pop %r14
	pop %r13
	pop %r12
	pop %rbp
	ret

# void cardAddStacked(card_t* c rdi, card_t* card rsi)
cardAddStacked:
	push %rbp
    mov %rsp, %rbp

	mov OFFSET_CARD_STACK(%rdi), %rdi   # ptr a stack de c(carta a la que anadir); ptr a card ya esta en el rsi
    call listAddFirst

	pop %rbp
	ret

# void cardDelete(card_t* c)
cardDelete:
	push %rbp
    mov %rsp, %rbp
	push %r12
	sub $8, %rsp

    movq %rdi, %r12
    movq OFFSET_CARD_NUMBER(%r12), %rdi
    call free
	sub $8, %rsp
	movq OFFSET_CARD_SUIT(%r12), %rdi
    call free
	sub $8, %rsp
    movq OFFSET_CARD_STACK(%r12), %rdi
    call listDelete
	sub $8, %rsp
	movq %r12, %rdi
    call free
	add $24, %rsp
	
	.exit_cardDelete:
		add $8, %rsp
		pop %r12
		pop %rbp
		ret


