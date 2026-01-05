# Projekt: Ciastkarnia

**Autor:** Kopta Jakub  
**Nr albumu:** 155280  
**Temat projektu:** Ciastkarnia  
**Repozytorium GitHub:** https://github.com/jakubkopta/Ciastkarnia

---

## 1. Opis projektu

Ciastkarnia produkuje P różnych produktów (P>10), każdy w innej cenie i na bieżąco sprzedaje je w
samoobsługowym sklepie firmowym. Produkty bezpośrednio po wypieku (losowa liczba sztuk różnych
produktów co określony czas) trafiają do sprzedaży w sklepie - każdy rodzaj produktu Pi na oddzielny
podajnik. Każdy podajnik może przetransportować w danej chwili maksymalnie Ki sztuk pieczywa.
Ciastka z danego podajnika muszą być pobieranie w sklepie dokładnie w takiej kolejności jak zostało
położone na tym podajniku w piekarni.
Zasady działania ciastkarni przyjęte przez kierownika są następujące:
- Ciastkarnia jest czynna w godzinach od Tp do Tk;
- Sklep jest czynny w godzinach od Tp+30min do Tk;
- W sklepie w danej chwili może się znajdować co najwyżej N klientów (pozostali, jeżeli są
czekają przed wejściem);
- W sklepie są 2 stanowiska kasowe, zawsze działa min. 1 stanowisko kasowe.
- Na każdych K (K=N/2) klientów znajdujących się na terenie supermarketu powinno przypadać
min. 1 czynne stanowisko kasowe.
- Jeśli liczba klientów jest mniejsza niż N/2, to jedna z kas zostaje zamknięta.
- Jeśli w kolejce do kasy czekali klienci (przed ogłoszeniem decyzji o jej zamknięciu) to powinni
zostać obsłużeni przez tę kasę.

Klienci przychodzą do sklepu w losowych momentach czasu z losową listą zakupów (spośród P
produktów każdy klient wybiera min. dwa różne, np.: kremówki 3szt., WZ-tka 1 szt., …). Jeżeli dany
produkt jest niedostępny (podajnik jest pusty) klient nie kupuje tego towaru. Następnie klient udaje
się do kasy, a kasjer wystawia paragon na zakupy (kasjer zapamiętuje ile sztuk danego produktu
skasował).
Na komunikat (sygnał1) o inwentaryzacji - sygnał wysyła kierownik - klienci kontynuują zakupy
normalnie do zamknięcia sklepu. Po zamknięciu sklepu, każda kasa robi podsumowanie sprzedanych
produktów (Pi - liczba szt.), kierownik sumuje towar na podajnikach, piekarz podaje ilość
wytworzonych produktów - raport zapisany w pliku tekstowym.
Na komunikat (sygnał2) o ewakuacji - sygnał wysyła kierownik - klienci natychmiast przerywają
zakupy i opuszczają piekarnię omijając kasy - pobrany już z podajników towar odkładają do kosza
przy kasach.
Napisz program, kierownika, piekarza, kasjera i klienta. Raport z przebiegu symulacji zapisać w pliku
(plikach) tekstowym. 

---

## 2. Testy

### 2.1 Test 1 - Podstawowa sprzedaż
**Cel:** Sprawdzenie poprawności przepływu klientów i kasjerów.  
**Scenariusz:**  
- 5 klientów wchodzących losowo w czasie.  
- Każdy wybiera 2-3 produkty.  
- Podajniki mają wystarczającą ilość produktów.  
**Oczekiwany wynik:**  
- Klienci kupują wybrane produkty, paragon generowany prawidłowo, stan podajników zmniejsza się zgodnie z zakupami.  

### 2.2 Test 2 - Brak produktów
**Cel:** Sprawdzenie reakcji systemu na pusty podajnik.  
**Scenariusz:**  
- Klient wybiera produkt, którego podajnik jest pusty.  
**Oczekiwany wynik:**  
- Klient nie kupuje brakującego produktu, pozostałe produkty są zakupione, raport kasjera prawidłowy.  

### 2.3 Test 3 - Inwentaryzacja
**Cel:** Sprawdzenie poprawności generowania raportu po sygnale inwentaryzacji.  
**Scenariusz:**  
- Wysyłany sygnał1 podczas pracy sklepu.  
**Oczekiwany wynik:**  
- Klienci kontynuują zakupy, po zamknięciu sklepu generowany raport z liczbą sprzedanych produktów, stanem podajników i liczbą wyprodukowanych produktów.  

### 2.4 Test 4 - Ewakuacja
**Cel:** Sprawdzenie działania sygnału ewakuacji.  
**Scenariusz:**  
- Wysyłany sygnał2 podczas pracy sklepu.  
**Oczekiwany wynik:**  
- Klienci natychmiast opuszczają sklep, produkty z podajników trafiają do koszyków, żadne zakupy nie są realizowane po sygnale.  
