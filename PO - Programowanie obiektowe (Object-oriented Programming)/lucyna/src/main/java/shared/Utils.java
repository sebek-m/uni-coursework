package shared;

import org.apache.lucene.analysis.Analyzer;
import org.apache.lucene.analysis.en.EnglishAnalyzer;
import org.apache.lucene.analysis.miscellaneous.PerFieldAnalyzerWrapper;
import org.apache.lucene.analysis.pl.PolishAnalyzer;
import org.apache.lucene.analysis.standard.StandardAnalyzer;
import org.apache.lucene.index.DirectoryReader;
import org.apache.lucene.index.IndexReader;
import org.apache.lucene.store.Directory;
import org.apache.lucene.store.FSDirectory;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.HashMap;
import java.util.Map;

/**
 * Klasa z metodami przydatnymi zarówno w indekserze, jak i w wyszukiwarce
 */
public abstract class Utils {
    /**
     * Zwraca ścieżkę z indexem.
     */
    public static Path getIndexPath() {
        return Paths.get(System.getProperty("user.home"), ".index");
    }

    /**
     * Jeśli nie istnieje, tworzy folder ".index" w katalogu domowym. Zwraca
     * obiekt klasy Path, odpowiadający ścieżce z indeksem.
     */
    public static Path createIndexFolder() {
        Path indexDir = getIndexPath();

        if (!Files.exists(indexDir)) {
            try {
                Files.createDirectories(indexDir);
            }
            catch (IOException e) {
                System.err.println("Critical error: Failed to create a new index");
                System.exit(1);
            }
        }

        return indexDir;
    }

    /**
     * Zwraca obiekt klasy Directory, który odpowiada ścieżce z indexem.
     */
    public static Directory openIndex(Path indexPath) {
        Directory indexDirectory = null;

        try {
            indexDirectory = FSDirectory.open(indexPath);
        }
        catch (IOException e) {
            System.err.println("Critical error: Could not access the index directory");
            System.exit(1);
        }

        return indexDirectory;
    }

    /**
     * Tworzy IndexReader do odpowiedniego Directory.
     */
    public static IndexReader getReader(Directory indexDir) {
        IndexReader reader = null;

        try {
            reader = DirectoryReader.open(indexDir);

        }
        catch (IOException e) {
            System.err.println("Critical error: Could not access the index directory or a low-level I/O error occurred");
            System.exit(1);
        }

        return reader;
    }

    /**
     * Tworzy uogólniony Analyzer, który sam wie, które pole ma analizować w jaki sposób.
     */
    public static PerFieldAnalyzerWrapper createAnalyzer() {
        Map<String, Analyzer> analyzerMap = new HashMap<>();
        analyzerMap.put("polishName", new PolishAnalyzer());
        analyzerMap.put("polishContents", new PolishAnalyzer());
        analyzerMap.put("englishName", new EnglishAnalyzer());
        analyzerMap.put("englishContents", new EnglishAnalyzer());
        analyzerMap.put("standardName", new StandardAnalyzer());
        analyzerMap.put("standardContents", new StandardAnalyzer());

        return new PerFieldAnalyzerWrapper(new StandardAnalyzer(), analyzerMap);
    }

    /**
     * Zwraca pełną nazwę języka na podstawie kodu języka, którym posługuje się
     * OptimaizeLangDetector.
     */
    public static String recognizeLangName(String langCode) {
        String langName;

        switch (langCode) {
            case "pl":
                langName = "polish";
                break;
            case "en":
                langName = "english";
                break;
            default:
                langName = "standard";
                break;
        }

        return langName;
    }
}
