public class SentymentalnyLimitowy extends Sentymentalny {
    private int maxPrzedmiotów;
    private int liczbaPrzedmiotów;

    public SentymentalnyLimitowy(String pseudonim, int budżet, String krajPochodzenia, int maxPrzedmiotów) {
        super(pseudonim, budżet, krajPochodzenia);
        this.maxPrzedmiotów = maxPrzedmiotów;
        this.liczbaPrzedmiotów = 0;
    }

    @Override
    protected boolean decyzja(Przedmiot przedmiot) {
        boolean dobryKraj =  super.decyzja(przedmiot);
        boolean mamMiejsce = liczbaPrzedmiotów < maxPrzedmiotów;

        if (dobryKraj && mamMiejsce)
            liczbaPrzedmiotów++;

        return dobryKraj && mamMiejsce;
    }
}
