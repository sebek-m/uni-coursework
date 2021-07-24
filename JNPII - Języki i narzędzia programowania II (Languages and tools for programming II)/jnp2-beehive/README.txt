Symulacja systemu automatycznej obsługi uli

- bee-camel-routes:
	- Moduł "lokalny"
	- Zawiera trasy Camela odpowiedzialne za:
		- Pobieranie prognozy pogody z OpenWeather
		- Wrzucanie rozkazów na podstawie prognozy do Kafki
		- Przerzucanie pomiarów z uli z Kafki do bazy danych
		- Przesyłanie wykresów, wygenerowanych za pośrednictwem
		  serwisu quickchart.io, na podstawie danych z uli,
		  w odpowiedzi na zapytania klienta.

- beehive-simulation
	- Moduł "zdalny" - w rzeczywistym systemie, każda instancja
	  byłaby uruchamiana na oddzielnym urządzeniu, znajdującym
	  się w poszczególnych ulach (w symulacji jest po jednym kontenerze
	  na ul, działającym na podstawie jednego obrazu).
	- Generuje dane losowe, symulujące pomiary czujników wewnątrz ula
	- Przesyła te dane do Kafki
	- Na podstawie tych danych przesyła rozkazy do trasy sterującej
	  za pośrednictwem Kafki (tego samego topica, co rozkazy z modułu
	  "lokalnego")
	- Trasa sterująca symuluje otwieranie i zamykanie wylotków w reakcji
	  na rozkazy z Kafki (poprzez wypiswanie na konsoli tego, co jest robione).

Przed uruchomieniem projektu komendą 'docker-compose up' z poziomu jego katalogu
głównego, należy zbudować (gradle clean build) oba powyższe moduły.

Należy też w pliku application.properties w podprojekcie bee-camel-routes dodać swój
klucz do serwisu OpenWeather.