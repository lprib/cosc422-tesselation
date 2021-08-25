SRC_FILES = $(wildcard src/*.c)
# Object files are the same as source files, but in out/ with .o extension
OBJ_FILES = $(addprefix out/,$(notdir $(SRC_FILES:.c=.o)))
MAIN_TARGET = out/main
CFLAGS = -lGL -lGLU -lGLEW -lglut -lm -Wall

# to build with system CGLM, run `make SYSTEM_CGLM=1`
ifndef SYSTEM_CGLM
CFLAGS += -I./cglm/include
CFLAGS += -L./cglm/build -l:libcglm.a
endif

out/%.o: src/%.c
	gcc $(CFLAGS) -c $< -o $@

$(MAIN_TARGET): $(OBJ_FILES)
	gcc $(CFLAGS) -o $@ $^

all: $(MAIN_TARGET)

clean:
	rm -f out/*

run: all
	$(MAIN_TARGET)

cglm_build:
	git submodule init
	git submodule update
	cd cglm && mkdir -p build && cd build && cmake -DCGLM_STATIC=ON .. && make

.PHONY: all clean run cglm_build
