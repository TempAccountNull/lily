.code
RunWithKernelStack PROC
swapgs
mov gs:[10h],rsp
mov rsp,gs:[1A8h]
sub rsp,78h
mov rax,rcx
mov rcx,rdx
call rax
mov rsp,gs:[10h]
swapgs
ret
RunWithKernelStack ENDP
END