./bin/soinstala testes/in/cenario1 testes/out/cenario1_grupo

./soinstala_prof/bin/soinstala soinstala_prof/testes/in/cenario1 testes/out/cenario1_prof

sort testes/out/cenario1_prof -o testes/out/cenario1_prof

sort testes/out/cenario1_grupo -o testes/out/cenario1_grupo

DIFF=$(diff testes/out/cenario1_grupo testes/out/cenario1_prof)

if [ "$DIFF" != "" ]
then
	echo "ficheiros iguais"
else
	echo "ficheiros diferentes"
fi
