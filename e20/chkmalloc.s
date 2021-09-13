LL0:
	.text
	.align	1
	.globl  mcount
mcount:
	calls	$0,_veralloc
	rsb
