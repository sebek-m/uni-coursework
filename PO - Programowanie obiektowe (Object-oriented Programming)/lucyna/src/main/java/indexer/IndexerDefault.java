package indexer;

import org.apache.lucene.document.Document;
import org.apache.lucene.index.IndexWriter;
import org.apache.lucene.index.Term;
import org.apache.lucene.search.IndexSearcher;
import org.apache.lucene.search.Query;
import org.apache.lucene.search.TermQuery;
import org.apache.lucene.store.Directory;

import java.io.IOException;
import java.nio.file.*;
import java.nio.file.attribute.BasicFileAttributes;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import static java.nio.file.StandardWatchEventKinds.ENTRY_CREATE;
import static java.nio.file.StandardWatchEventKinds.ENTRY_DELETE;
import static java.nio.file.StandardWatchEventKinds.ENTRY_MODIFY;

/**
 * Odpowiada za domyślną pracę indeksera, czyli monitorowanie katalogów.
 */
public class IndexerDefault extends Readers {
    private final WatchService watcher;
    private final Map<WatchKey, Path> keys;

    public IndexerDefault() throws IOException {
        this.watcher = FileSystems.getDefault()
                    .newWatchService();
        this.keys = new HashMap<WatchKey, Path>();
    }

    /**
     * Przygotowuje monitoring i go wykonuje
     */
    @Override
    protected void readOrModifyIndex(IndexSearcher searcher, Query monitoredQuery, Directory indexDir) throws IOException {
        List<String> pathStrList = getPathList(searcher, monitoredQuery);
        List<Path> pathList = convertPathList(pathStrList);

        for (Path path : pathList)
            registerAll(path);

        IndexWriter writer = Writers.createWriter(indexDir);

        processEvents(writer);

        try {
            writer.close();
        }
        catch (IOException e) {
            System.err.println("An I/O error has occurred while closing the writer.");
        }
    }

    /**
     * Konwertuje listę napisów na listę odpowiadających im ścieżek.
     */
    private List<Path> convertPathList(List<String> pathStrList) {
        List<Path> pathList = new ArrayList<>();

        for (String path : pathStrList)
            pathList.add(Paths.get(path));

        return pathList;
    }

    /**
     * Rejestruje pojedynczy katalog do WatchService.
     */
    private void register(Path dir) throws IOException {
        WatchKey key = dir.register(watcher, ENTRY_CREATE, ENTRY_DELETE, ENTRY_MODIFY);
        keys.put(key, dir);
    }

    /**
     * Rejestruje podany katalog i jego drzewo podkatalogów do WatchService.
     */
    private void registerAll(final Path start) throws IOException {
        Files.walkFileTree(start, new SimpleFileVisitor<Path>() {
            @Override
            public FileVisitResult preVisitDirectory(Path dir, BasicFileAttributes attrs) throws IOException {
                register(dir);
                return FileVisitResult.CONTINUE;
            }
        });
    }

    /**
     * Nieskończona pętla, monitorująca katalogi w indeksie.
     */
    private void processEvents(IndexWriter writer) {
        for (;;) {
            WatchKey key;
            try {
                // Czekamy na zdarzenie
                key = watcher.take();
            }
            catch (InterruptedException x) {
                System.err.println("An error has occurred while monitoring the directories.");
                return;
            }

            // Bierzemy katalog, w którym doszło do zdarzenia.
            Path dir = keys.get(key);
            if (dir == null)
                continue;

            // Czekamy, żeby uniknąć zduplikowania zdarzenia.
            try {
                Thread.sleep(2000);
            }
            catch (InterruptedException x) {
                System.err.println("An error has occurred while monitoring the directories.");
                return;
            }

            for (WatchEvent<?> event : key.pollEvents()) {
                WatchEvent.Kind<?> kind = event.kind();

                WatchEvent<Path> ev = cast(event);
                Path name = ev.context();
                Path child = dir.resolve(name);

                resolveEvent(child, writer, kind);
            }

            // Jeśli usunięty został jakiś katalog, zdejmujemy go też z mapy monitorowanych katalogów.
            boolean valid = key.reset();
            if (!valid) {
                keys.remove(key);

                // Jeśli nie ma już nic do monitorowania - być może nie powinno się tutaj kończyć pracy,
                // ponieważ w treści zakończenie działania indeksera miało być w momencie naciśnięcia
                // Ctrl+C, ale jest to chyba szczególny przypadek.
                if (keys.isEmpty()) {
                    break;
                }
            }
        }
    }

    /**
     * Wywołuje odpowiednie operacje na indeksie na podstawie typu zdarzenia
     */
    private void resolveEvent(Path child, IndexWriter writer, WatchEvent.Kind<?> kind) {
        try {
            IndexerAdd indexer = new IndexerAdd(child);
            if (kind == ENTRY_CREATE) {
                if (Files.isDirectory(child)) {
                    // Dodajemy wszystko z katalogu do indeksu i do monitoringu
                    registerAll(child);
                    indexer.indexAllFiles(writer, child);
                }
                else if (Files.isRegularFile(child)) {
                    // Dodajemy dokument do indeksu
                    indexer.indexDoc(writer, child);
                }
            }
            else if (kind == ENTRY_MODIFY) {
                if (Files.isRegularFile(child)) {
                    // Uaktualniamy dokument w indeksie
                    Document doc = indexer.fillDocument(child);

                    writer.updateDocument(new Term("canonicalPath", child.toString()), doc);
                }
            }
            else if (kind == ENTRY_DELETE) {
                // Tutaj po kolei będą zaliczały się zdarzenia do pojedynczych plików,
                // nawet należących do jakiegoś katalogu.
                Term documentTerm = new Term("canonicalPath", child.toString());
                Query documentQuery = new TermQuery(documentTerm);

                Term monitoredTerm = new Term("monitoredPath", child.toString());
                Query monitoredQuery = new TermQuery(monitoredTerm);

                writer.deleteDocuments(documentQuery, monitoredQuery);
            }

            writer.commit();
        }
        catch (IOException e) {
            System.err.println("An I/O error has occurred while watching the directories.");
        }
    }

    @SuppressWarnings("unchecked")
    static private <T> WatchEvent<T> cast(WatchEvent<?> event) {
        return (WatchEvent<T>) event;
    }
}
