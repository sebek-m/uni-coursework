import java.util.Random;

public class Main {

    public static void main(String[] args) {
        UczestnikAukcji[] uczestnicy = new UczestnikAukcji[6];

        uczestnicy[0] = new Historyk("Bolek", 12500, 1300, 1776);
        uczestnicy[1] = new Losowy("Kozak2005", 30000);
        uczestnicy[2] = new Sentymentalny("Karpiński", 11000, "Polska");
        uczestnicy[3] = new SentymentalnyLimitowy("José", 27000, "Kolumbia", 10);
        uczestnicy[4] = new BardzoOszczędny("Spryciarz.pl", 4000);
        uczestnicy[5] = new PrzeciętnieOszczędny("Żyła", 5000);

        Przedmiot[] przedmioty = new Przedmiot[100];
        Random generator = new Random();

        for (int i = 0; i < przedmioty.length; i++) {
            int cena = generator.nextInt(7000) + 100;
            int rokProdukcji = generator.nextInt(2019) + 1;

            if (i < 20)
                przedmioty[i] = new Przedmiot(cena, "Afganistan", rokProdukcji);
            else if (i < 40)
                przedmioty[i] = new Przedmiot(cena, "Japonia", rokProdukcji);
            else if (i < 45)
                przedmioty[i] = new Przedmiot(cena, "Polska", rokProdukcji);
            else if (i < 60)
                przedmioty[i] = new Przedmiot(cena, "Norwegia", rokProdukcji);
            else if (i < 80)
                przedmioty[i] = new Przedmiot(cena, "Indie", rokProdukcji);
            else if (i < 89)
                przedmioty[i] = new Przedmiot(cena, "Kolumbia", rokProdukcji);
            else if (i < 100)
                przedmioty[i] = new Przedmiot(cena, "USA", rokProdukcji);
        }

        Licytator licytator = new Licytator();

        UczestnikAukcji[] wygrani = licytator.przeprowadź(przedmioty, uczestnicy);

        licytator.rozdaj(przedmioty, wygrani);

        licytator.raport(uczestnicy);
    }
}
