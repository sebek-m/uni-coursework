package indexer;

import org.apache.lucene.store.Directory;

/**
 * Interfejs, który reprezentuje klasy, odpowiadające za poszczególne czynności,
 * które da się wykonać na indeksie.
 */
public interface IndexerMode {
    public void perform(Directory indexDir);
}
