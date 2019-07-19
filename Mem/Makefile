cli=client
ser=server
cc=g++

cliSrc=client.cpp comm.cpp
serSrc=server.cpp comm.cpp

.PHONY:all
all:$(cli) $(ser)

$(cli):$(cliSrc)
	$(cc) -g -o $@ $^
$(ser):$(serSrc)
	$(cc) -g -o $@ $^

.PHONY:clean
clean:
	rm -f $(cli) $(ser)