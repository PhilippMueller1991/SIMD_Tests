; ============================================================================================= C++ implementation
;void VecAddRef(Vec4f& out, const Vec4f& A, const Vec4f& B)
;{
;	for (int i = 0; i < 4; i++)
;		out.m[i] = A.m[i] + B.m[i];
;}

; ============================================================================================= DEBUG x64

; 15   : 	for (int i = 0; i < 4; i++)

	; int i = 0 
	mov	DWORD PTR i$1[rbp], 0
	jmp	SHORT $LN4@VecAddRef
$LN2@VecAddRef:
	; i++
	mov	eax, DWORD PTR i$1[rbp]
	inc	eax
	mov	DWORD PTR i$1[rbp], eax
$LN4@VecAddRef:
	; i < 4
	cmp	DWORD PTR i$1[rbp], 4
	jge	SHORT $LN3@VecAddRef

; 16   : 		out.m[i] = A.m[i] + B.m[i];

	movsxd	rax, DWORD PTR i$1[rbp]
	movsxd	rcx, DWORD PTR i$1[rbp]
	mov	rdx, QWORD PTR A$[rbp]
	mov	r8, QWORD PTR B$[rbp]
	movss	xmm0, DWORD PTR [rdx+rax*4]
	addss	xmm0, DWORD PTR [r8+rcx*4]
	movsxd	rax, DWORD PTR i$1[rbp]
	mov	rcx, QWORD PTR out$[rbp]
	movss	DWORD PTR [rcx+rax*4], xmm0
	jmp	SHORT $LN2@VecAddRef

; ============================================================================================= RELEASE x64

; 15   : 	for (int i = 0; i < 4; i++)
; 16   : 		out.m[i] = A.m[i] + B.m[i];

	; Loop unrolling optimization
	; out.m[0] = A.m[0] + B.m[0];
	movss	xmm0, DWORD PTR [rdx]
	addss	xmm0, DWORD PTR [r8]
	movss	DWORD PTR [rcx], xmm0
	
	; out.m[1] = A.m[1] + B.m[1];
	movss	xmm1, DWORD PTR [rdx+4]
	addss	xmm1, DWORD PTR [r8+4]
	movss	DWORD PTR [rcx+4], xmm1
	
	; out.m[2] = A.m[2] + B.m[2];
	movss	xmm0, DWORD PTR [rdx+8]
	addss	xmm0, DWORD PTR [r8+8]
	movss	DWORD PTR [rcx+8], xmm0
	
	; out.m[3] = A.m[3] + B.m[3];
	movss	xmm1, DWORD PTR [rdx+12]
	addss	xmm1, DWORD PTR [r8+12]
	movss	DWORD PTR [rcx+12], xmm1