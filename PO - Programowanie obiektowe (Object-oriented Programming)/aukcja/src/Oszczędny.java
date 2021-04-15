public abstract class Oszczędny extends UczestnikAukcji {
    private int średniaCena;
    private int liczbaZaproponowanych = 0;
    private int poprzedniaSuma = 0;
    protected float mnożnikŚredniej;

    private void zaktualizujŚrednią(Przedmiot przedmiot) {
        poprzedniaSuma += przedmiot.getCena();
        liczbaZaproponowanych++;

        średniaCena = poprzedniaSuma / liczbaZaproponowanych;
    }

    private boolean tańszyOdŚredniej(Przedmiot przedmiot) {
        return przedmiot.getCena() <= średniaCena * mnożnikŚredniej;
    }

    @Override
    protected boolean decyzja(Przedmiot przedmiot) {
        boolean tańszy = tańszyOdŚredniej(przedmiot);

        zaktualizujŚrednią(przedmiot);

        return tańszy && liczbaZaproponowanych != 1;
    }
}
