public class Ogród {
    private int liczbaMiejsc;
    private Warzywo[] grządka;

    public Ogród(int liczbaMiejsc_) {
        this.liczbaMiejsc = liczbaMiejsc_;
        this.grządka = new Warzywo[liczbaMiejsc_];
    }

    public int getLiczbaMiejsc() {
        return this.liczbaMiejsc;
    }

    public Warzywo wydajWarzywo(int pozycja) {
        if (grządka[pozycja] == null) {
            System.out.println("Na podanej pozycji nie ma warzywa, nie ma czego zebrać.");
            return null;
        }

        Warzywo doZebrania = grządka[pozycja];
        doZebrania.setWartośćPrzyZebraniu(doZebrania.obliczWartość());

        grządka[pozycja] = null;

        return doZebrania;
    }

    public boolean przyjmijWarzywo(Warzywo warzywo, int pozycja) {
        if (grządka[pozycja] != null) {
            System.out.println("Na podanej pozycji jest już warzywo, nie można nic zasadzić.");
            return false;
        }
        else {
            grządka[pozycja] = warzywo;
            return true;
        }
    }

    public int podajWartośćWarzywa(int pozycja) {
        if (grządka[pozycja] == null) {
            System.out.println("Na podanej pozycji nie ma warzywa, nie można sprawdzić wartości.");
            return -1;
        }

        return grządka[pozycja].obliczWartość();
    }


}