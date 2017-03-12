rm -f src/*~ include/*~

tar -czvf backup_`date +%Y-%m-%d`.tar.gz src/* include/* testes/in/* logPlayer/* makefile criar_backup.sh
