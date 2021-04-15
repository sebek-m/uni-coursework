public class Razy extends Dwuargumentowe {
    public Razy(Wyrażenie arg1, Wyrażenie arg2) {
        this.arg1 = arg1;
        this.arg2 = arg2;
    }

    protected float policz2(float warArg1, float warArg2) {
        return warArg1 * warArg2;
    }

    protected char symbol() {
        return '*';
    }
}
