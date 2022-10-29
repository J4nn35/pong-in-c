CC := gcc
SRC := ./src
BIN := ./bin
INCLUDE := $(SRC)/include
CCFLAG := -Wall -I$(INCLUDE) -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -Wl,-subsystem,windows
OBJS := $(BIN)/main.o $(BIN)/game.o
EXE := $(BIN)/main

all: $(OBJS)
	@$(CC) $(OBJS) $(CCFLAG) -o $(EXE)

clean:
	@rm $(OBJS) $(EXE)

run:
	@$(EXE)

project:
	@mkdir src bin $(INCLUDE)

$(BIN)/%.o: $(SRC)/%.c
	@$(CC) -c $< $(CCFLAG) -o $@

$(BIN)/%.o: $(INCLUDE)/%.h
	@$(CC) -c $< $(CCFLAG) -o $@

$(BIN)/%.o:. $(SRC)/%.c $(INCLUDE)/%.h
	@$(CC) -c $< $(CCFLAG) -o $@