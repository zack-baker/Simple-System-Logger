all:
	mkdir bin
	gcc -o bin/sisyld src/sisyld.c
	gcc -o bin/sisyl_client src/sisyl_client_lib.c src/sisyl_client.c
clean:
	rm bin/*
test:
	gcc -o bin/run_sisyl_client_tests tests/all_tests.c tests/CuTest.c tests/sisyl_client_tests.c src/sisyl_client_lib.c 
	./bin/run_sisyl_client_tests
