public class Wymierna implements Algebraiczna<Wymierna> {
    private int numerator, denominator, sign;

    public Wymierna(int numerator_, int denominator_) {
        this.numerator = numerator_;
        this.denominator = denominator_;
        correctSign();
        cancellation();
    }

    public Wymierna(int integer) {
        this.numerator = integer;
        this.denominator = 1;
        correctSign();
    }

    public String toString() {
        return (this.sign * this.numerator) + "/" + this.denominator;
    }

    public int getNumerator() {
        return this.numerator;
    }

    public int getDenominator() {
        return this.denominator;
    }

    public int getSign() {
        return this.sign;
    }

    public void setSign(int sign_) {
        this.sign = sign_;
    }

    private void correctSign() {
        int sign = 1;

        if (this.numerator == 0) {
            sign = 0;
        }
        else if (this.numerator < 0) {
            this.numerator *= (-1);
            sign *= (-1);
        }

        if (this.denominator < 0) {
            this.denominator *= (-1);
            sign *= (-1);
        }

        this.sign = sign;
    }

    public static int greatestCommonDivisor(int x, int y) {
        if (x == 0)
            return y;

        return greatestCommonDivisor(y%x, x);
    }

    private void cancellation() {
        int commonDivisor = greatestCommonDivisor(this.numerator, this.denominator);

        this.numerator = this.numerator / commonDivisor;
        this.denominator = this.denominator / commonDivisor;
    }

    public static Wymierna zero() {
        return new Wymierna(0);
    }

    public static Wymierna one() {
        return new Wymierna(1);
    }

    public Wymierna opposite() {
        Wymierna tmp = new Wymierna(this.numerator, this.denominator);

        tmp.setSign((-1) * this.sign);

        return tmp;
    }

    public Wymierna inverse() {
        Wymierna tmp = new Wymierna(this.denominator, this.numerator);

        tmp.setSign(this.sign);

        return tmp;
    }

    public Wymierna dodaj(Wymierna arg) {
        int tmpNumerator = (arg.getDenominator() * this.numerator * this.sign) +
                (this.denominator * arg.getNumerator() * arg.getSign());
        int tmpDenominator = this.denominator * arg.getDenominator();

        Wymierna tmp = new Wymierna(tmpNumerator, tmpDenominator);

        return tmp;
    }

    public Wymierna subtract(Wymierna other) {
        return dodaj(other.opposite());
    }

    public Wymierna pomnóż(Wymierna arg) {
        int tmpNumerator = (this.numerator * this.sign) *
                (arg.getNumerator() * arg.getSign());
        int tmpDenominator = this.denominator * arg.getDenominator();

        Wymierna tmp = new Wymierna(tmpNumerator, tmpDenominator);

        return tmp;
    }

    public Wymierna divide(Wymierna other) {
        return pomnóż(other.inverse());
    }

    public int compareTo(Wymierna other) {
        if (this.sign > other.getSign() || this.sign < other.getSign())
            return (this.sign < other.getSign()) ? (-1) : (+1);

        int normalizedNumerator1 = other.getDenominator() * this.numerator * this.sign;
        int normalizedNumerator2 = this.denominator * other.getNumerator() * other.getSign();

        if (normalizedNumerator1 < normalizedNumerator2)
            return -1;
        else if (normalizedNumerator1 > normalizedNumerator2)
            return 1;
        else
            return 0;
    }
}