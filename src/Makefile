CC = gcc
SRC = main.c \
			minitile.c
LIBFILE = third_party/libraylib.a		# Linux version
LDLIBS = -lGL -lm -lpthread -ldl
OUT = minitile

default:
	$(CC) $(SRC) $(LIBFILE) $(LDLIBS) -o $(OUT)

ifeq ($(OS),Windows_NT)
	@echo Not available on Windows yet
endif
