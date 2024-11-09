TARGET = bin/dbview
SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c, obj/%.o, $(SRC))
DBFILE = ./mynewdb2.db
EMPLOYEES = \
	"Timmy H.,123 Sheshire Ln.,120" \
	"JimmyJohn D.,456 Elm St.,80" \
    "Jane S.,789 Maple Ave.,100" \
    "Alice B.,101 Oak Dr.,90"

run: clean default
	./$(TARGET)
	./$(TARGET) -f $(DBFILE) -n
	./$(TARGET) -f $(DBFILE)
	@for employee in $(EMPLOYEES); do \
 		./$(TARGET) -f $(DBFILE) -a "$$employee"; \
	done
	./$(TARGET) -f $(DBFILE) -l
	./$(TARGET) -f $(DBFILE) -a "Bob B.,123 Elm St.,40" -l
	./$(TARGET) -f $(DBFILE) -u "Jane S.,160" -l
	./$(TARGET) -f $(DBFILE) -u "Jonny D.,180" -l

default: $(TARGET)

clean:
	rm -f obj/*.o
	rm -f bin/*
	rm -f *.db

$(TARGET): $(OBJ)
	gcc -o $@ $?

obj/%.o : src/%.c
	gcc -c -g $< -o $@ -Iinclude


