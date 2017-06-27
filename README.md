# Project: Gydict

![](gydict.png "Gydict")

## Program Gydict
**Gydict** jest programem do obsługi różnych słowników multimedialnych (darmowych i komercyjnych) dostępnych pod Windows. Program działa pod systemem Linux.

### Wymagania
Aby skompilować program, będzie potrzebna wersja deweloperska biblioteki **gtk+** w wersji 3.22. Do wygenerowanie pliku **configure** będzie potrzebna paczka **gnome-common**, w której znajduje się skrypt **gnome-autogen.sh**.

### Jak skompilować?
Należy wykonać następujące polecenia:

```
 ./autogen
 make
 make install
 ```
### Cechy programu Gydict:
*  reaguje na schowek;
*  umożliwia  ustawienia własnych ścieżek do słowników oraz własnych fontów;
*  program posiada historię używanych słów;
*  możliwość wydrukowania aktualnie wyświetlanej definicji słowa.

## Jakie słowniki obsługuje Gydict?

### PWN Oxford 2006/2007
Moduł obsługuje słowniki angielsko-polski oraz polsko-angielski. Niezbędne jest posiadania dwóch plików z już zainstalowanej aplikacji pod Windows. Są to pliki '**angpol.win**' oraz '**polang.win**'.

### Wielki mutlimedialny słownik Niemiecko-Polski i Polsko-Niemiecki
Od wersji 0.3.0 Gydict obsługuje kolejny słownik PWN. Niezbędne jest posiadania dwóch plików z już zainstalowanej aplikacji pod Windows. Są to pliki '**niempol.win**' oraz '**polniem.win.**'.

### Słownik DEPL
Jest to darmowy słownik niemiecko-polski. Aby słownik działał należy wejść na stronę www.depl.pl i pobrać aktualną bazę wyrazów lub w konsoli wpisać polecenie: wget http://www.depl.pl/wyrazy.zip
Z archiwum **wyrazy.zip** bierzemy plik **a.dat**.

## Konfiguracja programu Gydict

### Konfiguracja ścieżek do słowników.
Przy pierwszym uruchomieniu należy ustawić ścieżki do słowników. Należy otworzyć okno preferencji i przejść do zakładki **Słowniki** i tam odpowiednio skorzystać z odpowiedniego przycisku.

### Konfiguracja czcionki.
Aby zmienić domyślną czcionkę, należy uruchomić program **Gydict**, potem wejść w menu aplikacji i wybrać **Preferencje**, i następnie przejść do zakładki **Ustawienia domyślne** i następnie wybrać odpowiednią czcionkę.

## Kontakt
|             |                          |
| :----:      | :----:                   |
| Author      | Jakub Czartek            |
| Email       | kuba@linux.pl            |
| Home Page   | http://jczartek.github.io/gydict |
| License     | GNU General Public License, version 2 (http://www.gnu.org/licenses/old-licenses/gpl-2.0.html) |

