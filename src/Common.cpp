#include "Common.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstdlib>
#include <iostream>

// Inicjalizacja listy produktów
std::vector<Produkt> produkty = {
    {0, "Kremówka", 3.50, 20},
    {1, "WZ-tka", 2.80, 25},
    {2, "Pączek", 2.50, 30},
    {3, "Rogalik", 2.20, 35},
    {4, "Drożdżówka", 3.00, 20},
    {5, "Sernik", 4.50, 15},
    {6, "Szarlotka", 4.00, 15},
    {7, "Makowiec", 3.80, 18},
    {8, "Babka", 3.20, 22},
    {9, "Eklerek", 3.50, 20},
    {10, "Beza", 2.00, 40},
    {11, "Ciastko czekoladowe", 3.00, 25}
};

std::string getFifoPath(int produktId) {
    return FIFO_DIR + FIFO_PREFIX + std::to_string(produktId);
}

void createFifoDirectory() {
    struct stat st;
    if (stat(FIFO_DIR.c_str(), &st) != 0) {
        // Katalog nie istnieje, tworzymy go
        if (mkdir(FIFO_DIR.c_str(), 0777) != 0) {
            std::cerr << "Błąd: Nie można utworzyć katalogu FIFO: " << FIFO_DIR << std::endl;
            exit(1);
        }
    }
}

void removeFifoDirectory() {
    // Usuwanie katalogu (w praktyce lepiej usuwać poszczególne FIFO)
    // Na razie zostawiamy puste - będziemy usuwać pojedyncze FIFO
}
