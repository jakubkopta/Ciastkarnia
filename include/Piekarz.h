#ifndef PIEKARZ_H
#define PIEKARZ_H

#include <string>
#include <vector>

class Piekarz {
private:
    std::vector<int> fifoDeskryptory;  // Deskryptory FIFO dla każdego produktu
    
public:
    Piekarz();
    ~Piekarz();
    
    // Tworzy FIFO dla wszystkich podajników
    void utworzPodajniki();
    
    // Kładzie produkty na podajnik (zapisuje do FIFO)
    void polozNaPodajnik(int produktId, int ilosc);
    
    // Zamyka wszystkie podajniki
    void zamknijPodajniki();
};

#endif
