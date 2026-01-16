#ifndef COMMON_H
#define COMMON_H

#include <string>
#include <vector>
#include <ctime>

// Konfiguracja systemu
const int P = 12;  // Liczba różnych produktów (P>10)
const int N = 10;  // Maksymalna liczba klientów w sklepie
const int K = N / 2;  // K = N/2 - liczba klientów na kasę

// Struktura produktu
struct Produkt {
    int id;              // ID produktu (0 do P-1)
    std::string nazwa;   // Nazwa produktu
    double cena;         // Cena produktu
    int pojemnoscPodajnika;  // Maksymalna pojemność podajnika Ki
};

// Struktura wiadomości dla FIFO
struct WiadomoscFIFO {
    int produktId;       // ID produktu
    int ilosc;           // Liczba sztuk
    time_t czas;         // Czas operacji
};

// Ścieżki do FIFO
const std::string FIFO_DIR = "/tmp/ciastkarnia/";
const std::string FIFO_PREFIX = "podajnik_";

// Funkcje pomocnicze
std::string getFifoPath(int produktId);
void createFifoDirectory();
void removeFifoDirectory();

// Lista produktów (będzie zainicjalizowana w Common.cpp)
extern std::vector<Produkt> produkty;

#endif // COMMON_H
