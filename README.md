# Project: Gydict

## Program Gydict
**Gydict** jest programem do obsługi różnych słowników multimedialnych (darmowych i komercyjnych) dostępnych pod Windows. Program działa pod systemem Linux.

### Wymagania
Aby skompilować program, będzie potrzebna wersja deweloperska biblioteki **gtk+** w wersji 3.14. Do wygenerowanie pliku **configure** będzie potrzebna paczka **gnome-common**, w której znajduje się skrypt **gnome-autogen.sh**.

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
Moduł obsługuje słowniki angielsko-polski oraz polsko-angielski. Niezbędne jest posiadania dwóch plików z już zainstalowanej aplikacji pod Windows. Są to pliki '**angpol.win**' oraz '**polang.win**'. Program powinien obsłużyć również starsze słowniki z roku 2003/2004, ale nie jest to przetestowane.

### Słownik DEPL
Jest to darmowy słownik niemiecko-polski. Aby słownik działał należy wejść na stronę www.depl.pl i pobrać aktualną bazę wyrazów lub w konsoli wpisać polecenie: wget http://www.depl.pl/wyrazy.zip
Z archiwum **wyrazy.zip** bierzemy pliki **a.dat** i **b.dat**.

## Konfiguracja programu Gydict

### Konfiguracja ścieżek do słowników.
Tu mamy dwie możliwości. Możemy umieścić pliki w domyślnym katalogu tj. **/opt/gydict/data** lub ustawić własne ścieżki do słowników. Aby skorzystać z drugiej możliwości, należy uruchomić program gydict, potem wejść w menu i wybrać **Preferencje->Słowniki** oraz odznaczyć opcję **Użyj domyślnych** i następnie ustawiać odpowiednią ścieżkę wykorzystując do tego odpowiedni przycisk.

### Konfiguracja czcionki.
Aby zmienić domyślną czcionkę, należy uruchomić program **Gydict**, potem wejść w menu i wybrać **Preferencje->Czcionki** oraz odznaczyć opcję **Użyj domślnych czcionek** i następnie wybrać odpowiednią czcionkę.

## Kontakt
|             |                          |
| :----:      | :----:                   |
| Author      | Jakub Czartek            |
| Email       | kuba@linux.pl            |
| Home Page   | http://jczartek.github.io/gydict |
| License     | GNU General Public License, version 2 (http://www.gnu.org/licenses/old-licenses/gpl-2.0.html) |

