package indexer;

import org.apache.lucene.index.IndexWriter;
import org.apache.lucene.index.Term;
import org.apache.lucene.search.PrefixQuery;
import org.apache.lucene.search.Query;
import org.apache.lucene.search.TermQuery;

import java.io.File;
import java.io.IOException;
import java.nio.file.Path;

/**
 * Odpowiada za usunięcie dokumentów z indeksu, które odpowiadają plikom w danym katalogu.
 */
public class IndexerRemove extends DirectoryBased {
    public IndexerRemove(Path dir) {
        directory = dir;
    }

    public void modifyIndex(IndexWriter writer) throws IOException {
        Term documentTerm = new Term("canonicalPath", this.directory.toString() + File.separator);
        Query documentQuery = new PrefixQuery(documentTerm);

        Term monitoredTerm = new Term("monitoredPath", this.directory.toString());
        Query monitoredQuery = new TermQuery(monitoredTerm);

        writer.deleteDocuments(documentQuery, monitoredQuery);
    }
}
