CC      = gcc
CFLAGS  = -g -Wall -Wextra
TARGET  = build/psi

SRC_DIR = src
OBJ_DIR = build
INC_DIR = include
TEST_DIR = unit-tests

SRC     = $(SRC_DIR)/main.c $(SRC_DIR)/SinglyLinkedList.c $(SRC_DIR)/operations.c $(SRC_DIR)/token.c $(SRC_DIR)/pval.c $(SRC_DIR)/hashtable.c $(SRC_DIR)/cell.c $(SRC_DIR)/env.c $(SRC_DIR)/evaluator.c
OBJ     = $(OBJ_DIR)/main.o $(OBJ_DIR)/SinglyLinkedList.o $(OBJ_DIR)/operations.o $(OBJ_DIR)/token.o $(OBJ_DIR)/pval.o $(OBJ_DIR)/hashtable.o $(OBJ_DIR)/cell.o $(OBJ_DIR)/env.o $(OBJ_DIR)/evaluator.o

# === Default build ===
all: $(TARGET)

$(TARGET): $(OBJ)
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET) -lm

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(INC_DIR)/*.h
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

# === Unit testing ===
TEST_SRC = $(filter-out $(TEST_DIR)/_%.c, $(wildcard $(TEST_DIR)/*.c))

test: $(TARGET)
	@echo "🧪 Running PSI tests..."
	@mkdir -p tests
	@./$(TARGET) < tests/phase2.psi | tee tests/out.txt
	
unit_tests: $(OBJ_DIR)/unit_tests
	@echo "🧪 Running all unit tests..."
	@./$(OBJ_DIR)/unit_tests

$(OBJ_DIR)/unit_tests: $(SRC) $(TEST_SRC)
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -I$(INC_DIR) $(SRC) $(TEST_SRC) -o $@ -lm

memcheck: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)

clean:
	rm -f $(OBJ) $(TARGET) $(OBJ_DIR)/unit_tests
