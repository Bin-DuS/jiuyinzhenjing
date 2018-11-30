MBOOT_MAGIC  equ 0x1BADB002  ; multiboot magic�򣬱���Ϊ��ֵ
MBOOT_FLAGS  equ 0x00        ; multiboot flag��, GRUB����ʱ�Ƿ�Ҫ��һЩ�������
MBOOT_CHECKSUM  equ -(MBOOT_MAGIC + MBOOT_FLAGS) ; multiboot checksum��У�������������Ƿ���ȷ

[BITS 32]                    ; ��32λ����

section .text
  dd  MBOOT_MAGIC
  dd  MBOOT_FLAGS
  dd  MBOOT_CHECKSUM
  dd  start

[GLOBAL start]
[GLOBAL glb_mboot_ptr]
[EXTERN kernel_main]

start:
    cli
    mov esp, STACK_TOP
    mov ebp, 0
    and esp, 0FFFFFFF0H
    mov [glb_mboot_ptr], ebx
    call kernel_main      ; �����ں���ں���
stop:
    hlt
    jmp stop

;-----------------------------------------------------------------------------

section .bss
stack:
    resb 32768
glb_mboot_ptr:
    resb 4

STACK_TOP equ $-stack-1
