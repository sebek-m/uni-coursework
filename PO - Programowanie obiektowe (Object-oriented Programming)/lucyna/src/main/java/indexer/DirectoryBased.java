package indexer;

import java.nio.file.Path;

/**
 * Klasa, pod którą podlegają podklasy wykonujące komendy, do których podana jest jakaś ścieżka.
 */
public abstract class DirectoryBased extends Writers {
    protected Path directory;
}
