SYS_EXIT    equ 60
SYS_READ    equ 0
SYS_WRITE   equ 1
STDIN       equ 0
STDOUT      equ 1
BUF_LEN     equ 1024
MIN_ARG_NUM equ 2
MODULO_BASE equ 0x10FF80

FIVE_BYTES_BASE equ 11111000b

INNER_BYTE_MASK equ 00111111b
SECOND_BIT_MASK equ 01000000b

FOUR_UNI_MAX    equ 0x10FFFF

global _start

section .bss

input_buf       resb BUF_LEN + 4
output_buf      resb BUF_LEN + 4

section .rodata

output_buf_end  equ output_buf + BUF_LEN
utf8_masks      dq 0x7F, 0x1F3F, 0xF3F3F, 0x73F3F3F   ; Maski używane do ekstrakcji bitów Unicode z UTF-8 i wrzucania bitów Unicode na właściwe pozycje w UTF-8
utf8_bases      dq 0x0, 0xC080, 0xE08080, 0xF0808080  ; "Bazy" kodów UTF-8, czyli bajty z ustawionymi odpowiednimi początkowymi bitami
min_unicodes    dq 0x0, 0x80, 0x800, 0x10000          ; Minimalne kody Unicode, które można zakodować na danej liczbie bajtów w UTF-8

section .text

exit_error:
        call    print_output_buf
        mov     rax, SYS_EXIT ; Kod funkcji systemowej
        mov     rdi, 1        ; Kod błędu
        syscall

exit_ok:
        mov     rax, SYS_EXIT ; Kod funkcji systemowej
        xor     rdi, rdi      ; Zerowanie kodu powrotu
        syscall

invalid_char:
        jmp     exit_error

_start:
        mov     r9, qword [rsp]           ; Zapamiętujemy argc.
        cmp     r9, MIN_ARG_NUM
        jb      exit_error                ; Jeśli argc < 2, to znaczy, że nie podano a0.
        lea     rdi, [rsp + 16]           ; Adres argv[1]
        neg     r9                        ; Nie można odejmować przy lea
        lea     rsp, [rsp + r9 * 8]       ; Robimy miejsce na stosie na sparsowane współczynniki + oddzielający fragment.
        mov     r12, rsp                  ; Początek przyszłej listy współczynników
        call    parse_arguments
        mov     r13, rax                  ; Strażnik za listą współczynników
        mov     r14, output_buf           ; Aktualna pozycja w buforze wyjścia
read_next_portion:
        call    read_input
        test    rax, rax                 ; W rax jest liczba przeczytanych bajtów.
        jz      after_reading            ; Jeśli przeczytaliśmy 0 bajtów, to znaczy, że koniec wejścia.
        mov     r8, input_buf
        lea     r10, [input_buf + rax]   ; Koniec danych w buforze jako strażnik
diacriticize_char:
        call    read_char
        mov     rbx, rdx                 ; Zapamiętujemy liczbę bajtów wewnętrznych.
        test    rbx, rbx
        js      exit_error
        jz      ready_for_output         ; 0 bajtów wewnętrznych, czyli znak jest zapisany na jednym bajcie, więc go nie zmieniamy, bo jego Unicode jest z zakresu od 0x00 do 0x7F.
        mov     rdi, rax                 ; Wczytane bajty
        call    ensure_full_char
        mov     rbp, rax
        mov     rdi, rax                 ; Znak w UTF-8
        mov     rcx, rbx                 ; Liczba bajtów wewnętrznych
        call    check_inner_bytes
        ; Ekstrakcja znaku w Unicode
        pext    rdi, rbp, [utf8_masks + 8 * rbx]
        mov     rsi, rbx                 ; Liczba bajtów wewnętrznych znaku w UTF-8
        call    ensure_shortest
        call    calculate_polynomial
        mov     rbp, rax                 ; Unicode po diakrytyzacji
        mov     rdi, rax
        call    encode_utf8
ready_for_output:
        mov     rdi, rax                 ; Znak prawidłowo zakodowany w UTF-8
        mov     rsi, rdx
        inc     rsi                      ; Liczba bajtów w znaku
        call    output_diacriticized
after_diacriticization:
        cmp     r8, r10
        je      read_next_portion        ; Sprawdziliśmy cały aktualny input_buf, więc trzeba załadować dalsze dane.
        jmp     diacriticize_char        ; Kolejny znak z aktualnego bufora
after_reading:
        call    print_output_buf
        jmp     exit_ok


; input: rdi - znak do sprawdzenia
; output: rax - 0, jeśli w rdi była cyfra, 1 jeśli inny znak
check_if_digit:
        mov     rax, 1             ; Zwracana wartość. Zakładamy najpierw, że nie jest cyfrą.
        cmp     rdi, `0`
        jb      check_if_digit_end ; Znak jest mniejszy niż '0'.
        cmp     rdi, `9`
        ja      check_if_digit_end ; Znak jest większy niż '9'.
        xor     rax, rax           ; Znak jest z zakresu '0'-'9', czyli zwrócimy 0.
check_if_digit_end:
        ret

; input:
;       input_buf - bez ważnych danych
; output:
;       input_buf - z załadowanymi danymi z wejścia
;       rax       - liczba przeczytanych bajtów
read_input:
        mov     rax, SYS_READ
        mov     rdi, STDIN
        mov     rsi, input_buf
        mov     rdx, BUF_LEN
        syscall
        ret

; input:
;       rdi - kod znaku Unicode (x), >= 0x80
;       r12 - wskaźnik na a0 (początek listy współczynników)
;       r13 - wskaźnik na strażnika listy współczynnników
; output:
;       rax - w(x - 0x80) + 0x80, gdzie w to wielomian diakrytyzujący
calculate_polynomial:
        mov     r9, rdi                ; Zapamiętujemy x, żeby zwolnić rdi
        sub     r9, 0x80
        mov     rsi, 1                 ; x^0
        xor     rax, rax               ; Wynik
        mov     rcx, r12               ; Adres a0
polynomial_loop:
        cmp     rcx, r13
        je      polynomial_calculated  ; Dotarliśmy do strażnika tuż za listą współczynników

        mov     rdx, qword [rcx]       ; ak, gdzie k to nr aktualnego współczynnika
        imul    rdx, rsi               ; ak * x^k
        add     rax, rdx
        cmp     rax, MODULO_BASE
        jb      polynomial_updated
        mov     rdi, rax
        call    modulo                 ; Dotychczasowy wynik z powrotem w rax, tylko już z wartością modulo
polynomial_updated:
        add     rcx, 8                 ; Kolejny współczynnik
        imul    rsi, r9                ; x^(k+1)
        cmp     rsi, MODULO_BASE
        jb      polynomial_loop
        push    rax
        mov     rdi, rsi
        call    modulo
        mov     rsi, rax
        pop     rax
        jmp     polynomial_loop
polynomial_calculated:
        add     rax, 0x80
        ret

; input:  rdi - a
; output: rax - a % MODULO_BASE
modulo:
        xor     rdx, rdx               ; Zerujemy, bo div dzieli rdx:rax.
        mov     rax, rdi
        mov     r11, MODULO_BASE
        div     r11
        mov     rax, rdx               ; div zapisuje resztę w rdx.
        ret

; input:
;       r8  - aktualna pozycja w buforze
; output:
;       r8  - wskazuje na pozycję bufora za wczytanym znakiem
;       rax - wczytany znak lub kilka jego pierwszych bajtów
;       rdx - liczba bajtów wewnętrznych znaku
read_char:
        movzx   rdi, byte [r8]           ; Pierwszy bajt znaku
        call    is_inner_byte
        test    rax, rax
        jnz     exit_error               ; Pierwszy bajt ma postać bajtu wewnętrznego, więc nie jest poprawnym bajtem początkowym.
        cmp     rdi, FIVE_BYTES_BASE
        jae     exit_error               ; Pierwszy bajt ma postać bajtu początkowego znaku o co najmniej 5 bajtach, więc go nie dopuszczamy.
        mov     rcx, 3
byte_num_loop:
        mov     rdx, qword [utf8_bases + 8 * rcx]
        push    rcx
        imul    rcx, 8
        shr     rdx, cl
        pop     rcx
        cmp     dil, dl                  ; Sprawdzamy, w którą postać bajtu początkowego wpisuje się przeczytany bajt.
        jae     read_char_finish
        loop    byte_num_loop
read_char_finish:
        mov     rdx, rcx                 ; Liczba bajtów wewnętrznych
        mov     eax, dword [r8]          ; Ładujemy cały znak + potencjalnie zbędne bajty.
        bswap   eax                      ; Załadowane z pamięci bajty są w odwrotnej kolejności niż obsługiwane jest dalej.
        lea     r8, [r8 + rcx + 1]       ; Przesuwamy się w buforze za znak.
        lea     rsi, [rdx + 1]
        neg     rsi                      ; Nie można odejmować przy lea.
        lea     rcx, [4 + rsi]           ; O ile trzeba przesunąć, żeby uciąć zbędne bajty.
        imul    rcx, 8
        shr     rax, cl                  ; Ucinamy zbędne bajty
        ret

; input:
;       r8        - wskaźnik na adres oddalony o długość znaku od początku tego znaku
;       r10       - strażnik za danymi w buforze
;       rdi       - dane, co do których chcemy upewnić się, że są pełnym znakiem lub je uzupełnić jeśli nie są
; output:
;       rax       - pełny znak
;       input_buf - jeśli w rdi był pełny znak, to bez zmian. wpp. ciąg dalszy danych z wejścia
;       r8        - jeśli w rdi był pełny znak, to bez zmian. wpp. wskaźnik na aktualną pozycję w nowym buforze
;       r10       - jeśli w rdi był pełny znak, to bez zmian. wpp. strażnik za danymi w nowym buforze
ensure_full_char:
        mov     rcx, r8
        sub     rcx, r10
        jle     ensure_full_char_finish   ; Cały znak został wcześniej załadowany do bufora, bo r8 wskazuje na adres w obrębie bufora.
        ; Else: z aktualnego bufora zdołaliśmy odczytać tylko fragment znaku, a resztę musimy doczytać z wejścia.
        mov     r9, rdi                   ; read_input modyfikuje rdi
        push    rcx
        call    read_input
        pop     rcx
        mov     rdi, r9
        cmp     rax, rcx
        jb      exit_error               ; Załadowaliśmy mniej bajtów niż było nam potrzebne, czyli znak na pewno nie był zapisany poprawnie.
        
        lea     r8, [input_buf + rcx]    ; Aktualna pozycja w nowym buforze.
        lea     r10, [input_buf + rax]   ; Koniec danych w nowym buforze jako strażnik
        mov     r9d, dword [input_buf]   ; Końcówka znaku + coś co nas nie interesuje na końcu.
        bswap   r9d
        imul    rcx, 8                   ; Ile bitów zajmują bajty, których nam zabrakło.

        mov     rdx, 32
        sub     rdx, rcx                 ; O ile bitów więcej niż potrzeba załadowaliśmy do r9.
        push    rcx
        mov     rcx, rdx
        shr     r9, cl                   ; Bity z końcówki znaku przesuwamy na odpowiedni odcinek.
        pop     rcx

        shr     rdi, cl
        shl     rdi, cl                  ; Robimy miejsce na właściwą końcówkę znaku.
        or      rdi, r9                  ; Scalamy początek i końcówkę, by otrzymać cały znak.
ensure_full_char_finish:
        mov     rax, rdi
        ret

; input:
;       rdi - znak, którego wewnętrzne bajty chcemy zwalidować
;       rcx - liczba bajtów wewnętrznych znaku
; output:
;       Jeśli któryś bajt wewnętrzny jest nieprawidłowy, to program kończy działanie.
check_inner_bytes:
        call    is_inner_byte
        test    rax, rax
        jz      exit_error               ; Sprawdzany bajt nie był wewnętrzny.
        shr     rdi, 8                   ; Niech o jeden starszy bajt będzie teraz najmłodszy.
        loop    check_inner_bytes
        ret

; input:
;       rdi - rejestr z dolnym bajtem odpowiadającym pewnemu bajtowi wewnętrznemu
; output:
;       rax - 1: bajt jest wewnętrzny, 0: nie jest wewnętrzny
is_inner_byte:
        mov     rax, 0
        bt      rdi, 7                  ; Kopiuje najstarszy bit do CF
        jnc     after_checking_byte     ; Bajt nie ma 1 na najstarszym bicie.
        bt      rdi, 6                  ; Kopiuje drugi najstarszy bit do CF
        jc      after_checking_byte     ; Bajt nie ma 0 na drugim najstarszym bicie.
        mov     rax, 1
after_checking_byte:
        ret

; input:
;       rdi - Unicode znaku
;       rsi - liczba bajtów wewnętrznych w oryginalnym zapisie znaku z rdi w UTF-8
; output:
;       Jeśli znak nie był zakodowany w najkrótszy możliwy sposób, to program kończy działanie z błędem.
ensure_shortest:
        cmp     rdi, qword [min_unicodes + 8*rsi]
        jnae    exit_error
        cmp     rsi, 3
        jne     shortest_ensured
        cmp     rdi, FOUR_UNI_MAX        ; Jeśli były 4 bajty, to przy okazji sprawdzamy, czy Unicode nie przekracza ograniczenia górnego zadanego w treści.
        jnbe    exit_error
shortest_ensured:
        ret

; input:
;       rdi - Unicode zdiakrytyzowanego znaku
; output:
;       rax - liczba bajtów wewnętrznych w docelowym kodowaniu znaku z rdi w UTF-8
deduce_byte_num:
        mov     rcx, 3
check_byte_num:
        cmp     rdi, [min_unicodes + 8*rcx]
        jae     byte_num_deduced               ; Sprawdzamy, czy Unicode znaku musi zostać zakodowany w UTF-8 na liczbie bajtów z rcx + 1.
        loop    check_byte_num
byte_num_deduced:
        mov     rax, rcx
        ret

; input:
;       rdi - Unicode znaku
; output:
;       rax - znak z rdi zakodowany w UTF-8
;       rdx - liczba bajtów wewnętrznych znaku w UTF-8
encode_utf8:   
        call    deduce_byte_num
        mov     rdx, rax                          ; Liczba bajtów wewnętrznych
        pdep    rax, rdi, [utf8_masks + 8 * rdx]  ; Wrzucenie bitów kodu Unicode zdiakrytyzowanego znaku na docelowe pozycje w UTF-8     
        or      rax, [utf8_bases + 8 * rdx]       ; Scalamy bazę UTF-8 i bity z Unicode
        ret

; input:
;       rdi       - zdiakrytyzowany znak w UTF-8
;       rsi       - liczba bajtów znaku zakodowanego w UTF-8
;       r14       - wskaźnik na aktualną pozycję w output_buf
; output:
;       r14       - aktualna pozycja (za wrzuconym znakiem) w dotychczasowym lub nowym buforze
;       input_buf - zawiera teraz znak z rdi
;       stdout    - nic lub zawartość output_buf, jeśli znak się w nim nie mieścił
output_diacriticized:
        mov     rcx, 4
        sub     rcx, rsi
        imul    rcx, 8
        shl     rdi, cl                   ; Bajty znaku potencjalnie przesuniemy do lewej tak, by zawsze wrzucać 4 bajty do bufora.
        mov     rdx, r14
        add     rdx, rsi
        cmp     rdx, output_buf_end       ; Jak adres aktualnej pozycji w buforze będzie się miał do jego końca po wrzuceniu znaku.
        ja      buf_overflow              ; Znak nie zmieści się w buforze, więc musimy wypisać i opróżnić aktualny i wrzucić znak do nowego.
        bswap   edi
        mov     dword [r14], edi          ; Wrzucamy znak do bufora wyjścia. MOV nie narusza flag, więc poniższy skok warunkowy bazuje na wyniku powyższego cmp.
        mov     r14, rdx                  ; Adres za wrzuconym znakiem
        jmp     after_output  
buf_overflow:
        push    rdi
        push    rsi                       ; rdi i rsi są modyfikowane w print_output_buf.
        call    print_output_buf
        pop     rsi
        pop     rdi
        bswap   edi
        mov     dword [output_buf], edi   ; Wrzucamy znak na początek świeżego bufora.
        lea     r14, [output_buf + rsi]   ; Aktualna pozycja w świeżym buforze
after_output:
        ret

; input:
;       r14    - wskaźnik na aktualną pozycję w output_buf
; output:
;       stdout - dotychczasowa zawartość output_buf
print_output_buf:
        mov     rdi, STDOUT
        mov     rsi, output_buf
        mov     rdx, r14
        sub     rdx, output_buf           ; Liczba bajtów w buforze
        mov     rax, SYS_WRITE
        syscall
        ret

; input:
;       rdi  - adres argv[1]
;       r12  - wskaźnik na miejsce na stosie, za którym zmieszczą się wszystkie współczynniki
; output:
;       rax  - wskaźnik na slot stosu tuż za listą współczynników (strażnik)
;       stos - na odpowiednim odcinku wypełniony współczynnikami
parse_arguments:
        mov     r10, r12
        mov     r8, rdi                   ; rdi przyda nam się poniżej.
argument_parsing_loop:
        mov     r9, qword [r8]            ; Aktualnie sprawdzany argument
        test    r9, r9
        jz      after_parsing             ; Sprawdzamy czy obejrzeliśmy już wszystkie argumenty, czyli czy jesteśmy w argumencie z adresem zerowym.
        xor     rcx, rcx                  ; W rcx będziemy przechowywać chwilowo aktualnie parsowany współczynnik.
inspect_digit:
        movzx   rdi, byte [r9]            ; Ustawiamy aktualnie sprawdzany znak jako parametr dla check_if_digit.
        test    rdi, rdi
        jz      argument_parsed           ; Napotkaliśmy znak o kodzie 0, czyli dotarliśmy do końca napisu.
        call    check_if_digit
        test    rax, rax
        jnz     exit_error                ; rax był niezerowy, czyli okazało się, że znak nie jest cyfrą, więc argument nie jest prawidłowy.
        sub     rdi, `0`                  ; Konwersja ze znaku (cyfry) na liczbę.
        imul    rcx, 10
        add     rcx, rdi                  ; Uzupełniamy parsowany argument o wartość z aktualnej cyfry.
        cmp     rcx, MODULO_BASE
        jb      digit_inspected
        mov     rdi, rcx
        call    modulo                    ; Współczynniki wielomianu liczymy modulo, bo mogą być podane większe niż mieszczące się w 64 bitach, a potem i tak wielomian liczymy modulo.
        mov     rcx, rax
digit_inspected:
        inc     r9                        ; Kolejny znak
        jmp     inspect_digit
argument_parsed:
        mov     qword [r10], rcx          ; Wrzucamy współczynnik na właściwą pozycję na stosie.
        add     r8, 8                     ; Adres kolejnego argumentu
        add     r10, 8                    ; Kolejny slot na stosie
        jmp     argument_parsing_loop
after_parsing:
        mov     rax, r10
        ret