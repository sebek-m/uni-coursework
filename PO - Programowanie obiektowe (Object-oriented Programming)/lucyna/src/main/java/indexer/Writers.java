package indexer;

import org.apache.lucene.analysis.Analyzer;
import org.apache.lucene.index.IndexWriter;
import org.apache.lucene.index.IndexWriterConfig;
import org.apache.lucene.store.Directory;

import java.io.IOException;

import static shared.Utils.createAnalyzer;

/**
 * Klasa abstrakcyjna, której podklasami są tryby Indeksera, które głównie korzystają z IndexWritera.
 */
public abstract class Writers implements IndexerMode {
    public static IndexWriter createWriter(Directory indexDir) {
        Analyzer analyzer = createAnalyzer();
        IndexWriterConfig iwc = new IndexWriterConfig(analyzer);
        IndexWriter writer = null;

        iwc.setOpenMode(IndexWriterConfig.OpenMode.CREATE_OR_APPEND);

        try {
            writer = new IndexWriter(indexDir, iwc);
        }
        catch (IOException e) {
            System.err.println("Could not access the index directory or a low-level I/O error occurred");
            System.exit(1);
        }

        return writer;
    }

    /**
     * Wykonuje odpowiednie czynności na indeksie, na podstawie tego, jakiej jest podklasy.
     */
    protected abstract void modifyIndex(IndexWriter writer) throws IOException;

    public void perform(Directory indexDir) {
        IndexWriter writer = createWriter(indexDir);

        try {
           modifyIndex(writer);
        }
        catch (IOException e) {
            System.err.println("Could not access some directory or a low-level I/O error occurred");
        }
        finally {
            try {
                writer.commit();
                writer.close();
            }
            catch (IOException e) {
                System.err.println("A low-level I/O error has occurred while closing the IndexWriter");
            }
        }
    }
}
