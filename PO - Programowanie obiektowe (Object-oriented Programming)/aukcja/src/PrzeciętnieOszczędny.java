public class PrzeciętnieOszczędny extends Oszczędny {
    public PrzeciętnieOszczędny(String pseudonim, int budżet) {
        this.pseudonim = pseudonim;
        this.budżet = budżet;

        this.mnożnikŚredniej = 1;
    }
}