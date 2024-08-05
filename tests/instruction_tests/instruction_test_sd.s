	.include "test_defines.h"

	.text
	.align	2
	
	.globl	_start
_start:

	TEST_INIT

	li		t2, 0xf0e0d0c0b0a09080

	# Zero offset, base doubleword aligned

	la		t0, mem01+8
	sd		t2, 0(t0)
	la		t3, mem01
	ld		t1, 8(t3)
	TEST_EXPECT	t1, 0xf0e0d0c0b0a09080

	# Positive offset, base doubleword aligned

	la		t0, mem02+0
	sd		t2, 8(t0)
	la		t3, mem02
	ld		t1, 8(t3)
	TEST_EXPECT	t1, 0xf0e0d0c0b0a09080

	# Negative offset, base doubleword aligned

	la		t0, mem03+8
	sd		t2, -8(t0)
	la		t3, mem03
	ld		t1, 0(t3)
	TEST_EXPECT	t1, 0xf0e0d0c0b0a09080

	# Positive offset, base not doubleword aligned

	la		t0, mem04+5
	sd		t2, 3(t0)
	la		t3, mem04
	ld		t1, 8(t3)
	TEST_EXPECT	t1, 0xf0e0d0c0b0a09080

	# Negative offset, base not doubleword aligned

	la		t0, mem05+9
	sd		t2, -1(t0)
	la		t3, mem05
	ld		t1, 9(t3)
	TEST_EXPECT	t1, 0xf0e0d0c0b0a09080


	TEST_END

	.data
	.align	2
	
mem01:	.byte	0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37
	.byte	0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f
	
mem02:	.byte	0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37
	.byte	0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f
	
mem03:	.byte	0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37
	.byte	0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f
	
mem04:	.byte	0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37
	.byte	0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f
	
mem05:	.byte	0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37
	.byte	0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f
	
mem11:	.byte	0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37
	.byte	0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f
	
mem12:	.byte	0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37
	.byte	0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f
	
mem13:	.byte	0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37
	.byte	0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f
	
mem14:	.byte	0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37
	.byte	0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f
	
mem15:	.byte	0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37
	.byte	0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f
	
