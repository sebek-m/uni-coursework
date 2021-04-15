package finder;

import org.apache.lucene.analysis.Analyzer;
import org.apache.lucene.analysis.TokenStream;
import org.apache.lucene.document.Document;
import org.apache.lucene.index.IndexReader;
import org.apache.lucene.search.*;
import org.apache.lucene.search.highlight.*;
import org.apache.lucene.store.Directory;
import org.jline.terminal.Terminal;
import org.jline.terminal.TerminalBuilder;
import org.jline.utils.AttributedStringBuilder;
import org.jline.utils.AttributedStyle;
import shared.Utils;

import java.io.IOException;

/**
 * Klasa przeprowadzająca przeszukiwanie indeksu i wypisująca jego wyniki.
 */
public class QueryFinder {
    private String queryStr;
    private FinderConfig cfg;
    private Directory indexDir;

    /**
     * Ile maksymalnie fragmentów zostanie wyświetlonych w danym kontekście
     */
    private static final int maxNumFragments = 5;

    public QueryFinder(String queryStr, FinderConfig cfg, Directory indexDir) {
        this.queryStr = queryStr;
        this.cfg = cfg;
        this.indexDir = indexDir;
    }

    /**
     * Metoda nadrzędna, wywołuje funkcje szukania i wypisywania.
     */
    public void searchAndPrint() throws IOException {
        IndexReader reader = Utils.getReader(indexDir);

        IndexSearcher searcher = new IndexSearcher(reader);
        Query query = cfg.getProperQuery(queryStr);

        TopDocs hits = searcher.search(query, cfg.getLimit());

        print(hits, searcher, query);

        try {
            reader.close();
        }
        catch (IOException e) {
            System.err.println("An has occurred while closing the IndexReader");
        }
    }

    /**
     * Wypisuje wyniki w odpowiednim stylu
     */
    private void print(TopDocs hits, IndexSearcher searcher, Query query) {
        try (Terminal terminal = TerminalBuilder.builder()
                .jna(false)
                .jansi(true)
                .build()) {
            printHitNum(hits, terminal);

            if (!cfg.isDetails())
                onlyPrintPaths(hits, searcher, terminal);
            else
                printEveryThing(hits, query, searcher, terminal);
        }
        catch (IOException e) {
            System.err.println("An error has occurred while printing search results.");
        }
    }

    /**
     * W razie znalezienia większej liczby wyników niż określa to limit,
     * zwraca odpowiednią wartość, reprezentującą ilość wyników ograniczoną przez limit.
     */
    private long correctHitNumber(TopDocs hits) {
        long trulyFound = hits.totalHits.value;

        if (trulyFound > cfg.getLimit())
            return cfg.getLimit();
        else
            return trulyFound;
    }

    /**
     * Wypisuje liczbę wyników
     */
    private void printHitNum(TopDocs hits, Terminal terminal) {
        terminal.writer().println(new AttributedStringBuilder().style(AttributedStyle.DEFAULT)
                .append("Files count: ")
                .style(AttributedStyle.BOLD)
                .append(Long.toString(correctHitNumber(hits)))
                .toAnsi());
    }

    /**
     * Wypisuje ścieżkę pliku, w którym znaleziono zapytanie
     */
    private void printPath(Document doc, Terminal terminal, boolean details) {
        String filePath = doc.get("canonicalPath");

        if (details) {
            terminal.writer().println(new AttributedStringBuilder()
                    .style(AttributedStyle.BOLD)
                    .append(filePath)
                    .append(":")
                    .toAnsi());
        }
        else {
            terminal.writer().println(new AttributedStringBuilder()
                    .style(AttributedStyle.BOLD)
                    .append(filePath)
                    .toAnsi());
        }
    }

    /**
     * Metoda wypisywania w przypadku wyłączenia opcji "details"
     */
    private void onlyPrintPaths(TopDocs hits, IndexSearcher searcher, Terminal terminal) throws IOException {
        for (ScoreDoc scoreDoc : hits.scoreDocs) {
            int docid = scoreDoc.doc;
            Document doc = searcher.doc(docid);

            printPath(doc, terminal, cfg.isDetails());
        }
    }

    /**
     * Metoda wypisywania wraz z kontekstami (i, opcjonalnie, kolorami)
     */
    private void printEveryThing(TopDocs hits, Query query,
                                 IndexSearcher searcher,
                                 Terminal terminal) throws IOException {
        Analyzer analyzer = Utils.createAnalyzer();
        String[] fields = cfg.getProperFields();

        Highlighter highlighter = createHighLighter(query);

        for (int i = 0; i < hits.scoreDocs.length; i++) {
            int docId = hits.scoreDocs[i].doc;
            Document doc = searcher.doc(docId);

            printPath(doc, terminal, cfg.isDetails());

            String name = doc.get(fields[0]);
            String contents = doc.get(fields[1]);

            TokenStream nameStream = analyzer.tokenStream(fields[0], name);
            TokenStream contentsStream = analyzer.tokenStream(fields[1], contents);

            try {
                String[] nameFrags = highlighter.getBestFragments(nameStream, name, maxNumFragments);
                String[] contentsFrags = highlighter.getBestFragments(contentsStream, contents, maxNumFragments);

                printContext(nameFrags, contentsFrags, terminal);
            }
            catch (InvalidTokenOffsetsException e) {
                System.err.println("InvalidTokenOffsetsException while printing the context");
            }
        }
    }

    /**
     * Tworzy odpowiednio zmodyfikowany Highlighter
     */
    private Highlighter createHighLighter(Query query) {
        Formatter formatter = new JLineFormatter(cfg.isColor());

        QueryScorer scorer = new QueryScorer(query);

        Encoder encoder = new CustomEncoder();

        Highlighter highlighter = new Highlighter(formatter, encoder, scorer);

        Fragmenter fragmenter = new SimpleSpanFragmenter(scorer);

        highlighter.setTextFragmenter(fragmenter);

        return highlighter;
    }

    /**
     * Iteruje po znalezionych fragmentach, w których występuje zapytanie i je wypisuje,
     * oddzielając je linią myślników.
     */
    private void printContext(String[] nameFrags, String[] contentsFrags, Terminal terminal) {
        for (int j = 0; j < nameFrags.length; j++) {
            terminal.writer().println(nameFrags[j]);
            if (contentsFrags.length != 0 || j + 1 != nameFrags.length)
                System.out.println("-----------------------------");
        }

        for (int j = 0; j < contentsFrags.length; j++) {
            terminal.writer().println(contentsFrags[j]);
            if (j + 1 != contentsFrags.length)
                System.out.println("-----------------------------");
        }
    }
}
