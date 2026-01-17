#ifndef KLIENT_H
#define KLIENT_H

#include <vector>
#include <map>

struct Zakup {
    int produktId;
    int ilosc;
};

class Klient {
private:
    int klientId;
    std::vector<int> fifoDeskryptory;  // Deskryptory FIFO do odczytu
    std::map<int, int> koszyk;  // produktId -> ilość
    
public:
    Klient(int id);
    ~Klient();
    
    // Otwiera podajniki do odczytu (O_RDONLY - blokuje się do czasu otwarcia do zapisu)
    void otworzPodajniki();
    
    // Pobiera produkt z podajnika (czyta z FIFO)
    bool pobierzZPodajnika(int produktId, int ilosc);
    
    // Zamyka podajniki
    void zamknijPodajniki();
    
    // Idzie do kasy i wysyła koszyk
    void przejdzDoKasy(int kasjerId);
    
    // Zwraca koszyk klienta
    const std::map<int, int>& getKoszyk() const { return koszyk; }
    
    int getId() const { return klientId; }
};

#endif
