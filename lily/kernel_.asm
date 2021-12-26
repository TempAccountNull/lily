extern ExecuteCallback : proc
.code
RunWithKernelStack PROC
swapgs
mov gs:[10h],rsp
mov rsp,gs:[1a8h]
sub rsp,1000h
call ExecuteCallback
add rsp,1000h
mov rsp,gs:[10h]
swapgs
ret
RunWithKernelStack ENDP
END