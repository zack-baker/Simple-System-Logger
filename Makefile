all:
	clang++ -lsqlite3 syslog.cpp -o syslog
clean:
	rm syslog

