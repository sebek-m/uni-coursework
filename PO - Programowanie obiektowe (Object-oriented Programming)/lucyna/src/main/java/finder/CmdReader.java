package finder;

import org.apache.lucene.store.Directory;

import java.io.IOException;
import java.util.Scanner;

/**
 * Czyta i interpretuje komendy podawane przez użytkownika do wyszukiwarki.
 */
public class CmdReader {
    private Scanner scanner;
    private FinderConfig cfg;

    public static class InvalidCmdException extends RuntimeException {}

    public CmdReader(FinderConfig cfg) {
        scanner = new Scanner(System.in);
        this.cfg = cfg;
    }

    /**
     * Rozpoznaje komendę i, jeśli jest poprawna, uruchamia ją.
     */
    public void recognizeAndRunCommand(Directory indexDir) {
        String cmd = scanner.nextLine();
        String[] splitCmd = cmd.split(" ");

        // Sprawdzamy, czy dane wejściowe są komendą, czy zapytaniem.
        if (splitCmd[0].startsWith("%")) {
            try {
                cfg.changeConfig(splitCmd);
            }
            catch (InvalidCmdException e) {
                System.err.println("Invalid command");
            }
        }
        else {
            QueryFinder finder = new QueryFinder(cmd, cfg, indexDir);

            try {
                finder.searchAndPrint();
            }
            catch (IOException e) {
                System.err.println("An error has occurred while searching for the given query");
            }
        }
    }
}
