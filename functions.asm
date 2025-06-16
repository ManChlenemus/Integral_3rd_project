section .data
    const_neg2  dq -2.0  
    const_2     dq 2.0
    const_6     dq 6.0
    const_14    dq 14.0
    const_1     dq 1.0

section .text
global f1, f2, f3, df1, df2, df3
global test1, test2, test3, dtest1, dtest2, dtest3

; Основные функции (вариант 7)
; ln(x)
f1:
    push ebp
    mov ebp, esp
    fldln2
    fld qword [ebp + 8]
    fyl2x
    leave
    ret

; -2x + 14
f2:
    push ebp
    mov ebp, esp
    fld qword [const_neg2]
    fmul qword [ebp + 8]
    fadd qword [const_14]
    leave
    ret

; 1/(2-x) + 6
f3:
    push ebp
    mov ebp, esp
    fld qword [const_2]
    fsub qword [ebp + 8]
    fld1
    fdivrp
    fadd qword [const_6]
    leave
    ret

; Производные основных функций
; 1/x
df1:
    push ebp
    mov ebp, esp
    fld1
    fdiv qword [ebp + 8]
    leave
    ret

; -2
df2:
    push ebp
    mov ebp, esp
    fld qword [const_neg2]
    leave
    ret

; 1/(2-x)^2
df3:
    push ebp
    mov ebp, esp
    fld qword [const_2]
    fsub qword [ebp + 8]
    fmul st0, st0
    fld1
    fdivrp
    leave
    ret

; Тестовые функции
; x^2
test1:
    push ebp
    mov ebp, esp
    fld qword [ebp + 8]
    fmul qword [ebp + 8]
    leave
    ret

; sin(x)
test2:
    push ebp
    mov ebp, esp
    fld qword [ebp + 8]
    fsin
    leave
    ret

; x + 1
test3:
    push ebp
    mov ebp, esp
    fld qword [ebp + 8]
    fld1
    faddp
    leave
    ret

; Производные тестовых функций
; 2x
dtest1:
    push ebp
    mov ebp, esp
    fld qword [ebp + 8]
    fld1
    fld1
    faddp
    fmulp
    leave
    ret

; cos(x)
dtest2:
    push ebp
    mov ebp, esp
    fld qword [ebp + 8]
    fcos
    leave
    ret

; 1
dtest3:
    push ebp
    mov ebp, esp
    fld1
    leave
    ret
