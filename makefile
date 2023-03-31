APP=lab1
SRCS=lab1.cpp Matrix.cpp
OBJS=$(SRCS:.cpp=.o)

CC=g++
CFLAGS=-Wall -Wextra -Werror -O0 -g
LDFLAGS=-lm

.PHONY: all clean

all: $(APP)

clean:
	@rm -rf *.o 

$(APP): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) -c $^
