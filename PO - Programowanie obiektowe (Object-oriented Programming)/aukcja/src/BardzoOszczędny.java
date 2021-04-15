public class BardzoOszczędny extends Oszczędny {
    public BardzoOszczędny(String pseudonim, int budżet) {
        this.pseudonim = pseudonim;
        this.budżet = budżet;

        this.mnożnikŚredniej = 0.1f;
    }
}