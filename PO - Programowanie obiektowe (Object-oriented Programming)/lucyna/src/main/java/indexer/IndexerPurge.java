package indexer;

import org.apache.lucene.index.IndexWriter;

import java.io.IOException;

/**
 * Odpowiada za usuwanie wszystkich dokumentów z indeksu
 */
public class IndexerPurge extends Writers {
    protected void modifyIndex(IndexWriter writer) throws IOException {
        writer.deleteAll();
    }
}
