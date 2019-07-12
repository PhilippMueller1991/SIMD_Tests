; ============================================================================================= RELEASE x64 - VecAddRef

; 15   : 	for (int i = 0; i < 4; i++)
; 16   : 		out.m[i] = A.m[i] + B.m[i];

	movss	xmm0, DWORD PTR [rdx]
	addss	xmm0, DWORD PTR [r8]
	movss	DWORD PTR [rcx], xmm0
	movss	xmm1, DWORD PTR [rdx+4]
	addss	xmm1, DWORD PTR [r8+4]
	movss	DWORD PTR [rcx+4], xmm1
	movss	xmm0, DWORD PTR [rdx+8]
	addss	xmm0, DWORD PTR [r8+8]
	movss	DWORD PTR [rcx+8], xmm0
	movss	xmm1, DWORD PTR [rdx+12]
	addss	xmm1, DWORD PTR [r8+12]
	movss	DWORD PTR [rcx+12], xmm1
	
; ============================================================================================= RELEASE x64 - VecAddSSE
; Did not change

; 21   : 	out.row = _mm_add_ps(A.row, B.row);

	movups	xmm0, XMMWORD PTR [rdx]
	addps	xmm0, XMMWORD PTR [r8]
	movups	XMMWORD PTR [rcx], xmm0