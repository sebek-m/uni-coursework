default rel
extern debug
global notec:function

SYS_EXIT        equ 60
SYS_WRITE       equ 1
STDOUT          equ 1

; Specjalna wartość do zapamiętania, czy stos
; był cofnięty przed wołaniem debug.
TMP_ALIGN       equ 2

section .bss

; Tablica służąca do wymiany następującej w operacji W,
; poprzez udostępnianie sobie nawzajem adresów do swoich
; wartości z wierzchołka stosu przez Notecie.
; NULL w komórce tablicy oznacza, że Noteć nie udostępnia
; aktualnie swojej wartości.
alignb 8
my_value        resq N

section .data

; waiting_for[i] = nr Notecia, na którego czeka Noteć i,
;                  lub -1, jeśli na nikogo nie czeka
align 8
waiting_for     times N dq -1

section .text

; input:
;       edi - n, czyli nr instancji Notecia
;       rsi - calc, czyli wskaźnik na napis ASCIIZ, opisujący obliczenie
; output:
;       rax - wartość z wierzchołka stosu po zakończeniu obliczenia
align 8
notec:
        push    r12
        push    r13
        push    r14
        push    r15
        push    rbx

        mov     r12, rdi                 ; Nr instancji Notecia
        xor     r13, r13                 ; Wyłączony tryb wpisywania liczby
        mov     r14, 1                   ; Czy stos jest wyrównany do 16. Teraz jest, bo nie był, a wrzuciliśmy nieparzystą liczbę wartości.
        mov     r15, rsp                 ; Pozycja stosu, by móc pod koniec przeskoczyć stos Notecia, który może być wtedy niepusty.
        mov     rbx, rsi                 ; Posłuży nam do iteracji po napisie.
inspect_next_char:
        movzx   rdi, byte [rbx]          ; Znak na aktualnej pozycji
        test    rdi, rdi
        jz      calc_finished            ; Napotkaliśmy NULLa, czyli koniec napisu
        
        cmp     rdi, '='
        je      exit_num_mode
        cmp     rdi, '+'
        je      add_case
        cmp     rdi, '*'
        je      mul_case
        cmp     rdi, '-'
        je      neg_case
        cmp     rdi, '&'
        je      and_case
        cmp     rdi, '|'
        je      or_case
        cmp     rdi, '^'
        je      xor_case
        cmp     rdi, '~'
        je      not_case
        cmp     rdi, 'Z'
        je      del_case
        cmp     rdi, 'Y'
        je      dup_case
        cmp     rdi, 'X'
        je      swap_case
        cmp     rdi, 'N'
        je      N_case
        cmp     rdi, 'n'
        je      num_case
        cmp     rdi, 'g'
        je      dbg_case
        cmp     rdi, 'W'
        je      wait_case

        ; else: cyfra
        ; Musimy wydedukować, jakiego "rodzaju" jest to cyfra,
        ; żeby móc ją poprawnie przekonwertować na liczbę.
        cmp     rdi, '9'
        ja      maybe_upper_hex
        
        sub     rdi, '0'                 ; Konwersja cyfry na liczbę
        jmp     converted_to_num
maybe_upper_hex:
        cmp     rdi, 'F'
        ja      is_lower_hex

        sub     rdi, 55                  ; 'A' ma kod ASCII 65
        jmp     converted_to_num
is_lower_hex:
        sub     rdi, 87                  ; 'a' ma kod ASCII 97
converted_to_num:
        test    r13, r13                 ; Sprawdzamy, czy tryb wpisywania liczby był włączony.
        jnz     update_top_num           ; Jeśli nie, to uzupełniamy aktualnie wpisywaną liczbę.
        
        mov     r13, 1                   ; Włączmy tryb wpisywania liczby.
        push    rdi
        xor     r14, 1                   ; Wrzuciliśmy nową liczbę na stos, więc wyrównanie stosu się zmieniło.
        jmp     char_inspected
update_top_num:
        shl     qword [rsp], 4           ; Robimy miejsce na nową cyfrę w liczbie z wierzchołka stosu.
        add     [rsp], rdi               ; "Dopisujemy" cyfrę.
        jmp     char_inspected

add_case:
        pop     rdi                      ; Pierwsza liczba
        add     [rsp], rdi               ; Po pop, rsp wskazuje na uprzednio drugą liczbę na stosie.
        jmp     switch_alignment         ; O jedną liczbę mniej na stosie

mul_case:
        pop     rax                      ; Pierwsza liczba
        imul    qword [rsp]              ; Po pop, rsp wskazuje na uprzednio drugą liczbę na stosie.
        mov     [rsp], rax               ; Wynik
        jmp     switch_alignment         ; O jedną liczbę mniej na stosie

neg_case:
        neg     qword [rsp]
        jmp     exit_num_mode

and_case:
        pop     rdi                      ; Pierwsza liczba
        and     [rsp], rdi               ; Po pop, rsp wskazuje na uprzednio drugą liczbę na stosie.
        jmp     switch_alignment         ; O jedną liczbę mniej na stosie

or_case:
        pop     rdi                      ; Pierwsza liczba
        or      [rsp], rdi               ; Po pop, rsp wskazuje na uprzednio drugą liczbę na stosie.
        jmp     switch_alignment         ; O jedną liczbę mniej na stosie

xor_case:
        pop     rdi                      ; Pierwsza liczba
        xor     [rsp], rdi               ; Po pop, rsp wskazuje na uprzednio drugą liczbę na stosie.
        jmp     switch_alignment         ; O jedną liczbę mniej na stosie

not_case:
        not     qword [rsp]
        jmp     exit_num_mode

del_case:
        add     rsp, 8                   ; Przesuwamy stos na wartość za uprzednim wierzchołkiem.
        jmp     switch_alignment         ; O jedną liczbę mniej na stosie

dup_case:
        push    qword [rsp]              ; Wierzchołek wrzucamy jeszcze raz.
        jmp     switch_alignment         ; O jedną liczbę więcej na stosie

swap_case:
        pop     rdi
        push    qword [rsp]              ; "Wysuwamy" drugą na wierzch przez zduplikowanie jej.
        mov     [rsp + 8], rdi           ; Pierwsza jest teraz drugą.
        jmp     exit_num_mode

N_case:
        push    N
        jmp     switch_alignment         ; O jedną liczbę więcej na stosie

num_case:
        push    r12                      ; Nr instancji Notecia
        jmp     switch_alignment         ; O jedną liczbę więcej na stosie

dbg_case:
        mov     rdi, r12                 ; Nr instancji Notecia
        mov     rsi, rsp                 ; Wierzchołek stosu Notecia
        test    r14, r14
        jnz     stack_aligned            ; Sprawdzamy wyrównanie stosu.
        
        mov     r14, TMP_ALIGN           ; Specjalna wartość, by zaznaczyć, że mamy potem przywrócić pierwotny stan.
        sub     rsp, 8                   ; Wyrównujemy stos do 16.
stack_aligned:
        call    debug
        
        cmp     r14, TMP_ALIGN           ; Czy wyrównywaliśmy stos.
        jne     dbg_move_rsp
        
        add     rsp, 8                   ; Przywracamy poprzednią pozycję stosu.
        xor     r14, r14                 ; Wcześniej musieliśmy wyrównać stos, czyli nie był wyrównany.
dbg_move_rsp:
        lea     rsp, [rsp + 8 * rax]     ; Przesuwamy stos Notecia o liczbę pozycji z wyniku f. debug.
        bt      rax, 0
        jnc     exit_num_mode            ; Przesunęliśmy stos o parzystą liczbę pozycji, czyli wyrównanie względem 16 się nie zmieniło.
        jmp     switch_alignment         ; Poprawiamy info o wyrównaniu po powyższym przesunięciu.

wait_case:
        ; Zapamiętujemy początki tablic,
        ; by uniknąć relokacji względem nich,
        ; które powodowały błędy linkowania.
        ; Z tego co zrozumiałem, to napisanie
        ; np. [my_value + 8 * rdi] sprawia,
        ; że kod nie jest position independent.
        mov     r8, waiting_for
        mov     r9, my_value

        pop     rdi                      ; Zczytujemy m, czyli na kogo będziemy czekać.
        mov     [r9 + 8 * r12], rsp      ; Przygotowujemy udostępnienie adresu swojej wartości z wierzchołka stosu.
        mov     [r8 + 8 * r12], rdi      ; Informujemy na kogo czekamy.
        lea     rsi, [r8 + 8 * rdi]      ; Zapamiętujemy gdzie będziemy sprawdzać, czy m-ty już na nas czeka.
id_spinlock:
        cmp     r12, qword [rsi]         ; Czekamy, aż m-ty ustawi, że na nas czeka.
        jne     id_spinlock

        mov     qword [rsi], -1          ; m-ty już na nikogo nie czeka, bo my się zjawiliśmy.
        mov     rdx, [r9 + 8 * rdi]
        mov     rsi, [rdx]               ; Zapamiętujemy wartość z wierzchołka stosu m-tego.
        mov     qword [r9 + 8 * rdi], 0  ; Sygnalizujemy, że już nie potrzebujemy jego wartości.
        lea     rdx, [r9 + 8 * r12]      ; Zapamiętujemy, gdzie będziemy sprawdzać, czy m-ty już wziął naszą wartość.
value_spinlock:
        cmp     qword [rdx], 0           ; Czekamy aż m-ty weźmie już naszą wartość.
        jne     value_spinlock

        mov     [rsp], rsi               ; We własnym zakresie ustawiamy jego wartość na nasz wierzchołek.
        jmp     switch_alignment         ; Na początku zdejmowaliśmy m z wierzchołka, czyli o 1 obiekt mniej na stosie.

switch_alignment:
        xor      r14, 1                  ; Przełączamy informację o tym, czy stos jest wyrównany zgodnie z ABI.
exit_num_mode:
        mov      r13, 0                  ; Wyłączamy tryb wpisywania liczby.

char_inspected:
        inc     rbx                      ; Następny znak
        jmp     inspect_next_char

calc_finished:
        mov     rax, [rsp]               ; Zwracamy wartość z wierzchołka stosu.
        mov     rsp, r15                 ; Przesuwamy stos sprzętowy za stos Notecia, żeby móc przywrócić zapisane wcześniej wartości.
        pop     rbx
        pop     r15
        pop     r14
        pop     r13
        pop     r12
        ret