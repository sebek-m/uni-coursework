package indexer;

import org.apache.lucene.search.*;
import org.apache.lucene.store.Directory;

import java.io.IOException;
import java.nio.file.Path;
import java.util.List;

/**
 * Usuwa pliki z indeksu i dokonuje ponownego indeksowania wszystkich dodanych wcześniej katalogów.
 */
public class IndexerReindex extends Readers {
    protected void readOrModifyIndex(IndexSearcher searcher,
                                     Query monitoredQuery,
                                     Directory indexDir) throws IOException {
        List<String> pathList = getPathList(searcher, monitoredQuery);

        IndexerPurge cleanIndex = new IndexerPurge();

        cleanIndex.perform(indexDir);

        for (String path : pathList) {
            Path newPath = ArgumentDetector.checkDirectory(path);
            IndexerAdd reIndex = new IndexerAdd(newPath);
            reIndex.perform(indexDir);
        }
    }
}
