package indexer;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;

/**
 * Klasa z metodami do rozpoznawania, co oznaczają podane argumenty.
 */
public class ArgumentDetector {

    public ArgumentDetector() {}

    public static class InvalidArgException extends RuntimeException {}

    /**
     * Sprawdza, czy podany napis jest ścieżką. Zwraca obiekt Path z tą ścieżką, jeśli tak.
     */
    public static Path checkDirectory(String pathStr) throws InvalidArgException, IOException {
        if (pathStr == null) {
            System.err.println("Directory not given.");
            throw new InvalidArgException();
        }

        Path dir = Paths.get(pathStr).toRealPath();
        if (!Files.isReadable(dir) || !Files.isDirectory(dir)) {
            System.err.println("Document directory '" + dir.toAbsolutePath() +
                    "' does not exist or is not readable, please check the path.");
            throw new InvalidArgException();
        }

        return dir;
    }

    /**
     * Rozpoznaje argument i tworzy odpowiedni obiekt IndexerMode, który odpowiada za dalszą
     * pracę indeksera.
     */
    public IndexerMode recognizeArgument(String[] args) throws InvalidArgException, IOException {
        IndexerMode mode;
        Path dir;

        switch (args[0]) {
            case "--purge":
                mode = new IndexerPurge();
                break;
            case "--add":
                dir = checkDirectory(args[1]);

                mode = new IndexerAdd(dir);
                break;
            case "--rm":
                dir = checkDirectory(args[1]);

                mode = new IndexerRemove(dir);
                break;
            case "--reindex":
                mode = new IndexerReindex();
                break;
            case "--list":
                mode = new IndexerList();
                break;
            default:
                throw new InvalidArgException();
        }

        return mode;
    }
}

