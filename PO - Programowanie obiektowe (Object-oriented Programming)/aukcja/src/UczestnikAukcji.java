import java.util.ArrayList;

public abstract class UczestnikAukcji {
    protected String pseudonim;
    protected int budżet;
    private int sumaWydatków = 0;
    private ArrayList<Przedmiot> kupionePrzedmioty = new ArrayList<>();

    public String getPseudonim() {
        return pseudonim;
    }

    public String toString() {
        StringBuilder builder = new StringBuilder();

        for (Przedmiot przedmiot : kupionePrzedmioty)
            builder.append("- " + przedmiot.toString() + '\n');

        return builder.toString();
    }

    private void weź(Przedmiot przedmiot) {
        sumaWydatków += przedmiot.getCena();
    }

    public boolean możeWeź(Przedmiot przedmiot) {
        boolean starczyPieniędzy = przedmiot.getCena() + sumaWydatków <= budżet;
        boolean chcęKupić = decyzja(przedmiot);

        if (starczyPieniędzy && chcęKupić)
            weź(przedmiot);

        return starczyPieniędzy && chcęKupić;
    }

    public void przyjmij(Przedmiot przedmiot) {
        kupionePrzedmioty.add(przedmiot);
    }

    protected abstract boolean decyzja(Przedmiot przedmiot);
}
