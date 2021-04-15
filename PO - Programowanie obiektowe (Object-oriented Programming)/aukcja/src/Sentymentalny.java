public class Sentymentalny extends UczestnikAukcji {
    private String krajPochodzenia;

    public Sentymentalny(String pseudonim, int budżet, String krajPochodzenia) {
        this.pseudonim = pseudonim;
        this.budżet = budżet;
        this.krajPochodzenia = krajPochodzenia;
    }

    @Override
    protected boolean decyzja(Przedmiot przedmiot) {
        return przedmiot.getKrajPochodzenia() == this.krajPochodzenia;
    }
}
