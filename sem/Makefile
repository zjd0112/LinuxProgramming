bin=test
cc=g++
src=comm.cpp test.cpp

$(bin):$(src)
	$(cc) -o $@ $^

.PHONY:clean
clean:
	rm -f $(bin)