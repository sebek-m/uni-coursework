import java.util.Random;

public class Losowy extends  UczestnikAukcji {
    private static final Random generator = new Random();

    public Losowy(String pseudonim, int budżet) {
        this.pseudonim = pseudonim;
        this.budżet = budżet;
    }

    @Override
    protected boolean decyzja(Przedmiot przedmiot) {
        return generator.nextBoolean();
    }
}
