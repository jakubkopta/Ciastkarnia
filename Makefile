# Kompilator
CXX = clang++
# Standard C++
CXXFLAGS = -std=c++17 -Iinclude -Wall -Wextra

# Foldery
SRC_DIR = src
OBJ_DIR = obj
BIN = ciastkarnia

# Wszystkie pliki źródłowe
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
# Pliki obiektowe w obj/
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))

# Domyślne zadanie
all: $(OBJ_DIR) $(BIN)

# Tworzy katalog obj, jeśli nie istnieje
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Kompilacja plików .cpp do .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Linkowanie do binarki
$(BIN): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Czyszczenie
clean:
	rm -rf $(OBJ_DIR) $(BIN)

.PHONY: all clean
