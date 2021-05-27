typeset : Doc.n Doc1.n Doc2.n
	typeset -P LW4101 -ms Doc.n

Doc : Doc.n Doc1.n Doc2.n
	-mv -f $@ ,$@
	nroff -ms Doc.n | page > Doc
#       fix up Doc by hand:
#         indent extra lines in .Ia and .Ba macros
#         fix widoes

Doc1.n : Doc1.tbl
	-mv -f $@ ,$@
	tbl Doc1.tbl > Doc1.n

Doc2.n : Doc2.tbl
	-mv -f $@ ,$@
	tbl Doc2.tbl > Doc2.n
