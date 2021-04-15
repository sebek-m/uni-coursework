package indexer;

import org.apache.lucene.document.Document;
import org.apache.lucene.index.*;
import org.apache.lucene.search.*;
import org.apache.lucene.store.Directory;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

/**
 * Klasa abstrakcyjna, której podklasami są tryby Indeksera, które głównie korzystają z IndexReadera.
 */
public abstract class Readers implements IndexerMode {
    public void perform(Directory indexDir) {
        IndexReader reader = null;

        try {
            reader = DirectoryReader.open(indexDir);

        }
        catch (IOException e) {
            System.err.println("Could not access the index directory or a low-level I/O error occurred");
            System.exit(1);
        }

        IndexSearcher searcher = new IndexSearcher(reader);
        // Query do znalezienia dokumentów w indeksie, których jedynym celem jest
        // przechowywanie ścieżek katalogów, które zostały dodane wcześniej do indeksu.
        Term monitoredTerm = new Term("monitoredPath", "*");
        Query monitoredQuery = new WildcardQuery(monitoredTerm);

        try {
            readOrModifyIndex(searcher, monitoredQuery, indexDir);
        }
        catch(IOException e) {
            System.err.println("Could not access some directory or a low-level I/O error occurred");
        }
        finally {
            try {
                reader.close();
            }
            catch (IOException e) {
                System.err.println("A low-level I/O error has occurred while closing the IndexReader");
            }
        }
    }

    protected abstract void readOrModifyIndex(IndexSearcher searcher,
                                              Query monitoredQuery,
                                              Directory indexDir) throws IOException;

    /**
     * Zwraca listę napisów, które reprezentują ścieżki dodane wcześniej do indeksu.
     */
    protected List<String> getPathList(IndexSearcher searcher, Query monitoredQuery) throws IOException {
        List<String> pathList = new ArrayList<>();

        TopDocs results = searcher.search(monitoredQuery, Integer.MAX_VALUE);
        ScoreDoc[] monitoredPaths = results.scoreDocs;

        for (ScoreDoc res : monitoredPaths) {
            int docId = res.doc;
            Document pathDoc = searcher.doc(docId);

            pathList.add(pathDoc.get("monitoredPath"));
        }

        return pathList;
    }
}
