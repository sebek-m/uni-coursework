import java.util.ArrayList;

public abstract class Rolnik {
    protected String imię;
    protected ArrayList<Warzywo> zebraneWarzywa = new ArrayList<>();
    protected int sumaKosztów;
    protected int sumaZysków;

    protected abstract void simulate(Ogród ogród, int czas);

    protected abstract String odmienionaNazwa();

    protected void zbierzWarzywo(Warzywo warzywo) {
        if (warzywo != null) {
            this.zebraneWarzywa.add(warzywo);
            coZebrałem(warzywo);
            this.sumaZysków += warzywo.getWartośćPrzyZebraniu();
        }
    }

    protected void zbierzWszystko(Ogród ogród) {
        for (int i = 0; i < ogród.getLiczbaMiejsc(); i++) {
            zbierzWarzywo(ogród.wydajWarzywo(i));
        }
    }

    protected void zasadźWarzywo(Ogród ogród, Warzywo warzywo, int pozycja) {
        if (ogród.przyjmijWarzywo(warzywo, pozycja)) {
            coZasadziłem(warzywo);
            this.sumaKosztów += warzywo.getCenaZasadzenia();
        }
    }

    public static void czekaj(long sec) {
        try {
            Thread.sleep(sec * 1000);
        }
        catch(InterruptedException e) {
            System.out.println(e);
        }
    }

    private void coZebrałem(Warzywo warzywo) {
        System.out.println(this.imię + ": Zebrałem " + warzywo.odmienionaNazwa() + "! " + warzywo.wartośćString());
    }

    private void coZasadziłem(Warzywo warzywo) {
        System.out.println(this.imię + ": Zasadziłem " + warzywo.odmienionaNazwa() +
                "! Cena: " + warzywo.getCenaZasadzenia() + " PLN");
    }

    protected void raportSezonu(long długośćSezonu) {
        if (długośćSezonu > 0) System.out.println();
        System.out.println("Raport " + this.odmienionaNazwa() + " " + this.imię +
                " z minionego sezonu o długości " + długośćSezonu/1000 + " sekund:\n");

        System.out.println("Lista zebranych warzyw:");
        for(Warzywo warzywo : zebraneWarzywa) {
            System.out.println("- " + warzywo.toString());
        }

        System.out.println("\nSuma kosztów: " + sumaKosztów);
        System.out.println("Suma zysków: " + sumaZysków);
    }

    protected void zasadźWszystko(Ogród ogród) {
        for (int i = 0; i < ogród.getLiczbaMiejsc(); i++) {
            Warzywo warzywoDoZasadzenia = Warzywo.losoweWarzywo();
            zasadźWarzywo(ogród, warzywoDoZasadzenia, i);
        }
    }

    protected void przygotujOgród(Ogród ogród, int czekanieNaStart, long długośćSezonu) {
        if (długośćSezonu > 0) {
            zasadźWszystko(ogród);
            czekaj(czekanieNaStart);
        }
    }
}

class PracownikPGR extends Rolnik {
    public PracownikPGR(String imię_) {
        this.imię = imię_;
    }

    public String odmienionaNazwa() {
        return "pracownika PGR";
    }

    public void simulate(Ogród ogród, int czas) {
        long długośćSezonu = czas * 1000;
        long czasStartu = System.currentTimeMillis();
        long czasTrwania = 0;

        przygotujOgród(ogród, 10, długośćSezonu);

        while (czasTrwania < długośćSezonu) {
            zbierzWszystko(ogród);
            zasadźWszystko(ogród);
            czekaj(10);

            czasTrwania = System.currentTimeMillis() - czasStartu;
        }

        raportSezonu(długośćSezonu);
    }
}

class Gospodarz extends Rolnik {
    private int[] poprzednieWartościWarzyw;

    public Gospodarz(String imię_, int rozmiarPola) {
        this.imię = imię_;
        this.poprzednieWartościWarzyw = new int[rozmiarPola];
    }

    public String odmienionaNazwa() {
        return "gospodarza";
    }

    private boolean wartośćZmalała(int aktualnaWartość, int pozycjaWarzywa) {
        return aktualnaWartość < poprzednieWartościWarzyw[pozycjaWarzywa];
    }

    private void podmieńWarzywa(Ogród ogród, int pozycja) {
        zbierzWarzywo(ogród.wydajWarzywo(pozycja));
        Warzywo warzywoDoZasadzenia = Warzywo.losoweWarzywo();
        zasadźWarzywo(ogród, warzywoDoZasadzenia, pozycja);
    }

    private void przejrzyjOgród(Ogród ogród) {
        for (int i = 0; i < ogród.getLiczbaMiejsc(); i++) {
            int wartość = ogród.podajWartośćWarzywa(i);

            if (poprzednieWartościWarzyw[i] != -1 && wartośćZmalała(wartość, i))
                podmieńWarzywa(ogród, i);

            poprzednieWartościWarzyw[i] = wartość;
        }
    }

    public void simulate(Ogród ogród, int czas) {
        long długośćSezonu = czas * 1000;
        long czasStartu = System.currentTimeMillis();
        long czasTrwania = 0;

        przygotujOgród(ogród, 1, długośćSezonu);

        while (czasTrwania < długośćSezonu) {
            przejrzyjOgród(ogród);
            czekaj(1);

            czasTrwania = System.currentTimeMillis() - czasStartu;
        }

        raportSezonu(długośćSezonu);
    }
}

class Szczęściarz extends Rolnik {
    private Warzywo najlepszeWarzywo;
    private int maxWartość;
    private int optymalnyCzas;
    private boolean czyLosowo;

    public Szczęściarz(String imię_) {
        this.imię = imię_;
        reset();
    }

    public String odmienionaNazwa() {
        return "szczęściarza";
    }

    private void reset() {
        this.najlepszeWarzywo = null;
        this.maxWartość = 0;
        this.czyLosowo = true;
    }

    private void nowyFaworyt(Warzywo warzywo, int wartość) {
        this.maxWartość = wartość;
        this.optymalnyCzas = Warzywo.millisToSecInt(System.currentTimeMillis() - warzywo.getCzasZasadzenia()) +
                Warzywo.generator.nextInt(6);
        this.najlepszeWarzywo = warzywo;
        this.czyLosowo = false;
    }

    private void analizaIZbieranie(Ogród ogród) {
        for (int i = 0; i < ogród.getLiczbaMiejsc(); i++) {
            Warzywo zebrane = ogród.wydajWarzywo(i);
            zbierzWarzywo(zebrane);
            int wartość = zebrane.getWartośćPrzyZebraniu();

            if (wartość >= this.maxWartość)
                nowyFaworyt(zebrane, wartość);
        }
    }

    private boolean gorszePlony(int aktualnyWynik, int poprzedniWynik) {
        return poprzedniWynik > aktualnyWynik;
    }

    private void sadźWgWzorca(Ogród ogród) {
        for (int i = 0; i < ogród.getLiczbaMiejsc(); i++) {
            Warzywo doZasadzenia = najlepszeWarzywo.noweToSamo();
            zasadźWarzywo(ogród, doZasadzenia, i);
        }
    }

    private void sadzenie(Ogród ogród, int standardowyCzas) {
        if (czyLosowo) {
            reset();
            zasadźWszystko(ogród);
            czekaj(standardowyCzas);
        }
        else {
            sadźWgWzorca(ogród);
            czekaj(optymalnyCzas);
        }
    }

    public void simulate(Ogród ogród, int czas) {
        long długośćSezonu = czas * 1000;
        long czasStartu = System.currentTimeMillis();
        long czasTrwania = 0;
        final int standardowyCzas = Warzywo.generator.nextInt(czas/2);
        int poprzedniStanZysków = 0;
        int poprzedniWynik = 0;
        int wynikTury = 0;

        przygotujOgród(ogród, standardowyCzas, długośćSezonu);

        while (czasTrwania < długośćSezonu) {
            analizaIZbieranie(ogród);
            wynikTury = sumaZysków - poprzedniStanZysków;

            if (gorszePlony(wynikTury, poprzedniWynik))
                czyLosowo = true;

            sadzenie(ogród, standardowyCzas);

            poprzedniStanZysków = sumaZysków;
            poprzedniWynik = wynikTury;
            czasTrwania = System.currentTimeMillis() - czasStartu;
        }

        raportSezonu(długośćSezonu);
    }
}