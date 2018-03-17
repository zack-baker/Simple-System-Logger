all:
	gcc -o bin/sisyld src/sisyld.c
	gcc -o bin/sisyl_client src/sisyl_client.c
clean:
	rm bin/*
