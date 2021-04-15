package rod;

/**
 * Klasa reprezentująca pręt. Jej podklasy są specjalnymi
 * rodzajami prętów, a klasa sama w sobie odpowiada tak naprawdę
 * za odcinki, na które są cięte kupione pręty
 */
public class Rod {
    protected int length;

    public Rod(int length) {
        this.length = length;
    }

    /**
     * @return długość prętu
     */
    public int getLength() {
        return this.length;
    }
}
