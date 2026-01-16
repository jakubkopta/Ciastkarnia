#include "Piekarz.h"
#include "Common.h"
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>

Piekarz::Piekarz() {
    fifoDeskryptory.resize(P, -1);
}

Piekarz::~Piekarz() {
    zamknijPodajniki();
}

void Piekarz::utworzPodajniki() {
    createFifoDirectory();
    
    std::cout << "[PIEKARZ] Tworzenie podajników..." << std::endl;
    
    for (int i = 0; i < P; i++) {
        std::string fifoPath = getFifoPath(i);
        
        // Usuń istniejące FIFO jeśli istnieje
        unlink(fifoPath.c_str());
        
        // Utwórz nowe FIFO
        if (mkfifo(fifoPath.c_str(), 0666) == -1) {
            std::cerr << "[PIEKARZ] Błąd tworzenia FIFO dla produktu " << i 
                      << " (" << produkty[i].nazwa << "): " 
                      << strerror(errno) << std::endl;
            exit(1);
        }
        
        std::cout << "[PIEKARZ] Utworzono podajnik dla: " << produkty[i].nazwa 
                  << " (FIFO: " << fifoPath << ")" << std::endl;
    }
    
    // Nie otwieramy jeszcze FIFO do zapisu - poczekamy aż klienci otworzą do odczytu
    // FIFO zostaną otwarte przy pierwszym zapisie
    std::cout << "[PIEKARZ] Podajniki gotowe. Czekam na klientów..." << std::endl;
}

void Piekarz::polozNaPodajnik(int produktId, int ilosc) {
    if (produktId < 0 || produktId >= P) {
        std::cerr << "[PIEKARZ] Nieprawidłowy ID produktu: " << produktId << std::endl;
        return;
    }
    
    // Sprawdź czy FIFO jest otwarte, jeśli nie - otwórz (O_WRONLY - blokuje się)
    // Zgodnie z dokumentacją: O_WRONLY blokuje się dopóki proces nie otworzy do odczytu
    if (fifoDeskryptory[produktId] == -1) {
        std::string fifoPath = getFifoPath(produktId);
        int fd = open(fifoPath.c_str(), O_WRONLY);  // Blokuje się do czasu otwarcia przez klienta
        if (fd == -1) {
            std::cerr << "[PIEKARZ] Nie można otworzyć podajnika dla " 
                      << produkty[produktId].nazwa << ": " << strerror(errno) << std::endl;
            return;
        }
        fifoDeskryptory[produktId] = fd;
        std::cout << "[PIEKARZ] Otworzono podajnik do zapisu: " << produkty[produktId].nazwa << std::endl;
    }
    
    // Przygotuj wiadomość
    WiadomoscFIFO wiadomosc;
    wiadomosc.produktId = produktId;
    wiadomosc.ilosc = ilosc;
    wiadomosc.czas = time(nullptr);
    
    // Zapisz do FIFO (zapisujemy pojedynczo, każdy produkt osobno)
    // Zgodnie z dokumentacją: zapis <= PIPE_BUF jest atomowy
    ssize_t written = write(fifoDeskryptory[produktId], &wiadomosc, sizeof(WiadomoscFIFO));
    
    if (written == -1) {
        std::cerr << "[PIEKARZ] Błąd zapisu do podajnika " << produkty[produktId].nazwa 
                  << ": " << strerror(errno) << std::endl;
    } else if (written != sizeof(WiadomoscFIFO)) {
        std::cerr << "[PIEKARZ] Niepełny zapis do podajnika " << produkty[produktId].nazwa << std::endl;
    } else {
        std::cout << "[PIEKARZ] Położono " << ilosc << " szt. " 
                  << produkty[produktId].nazwa << " na podajnik" << std::endl;
    }
}

void Piekarz::zamknijPodajniki() {
    for (int i = 0; i < P; i++) {
        if (fifoDeskryptory[i] != -1) {
            close(fifoDeskryptory[i]);
            fifoDeskryptory[i] = -1;
        }
    }
}
