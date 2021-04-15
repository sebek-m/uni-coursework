package finder;

import org.apache.lucene.search.Query;

import java.io.IOException;

import static shared.Utils.recognizeLangName;

/**
 * Klasa zawierająca konfigurację wyszukiwarki oraz kilka funkcji przydatnych
 * na różnych etapach procesu wyszukiwania.
 */
public class FinderConfig {
    private String language;
    private int limit;
    private boolean details;
    private boolean color;
    private QueryCfg query;

    public FinderConfig() {
        language = recognizeLangName("en");
        limit = Integer.MAX_VALUE;
        details = false;
        color = false;
        query = new TermCreator();
    }

    /**
     * Rozpoznaje komendę podaną przez użytkownika na wejście i zmienia odpowiednie ustawienie
     */
    public void changeConfig(String[] cmd) throws CmdReader.InvalidCmdException {
        switch (cmd[0]) {
            case "%lang":
                if (cmd.length != 2 || (!cmd[1].equals("en") && !cmd[1].equals("pl")))
                    throw new CmdReader.InvalidCmdException();

                this.language = recognizeLangName(cmd[1]);
                break;
            case "%details":
                checkSwitchArgs(cmd);
                this.details = cmd[1].equals("on");
                break;
            case "%limit":
                modifyLimit(cmd[1]);
                break;
            case "%color":
                checkSwitchArgs(cmd);
                this.color = cmd[1].equals("on");
                break;
            case "%term":
                checkLength(1, cmd);
                query = new TermCreator();
                break;
            case "%phrase":
                checkLength(1, cmd);
                query = new PhraseCreator();
                break;
            case "%fuzzy":
                checkLength(1, cmd);
                query = new FuzzyCreator();
                break;
            default:
                throw new CmdReader.InvalidCmdException();
        }
    }

    /**
     * Sprawdza, czy komenda jest poprawnym poleceniem, które przyjmuje opcje on/off
     */
    private void checkSwitchArgs(String[] cmd) throws CmdReader.InvalidCmdException {
        if (cmd.length != 2 || (!cmd[1].equals("on") && !cmd[1].equals("off")))
            throw new CmdReader.InvalidCmdException();
    }

    /**
     * Sprawdza, czy komenda ma zadaną ilość elementów
     */
    private void checkLength(int length, String[] cmd) throws CmdReader.InvalidCmdException {
        if (cmd.length != length)
            throw new CmdReader.InvalidCmdException();
    }

    /**
     * Ustawia nowy limit wyników wyszukiwania
     */
    private void modifyLimit(String string) throws CmdReader.InvalidCmdException {
        int val;

        try {
            val = Integer.parseInt(string);

            if (val < 0)
                throw new CmdReader.InvalidCmdException();
        }
        catch (NumberFormatException e) {
            throw new CmdReader.InvalidCmdException();
        }

        if (val == 0)
            this.limit = Integer.MAX_VALUE;
        else
            this.limit = val;
    }

    /**
     * Zwraca Query typu, który aktualnie jest ustawiony
     */
    public Query getProperQuery(String queryStr) throws IOException {
        return query.createQuery(queryStr, language);
    }

    public int getLimit() {
        return limit;
    }

    /**
     * Zwraca tablicę z odpowiednimi nazwami pól w przeglądanych dokumentach,
     * na podstawie ustawionego języka.
     */
    public String[] getProperFields() {
        return new String[]{language + "Name", language + "Contents"};
    }

    public boolean isColor() {
        return color;
    }

    public boolean isDetails() {
        return details;
    }
}
