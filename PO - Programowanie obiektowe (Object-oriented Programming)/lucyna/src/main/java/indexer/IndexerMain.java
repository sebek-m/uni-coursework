package indexer;

import org.apache.log4j.Level;
import org.apache.log4j.Logger;
import org.apache.lucene.store.Directory;
import shared.Utils;

import java.io.IOException;
import java.nio.file.Path;

public class IndexerMain {
    public static void main(String[] args) {
        // Wyłączamy logging, który ma miejsce w klasie OptimaizeLangDetector z biblioteki tika.
        Logger.getRootLogger().setLevel(Level.OFF);

        Path indexDir = Utils.createIndexFolder();

        Directory indexDirectory = Utils.openIndex(indexDir);

        IndexerMode action = null;
        ArgumentDetector detector = new ArgumentDetector();

        if (args.length == 0)
            try {
                action = new IndexerDefault();
            }
            catch (IOException e) {
                System.err.println("An unexpected critical I/O error has occurred.");
                System.exit(1);
            }
        else {
            try {
                // Decydujemy jaka czynność ma zostać podjęta
                action = detector.recognizeArgument(args);
            }
            catch (ArgumentDetector.InvalidArgException e) {
                System.err.println("Critical error: The given argument(s) was/were invalid.");
                System.exit(1);
            }
            catch (IOException e) {
                System.err.println("Critical error: A problem has occurred while reading the given path.");
                System.exit(1);
            }
        }

        // Wykonujemy czynność.
        action.perform(indexDirectory);
    }
}
