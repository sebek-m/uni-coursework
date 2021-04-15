package indexer;

import org.apache.lucene.document.Document;
import org.apache.lucene.search.IndexSearcher;
import org.apache.lucene.search.Query;
import org.apache.lucene.search.ScoreDoc;
import org.apache.lucene.search.TopDocs;
import org.apache.lucene.store.Directory;

import java.io.IOException;

/**
 * Odpowiada za wypisywanie listy katalogów dodanych do indeksu
 */
public class IndexerList extends Readers {
    protected void readOrModifyIndex(IndexSearcher searcher, Query monitoredQuery, Directory indexDir) throws IOException {
        TopDocs results = searcher.search(monitoredQuery, Integer.MAX_VALUE);
        ScoreDoc[] monitoredPaths = results.scoreDocs;

        for (ScoreDoc res : monitoredPaths) {
            int docId = res.doc;
            Document path = searcher.doc(docId);

            System.out.println(path.get("monitoredPath"));
        }
    }
}
