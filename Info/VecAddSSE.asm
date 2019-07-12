; ============================================================================================= C++ implementation
;void VecAddSSE(Vec4f& out, const Vec4f& A, const Vec4f& B)
;{
;	out.row = _mm_add_ps(A.row, B.row);
;}

; ============================================================================================= DEBUG x64

; 21   : 	out.row = _mm_add_ps(A.row, B.row);

	mov	rax, QWORD PTR A$[rbp]
	mov	rcx, QWORD PTR B$[rbp]
	movups	xmm0, XMMWORD PTR [rax]
	addps	xmm0, XMMWORD PTR [rcx]
	movaps	XMMWORD PTR $T1[rbp], xmm0
	mov	rax, QWORD PTR out$[rbp]
	movaps	xmm0, XMMWORD PTR $T1[rbp]
	movups	XMMWORD PTR [rax], xmm0

; ============================================================================================= RELEASE x64


; 21   : 	out.row = _mm_add_ps(A.row, B.row);

	movups	xmm0, XMMWORD PTR [rdx]
	addps	xmm0, XMMWORD PTR [r8]
	movups	XMMWORD PTR [rcx], xmm0