#ifndef KASJER_H
#define KASJER_H

#include <map>
#include <string>

class Kasjer {
private:
    int kasjerId;
    int kasaFifoFd;  // Deskryptor FIFO do odbierania koszyków
    std::map<int, int> sprzedaz;  // produktId -> łączna ilość sprzedana
    
    // Utworz FIFO dla kasy
    void utworzKase();
    
public:
    Kasjer(int id);
    ~Kasjer();
    
    // Główna pętla kasjera - obsługuje klientów
    void run();
    
    // Zwraca podsumowanie sprzedaży
    const std::map<int, int>& getSprzedaz() const { return sprzedaz; }
    int getId() const { return kasjerId; }
};

#endif
