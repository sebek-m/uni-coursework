package finder;

import org.apache.lucene.store.Directory;
import shared.Utils;

/**
 * Główna klasa wyszukiwarki
 */
public class FinderMain {
    public static void main(String[] args) {
        FinderConfig cfg = new FinderConfig();
        CmdReader reader = new CmdReader(cfg);
        Directory indexDir = Utils.openIndex(Utils.getIndexPath());

        while (true) {
            System.out.print("> ");
            reader.recognizeAndRunCommand(indexDir);
        }
    }
}
