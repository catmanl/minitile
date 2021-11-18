CC = gcc
SRC = main.c \
			minitile.c \
			export.c
CFLAGS = -Wall
OUT = minitile

# Windows
ifeq ($(OS),Windows_NT)
	LDLIBS = -Lthird_party/windows -lraylib -lopengl32 -lwinmm -lgdi32
else # Linux
	LDLIBS = third_party/linux/libraylib.a -lGL -lm -lpthread -ldl
endif

default:
	$(CC) $(SRC) $(CFLAGS) $(LDLIBS) -o $(OUT)