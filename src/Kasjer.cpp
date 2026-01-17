#include "Kasjer.h"
#include "Common.h"
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>
#include <iomanip>
#include <signal.h>

static bool kasjerDziala = true;

void kasjerSignalHandler(int sig) {
    if (sig == SIGTERM) {
        kasjerDziala = false;
    }
}

// Struktura wiadomości do kasy
struct WiadomoscKasa {
    int klientId;
    int produktId;
    int ilosc;
    bool koniec;  // true jeśli to ostatnia wiadomość od klienta
};

Kasjer::Kasjer(int id) : kasjerId(id), kasaFifoFd(-1) {
    utworzKase();
}

Kasjer::~Kasjer() {
    if (kasaFifoFd != -1) {
        close(kasaFifoFd);
    }
}

void Kasjer::utworzKase() {
    createFifoDirectory();
    
    std::string kasaPath = FIFO_DIR + "kasa_" + std::to_string(kasjerId);
    
    // Usuń istniejące FIFO jeśli istnieje
    unlink(kasaPath.c_str());
    
    // Utwórz FIFO dla kasy
    if (mkfifo(kasaPath.c_str(), 0666) == -1) {
        std::cerr << "[KASJER " << kasjerId << "] Błąd tworzenia FIFO kasy: " 
                  << strerror(errno) << std::endl;
        exit(1);
    }
    
    // Otwórz FIFO do odczytu (O_RDONLY - blokuje się do czasu otwarcia przez klienta)
    kasaFifoFd = open(kasaPath.c_str(), O_RDONLY);
    if (kasaFifoFd == -1) {
        std::cerr << "[KASJER " << kasjerId << "] Błąd otwarcia FIFO kasy: " 
                  << strerror(errno) << std::endl;
        exit(1);
    }
    
    std::cout << "[KASJER " << kasjerId << "] Kasa gotowa (FIFO: " << kasaPath << ")" << std::endl;
}

void Kasjer::run() {
    // Ustaw handler sygnału
    signal(SIGTERM, kasjerSignalHandler);
    
    std::cout << "[KASJER " << kasjerId << "] Czekam na klientów..." << std::endl;
    
    WiadomoscKasa wiadomosc;
    int aktualnyKlient = -1;
    std::map<int, int> koszykKlienta;  // produktId -> ilość dla aktualnego klienta
    
    while (kasjerDziala) {
        ssize_t read_bytes = read(kasaFifoFd, &wiadomosc, sizeof(WiadomoscKasa));
        
        if (read_bytes == -1) {
            if (errno == EINTR) {
                // Przerwane przez sygnał
                if (!kasjerDziala) {
                    break;
                }
                continue;
            }
            std::cerr << "[KASJER " << kasjerId << "] Błąd odczytu z kasy: " 
                      << strerror(errno) << std::endl;
            break;
        }
        
        if (read_bytes == 0) {
            // EOF - koniec
            break;
        }
        
        if (read_bytes != sizeof(WiadomoscKasa)) {
            std::cerr << "[KASJER " << kasjerId << "] Niepełny odczyt z kasy" << std::endl;
            continue;
        }
        
        // Nowy klient
        if (aktualnyKlient != wiadomosc.klientId) {
            if (aktualnyKlient != -1) {
                // Zakończ obsługę poprzedniego klienta
                std::cout << "[KASJER " << kasjerId << "] Paragon wystawiony" << std::endl;
                std::cout << std::endl;
            }
            
            aktualnyKlient = wiadomosc.klientId;
            koszykKlienta.clear();
            std::cout << "[KASJER " << kasjerId << "] Obsługuję klienta " << aktualnyKlient << std::endl;
        }
        
        // Dodaj produkt do koszyka klienta
        if (wiadomosc.ilosc > 0) {
            koszykKlienta[wiadomosc.produktId] += wiadomosc.ilosc;
        }
        
        // Jeśli to koniec koszyka klienta, wystaw paragon
        if (wiadomosc.koniec) {
            std::cout << "[KASJER " << kasjerId << "] Skasowano:" << std::endl;
            double suma = 0.0;
            
            for (const auto& item : koszykKlienta) {
                int produktId = item.first;
                int ilosc = item.second;
                double cena = produkty[produktId].cena;
                double wartosc = cena * ilosc;
                suma += wartosc;
                
                // Aktualizuj sprzedaż
                sprzedaz[produktId] += ilosc;
                
                std::cout << "  - " << produkty[produktId].nazwa << ": " << ilosc << std::endl;
            }
            
            std::cout << "[KASJER " << kasjerId << "] Suma: " 
                      << std::fixed << std::setprecision(2) << suma << " zł" << std::endl;
            std::cout << "[KASJER " << kasjerId << "] Paragon wystawiony" << std::endl;
            std::cout << std::endl;
            
            aktualnyKlient = -1;
            koszykKlienta.clear();
        }
    }
}
