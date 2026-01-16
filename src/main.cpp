#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include "Piekarz.h"
#include "Klient.h"
#include "Common.h"

int main() {
    std::cout << "=== CIASTKARNIA - Symulacja ===" << std::endl;
    std::cout << "Liczba produktów: " << P << std::endl;
    std::cout << "Maksymalna liczba klientów: " << N << std::endl;
    std::cout << std::endl;
    
    // Proces piekarza - tworzy FIFO
    pid_t piekarz_pid = fork();
    
    if (piekarz_pid == 0) {
        // Proces potomny - PIEKARZ
        Piekarz piekarz;
        piekarz.utworzPodajniki();
        
        // Czekamy chwilę, żeby klienci zdążyli otworzyć podajniki do odczytu
        sleep(2);
        
        // Symulacja wypieku - kładziemy produkty na podajniki
        std::cout << "[PIEKARZ] Rozpoczynam wypiek..." << std::endl;
        
        // Przykład: kładziemy różne produkty na podajniki
        piekarz.polozNaPodajnik(0, 5);  // 5 kremówek
        piekarz.polozNaPodajnik(1, 3);  // 3 WZ-tki
        piekarz.polozNaPodajnik(2, 8);  // 8 pączków
        piekarz.polozNaPodajnik(0, 2);  // jeszcze 2 kremówki
        
        sleep(2);  // Czekamy chwilę
        
        // Zostawiamy podajniki otwarte dla klientów
        std::cout << "[PIEKARZ] Produkty gotowe na podajnikach" << std::endl;
        
        // Piekarz działa dalej (w rzeczywistości będzie w pętli)
        sleep(5);
        
        piekarz.zamknijPodajniki();
        exit(0);
    } else if (piekarz_pid == -1) {
        std::cerr << "Błąd: Nie można utworzyć procesu piekarza" << std::endl;
        return 1;
    }
    
    // Czekamy chwilę, żeby piekarz utworzył FIFO
    sleep(1);
    
    // Proces klienta - otwiera podajniki do odczytu (blokuje się)
    pid_t klient_pid = fork();
    
    if (klient_pid == 0) {
        // Proces potomny - KLIENT
        Klient klient(1);
        klient.otworzPodajniki();
        
        // Czekamy chwilę, żeby piekarz zdążył położyć produkty
        sleep(3);
        
        std::cout << "[KLIENT 1] Rozpoczynam zakupy..." << std::endl;
        
        // Próbuję kupić różne produkty
        klient.pobierzZPodajnika(0, 3);  // 3 kremówki
        sleep(1);
        klient.pobierzZPodajnika(1, 2);  // 2 WZ-tki
        sleep(1);
        klient.pobierzZPodajnika(2, 5);  // 5 pączków
        
        // Wyświetl koszyk
        std::cout << "[KLIENT 1] Podsumowanie zakupów:" << std::endl;
        for (const auto& item : klient.getKoszyk()) {
            std::cout << "  - " << produkty[item.first].nazwa 
                      << ": " << item.second << " szt." << std::endl;
        }
        
        klient.zamknijPodajniki();
        exit(0);
    } else if (klient_pid == -1) {
        std::cerr << "Błąd: Nie można utworzyć procesu klienta" << std::endl;
        return 1;
    }
    
    // Proces główny czeka na zakończenie procesów potomnych
    waitpid(piekarz_pid, nullptr, 0);
    waitpid(klient_pid, nullptr, 0);
    
    std::cout << std::endl << "=== Symulacja zakończona ===" << std::endl;
    
    return 0;
}
