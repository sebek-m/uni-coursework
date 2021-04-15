public class Przedmiot {
    private int cena;
    private String krajPochodzenia;
    private int rokProdukcji;

    public Przedmiot(int cena, String krajPochodzenia, int rokProdukcji) {
        this.cena = cena;
        this.krajPochodzenia = krajPochodzenia;
        this.rokProdukcji = rokProdukcji;
    }

    public int getCena() {
        return cena;
    }

    public int getRokProdukcji() {
        return rokProdukcji;
    }

    public String getKrajPochodzenia() {
        return krajPochodzenia;
    }

    public String toString() {
        return "Przedmiot z " + rokProdukcji + ", pochodzący z kraju " + krajPochodzenia +
                ". Cena sprzedaży: " + cena;
    }
}
