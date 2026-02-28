;UFMT-Compiladores
;Prof. Ivairton
;Procedimento para geracao do executavel apos compilacao (em Linux):
;(1) compilacao do Assembly com nasm: $ nasm -f elf64 <nome_do_arquivo>
;(2) likedicao: $ ld -m elf_x86_64 <nome_arquivo_objeto>

extern printf
extern scanf

	section .text
	global main,_start
main:
_start:

;escreve valor string
mov edx,16
mov ecx,str0
mov ebx,1
mov eax,4
int 0x80

; --- le valor inteiro ---
mov edx, 2
mov ecx, buffer_io
mov ebx, 0
mov eax, 3
int 0x80
movzx eax, byte [buffer_io]
sub eax, 48
mov dword [x], eax

;escreve valor string
mov edx,16
mov ecx,str1
mov ebx,1
mov eax,4
int 0x80

; --- le valor inteiro ---
mov edx, 2
mov ecx, buffer_io
mov ebx, 0
mov eax, 3
int 0x80
movzx eax, byte [buffer_io]
sub eax, 48
mov dword [y], eax

;escreve valor string
mov edx,16
mov ecx,str2
mov ebx,1
mov eax,4
int 0x80
;Chamada de funcao
jal func_calcula

;escreve valor string
mov edx,16
mov ecx,str3
mov ebx,1
mov eax,4
int 0x80

;encerra programa
mov ebx,0
mov eax,1
int 0x80
func_calcula:
;Carrega valor de variavel
mov eax, dword [x]
push rax
;Carrega valor de variavel
mov eax, dword [y]
push rax
;Adicao
pop rax
pop rbx
add rax,rbx
push rax
;Atribuicao
pop rax
mov [x], eax

;escreve valor string
mov edx,16
mov ecx,str4
mov ebx,1
mov eax,4
int 0x80

; --- escreve valor inteiro ---
mov eax, dword [x]
add eax, 48
mov byte [buffer_io], al
mov edx, 1
mov ecx, buffer_io
mov ebx, 1
mov eax, 4
int 0x80

;Retorna da funcao
jr $ra

	section .data
buffer_io db 0, 0
x: dd 0
y: dd 0
str0: db "Informe o valor de X:"
str1: db "\nInforme o valor de Y:"
str2: db "\nChamando a funcao...\n"
str3: db "\nDe volta ao programa principal!"
str4: db "Resultado de X + Y dentro da funcao: "
