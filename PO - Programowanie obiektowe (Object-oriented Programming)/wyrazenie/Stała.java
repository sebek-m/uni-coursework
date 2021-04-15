public class Stała extends Wyrażenie {
    private float wartość;

    public Stała(int wartość) {
        this.wartość = wartość;
    }

    public float policz(float x) {
        return wartość;
    }

    public String toString() {
        return String.valueOf(wartość);
    }

    public void zmieńNawiasy() {
    }
}
