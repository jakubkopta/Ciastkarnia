#include "Klient.h"
#include "Common.h"
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>
#include <algorithm>

Klient::Klient(int id) : klientId(id) {
    fifoDeskryptory.resize(P, -1);
}

Klient::~Klient() {
    zamknijPodajniki();
}

void Klient::otworzPodajniki() {
    // Nie otwieramy wszystkich podajników na raz
    // Będą otwierane na żądanie przy pobieraniu produktów
    std::cout << "[KLIENT " << klientId << "] Gotowy do zakupów" << std::endl;
}

bool Klient::pobierzZPodajnika(int produktId, int ilosc) {
    if (produktId < 0 || produktId >= P) {
        std::cerr << "[KLIENT " << klientId << "] Nieprawidłowy ID produktu: " << produktId << std::endl;
        return false;
    }
    
    // Otwórz podajnik jeśli nie jest jeszcze otwarty (O_RDONLY - blokuje się)
    if (fifoDeskryptory[produktId] == -1) {
        std::string fifoPath = getFifoPath(produktId);
        std::cout << "[KLIENT " << klientId << "] Otwieram podajnik: " 
                  << produkty[produktId].nazwa << " (czekam na piekarza)..." << std::endl;
        int fd = open(fifoPath.c_str(), O_RDONLY);  // Blokuje się do czasu otwarcia przez piekarza
        
        if (fd == -1) {
            std::cerr << "[KLIENT " << klientId << "] Błąd otwarcia podajnika dla " 
                      << produkty[produktId].nazwa << ": " << strerror(errno) << std::endl;
            return false;
        }
        
        fifoDeskryptory[produktId] = fd;
        std::cout << "[KLIENT " << klientId << "] Otworzono podajnik: " 
                  << produkty[produktId].nazwa << std::endl;
    }
    
    // Próbuj pobrać żądaną ilość produktów
    int pobrano = 0;
    WiadomoscFIFO wiadomosc;
    
    while (pobrano < ilosc) {
        ssize_t read_bytes = read(fifoDeskryptory[produktId], &wiadomosc, sizeof(WiadomoscFIFO));
        
        if (read_bytes == -1) {
            std::cerr << "[KLIENT " << klientId << "] Błąd odczytu z podajnika " 
                      << produkty[produktId].nazwa << ": " << strerror(errno) << std::endl;
            return false;
        }
        
        if (read_bytes == 0) {
            // EOF - podajnik jest pusty
            std::cout << "[KLIENT " << klientId << "] Podajnik " 
                      << produkty[produktId].nazwa << " jest pusty" << std::endl;
            break;
        }
        
        if (read_bytes != sizeof(WiadomoscFIFO)) {
            std::cerr << "[KLIENT " << klientId << "] Niepełny odczyt z podajnika" << std::endl;
            continue;
        }
        
        // Sprawdź czy to właściwy produkt
        if (wiadomosc.produktId != produktId) {
            std::cerr << "[KLIENT " << klientId << "] Błąd: otrzymano produkt " 
                      << wiadomosc.produktId << " zamiast " << produktId << std::endl;
            continue;
        }
        
        // Dodaj do koszyka
        int doPobrania = std::min(wiadomosc.ilosc, ilosc - pobrano);
        koszyk[produktId] += doPobrania;
        pobrano += doPobrania;
        
        std::cout << "[KLIENT " << klientId << "] Pobrano " << doPobrania << " szt. " 
                  << produkty[produktId].nazwa << " (łącznie w koszyku: " 
                  << koszyk[produktId] << ")" << std::endl;
        
        // Jeśli w wiadomości było więcej niż potrzebujemy, nie możemy zwrócić reszty
        // (FIFO działa jednokierunkowo)
    }
    
    return pobrano > 0;
}

void Klient::zamknijPodajniki() {
    for (int i = 0; i < P; i++) {
        if (fifoDeskryptory[i] != -1) {
            close(fifoDeskryptory[i]);
            fifoDeskryptory[i] = -1;
        }
    }
}

void Klient::przejdzDoKasy(int kasjerId) {
    if (koszyk.empty()) {
        std::cout << "[KLIENT " << klientId << "] Koszyk pusty, pomijam kasę" << std::endl;
        return;
    }
    
    std::cout << "[KLIENT " << klientId << "] Przechodzę do kasy" << std::endl;
    
    // Otwórz FIFO kasy do zapisu
    std::string kasaPath = FIFO_DIR + "kasa_" + std::to_string(kasjerId);
    int kasaFd = open(kasaPath.c_str(), O_WRONLY);
    
    if (kasaFd == -1) {
        std::cerr << "[KLIENT " << klientId << "] Błąd otwarcia kasy: " 
                  << strerror(errno) << std::endl;
        return;
    }
    
    // Struktura wiadomości do kasy
    struct WiadomoscKasa {
        int klientId;
        int produktId;
        int ilosc;
        bool koniec;
    };
    
    // Wyślij wszystkie produkty z koszyka
    for (const auto& item : koszyk) {
        WiadomoscKasa wiadomosc;
        wiadomosc.klientId = klientId;
        wiadomosc.produktId = item.first;
        wiadomosc.ilosc = item.second;
        wiadomosc.koniec = false;
        
        ssize_t written = write(kasaFd, &wiadomosc, sizeof(WiadomoscKasa));
        if (written != sizeof(WiadomoscKasa)) {
            std::cerr << "[KLIENT " << klientId << "] Błąd wysyłania do kasy" << std::endl;
        }
    }
    
    // Wyślij sygnał końca koszyka
    WiadomoscKasa wiadomosc;
    wiadomosc.klientId = klientId;
    wiadomosc.produktId = -1;
    wiadomosc.ilosc = 0;
    wiadomosc.koniec = true;
    
    write(kasaFd, &wiadomosc, sizeof(WiadomoscKasa));
    
    close(kasaFd);
    
    std::cout << "[KLIENT " << klientId << "] Zakończyłem zakupy" << std::endl;
}
