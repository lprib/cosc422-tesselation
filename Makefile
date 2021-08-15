SRC_FILES = $(wildcard src/*.c)
# Object files are the same as source files, but in out/ with .o extension
OBJ_FILES = $(addprefix out/,$(notdir $(SRC_FILES:.c=.o)))
MAIN_TARGET = out/main
CFLAGS = -lGL -lGLU -lGLEW -lglut -lm -Wall

out/%.o: src/%.c
	gcc $(CFLAGS) -c $< -o $@

$(MAIN_TARGET): $(OBJ_FILES)
	gcc $(CFLAGS) -o $@ $^

all: $(MAIN_TARGET)

clean:
	rm -f out/*

run: all
	$(MAIN_TARGET)

.PHONY: all clean run
