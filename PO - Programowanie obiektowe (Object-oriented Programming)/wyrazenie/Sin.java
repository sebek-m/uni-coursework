public class Sin extends Jednoargumentowe {
    public Sin(Wyrażenie arg) {
        this.arg = arg;
    }

    protected float policz2(float x) {
        return (float) Math.sin(x);
    }

    public String toString() {
        return "sin(" + arg.toString() + ")";
    }
}
