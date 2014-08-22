all: rdseed rdseedi example
	#
rdseed: rdseed.c
	ocamlc -c rdseed.c
rdseedi: rdseed.mli
	ocamlc -c rdseed.mli
example: example.ml rdseedi rdseed
	ocamlfind ocamlopt \
-o example.native \
example.ml rdseed.mli rdseed.o

clean:
	rm *.cmi *.cmo *.cmx *.native *.o

