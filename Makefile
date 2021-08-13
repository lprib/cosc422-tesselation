
SRC_FILES = $(wildcard src/*.c)
OBJ_FILES = $(addprefix out/,$(notdir $(SRC_FILES:.c=.o)))
MAIN_TARGET = out/main

CFLAGS = -lGL -lGLU -lglut -lm

out/%.o: src/%.c
	gcc $(CFLAGS) -c $< -o $@

$(MAIN_TARGET): $(OBJ_FILES)
	gcc $(CFLAGS) -o $@ $^

all: $(MAIN_TARGET)

clean:
	rm -rf $(OBJ_FILES) $(MAIN_TARGET)

run: all
	$(MAIN_TARGET)

.PHONY: all clean run
