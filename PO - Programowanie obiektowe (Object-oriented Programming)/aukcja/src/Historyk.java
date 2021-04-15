public class Historyk extends UczestnikAukcji {
    private int minRokProdukcji;
    private int maxRokProdukcji;

    public Historyk(String pseudonim, int budżet, int minRokProdukcji, int maxRokProdukcji) {
        this.pseudonim = pseudonim;
        this.budżet = budżet;
        this.minRokProdukcji = minRokProdukcji;
        this.maxRokProdukcji = maxRokProdukcji;
    }

    @Override
    protected boolean decyzja(Przedmiot przedmiot) {
        int rokProdukcji = przedmiot.getRokProdukcji();

        return minRokProdukcji <= rokProdukcji && rokProdukcji <= maxRokProdukcji;
    }
}
