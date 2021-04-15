public class Cos extends Jednoargumentowe {
    public Cos(Wyrażenie arg) {
        this.arg = arg;
    }

    protected float policz2(float x) {
        return (float) Math.cos(x);
    }

    public String toString() {
        return "cos(" + arg.toString() + ")";
    }
}
