public class Licytator {
    public Licytator(){
    }

    private UczestnikAukcji spytajUczestników(Przedmiot przedmiot, UczestnikAukcji[] uczestnicy) {
        UczestnikAukcji kupiec = null;

        for (int i = 0; i < uczestnicy.length && kupiec == null; i++) {
            UczestnikAukcji kandydat = uczestnicy[i];

            if (kandydat.możeWeź(przedmiot))
                kupiec = kandydat;
        }

        return kupiec;
    }

    public UczestnikAukcji[] przeprowadź(Przedmiot[] przedmioty, UczestnikAukcji[] uczestnicy) {
        UczestnikAukcji[] wygrani = new UczestnikAukcji[przedmioty.length];

        for (int i = 0; i < przedmioty.length; i++)
            wygrani[i] = spytajUczestników(przedmioty[i], uczestnicy);

        return wygrani;
    }

    private void przekaż(Przedmiot przedmiot, UczestnikAukcji kupiec) {
        kupiec.przyjmij(przedmiot);
    }

    public void rozdaj(Przedmiot[] przedmioty, UczestnikAukcji[] wygrani) {
        for (int i = 0; i < wygrani.length; i++) {
            UczestnikAukcji aktualny = wygrani[i];

            if (aktualny != null) {
                przekaż(przedmioty[i], aktualny);
                przedmioty[i] = null;
            }
        }
    }

    public void raport(UczestnikAukcji[] uczestnicy) {
        for (UczestnikAukcji uczestnik : uczestnicy) {
            System.out.println("Przedmioty zakupione przez uczestnika " + uczestnik.getPseudonim() + ":");
            System.out.println(uczestnik.toString());
        }
    }
}
