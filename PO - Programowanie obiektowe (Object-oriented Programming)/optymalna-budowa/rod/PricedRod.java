package rod;

/**
 * Specjalny rodzaj pręta, który zawiera dodatkowo swoją cenę.
 * Zazwyczaj przechowywany jest w cenniku.
 */
public class PricedRod extends Rod {
    private int price;

    public PricedRod(int length, int price) {
        super(length);
        this.price = price;
    }

    /**
     * @return cena pręta
     */
    public int getPrice() {
        return this.price;
    }

    /**
     * "Przygotowuje pręt na sprzedaż", czyli tworzy
     * instancję pręta do cięcia o tej samej długości
     *
     * @return pręt do cięcia o tej samej długości
     */
    public UncutRod prepareForSale() {
        return new UncutRod(this.length);
    }
}
