package indexer;

import org.apache.lucene.document.Document;
import org.apache.lucene.document.Field;
import org.apache.lucene.document.StringField;
import org.apache.lucene.document.TextField;
import org.apache.lucene.index.IndexWriter;
import org.apache.tika.exception.TikaException;
import org.apache.tika.langdetect.OptimaizeLangDetector;
import org.apache.tika.language.detect.LanguageDetector;
import org.apache.tika.language.detect.LanguageResult;
import org.apache.tika.metadata.Metadata;
import org.apache.tika.parser.AutoDetectParser;
import org.apache.tika.sax.BodyContentHandler;
import org.xml.sax.SAXException;

import java.io.IOException;
import java.io.InputStream;
import java.nio.file.*;
import java.nio.file.attribute.BasicFileAttributes;
import java.util.List;

import static shared.Utils.recognizeLangName;

/**
 * Klasa odpowiadająca za tryb indeksera z argumentem --add
 */
public class IndexerAdd extends DirectoryBased {
    public IndexerAdd(Path dir) {
        directory = dir;
    }

    /**
     * Wywołuje metody potrzebne do dodania dokumentów do indeksu.
     */
    protected void modifyIndex(IndexWriter writer) throws IOException {
        indexAllFiles(writer, this.directory);

        Document pathDoc = new Document();
        Field monitoredPathField = new StringField("monitoredPath",
                this.directory.toString(), Field.Store.YES);
        pathDoc.add(monitoredPathField);
        writer.addDocument(pathDoc);
    }

    /**
     * Indeksuje wszystkie pliki ze ścieżki dir
     */
    public void indexAllFiles(IndexWriter writer, Path dir) throws IOException {
        Files.walkFileTree(dir, new SimpleFileVisitor<Path>() {
            @Override
            public FileVisitResult visitFile(Path file, BasicFileAttributes attrs) {
                try {
                    if (Files.isRegularFile(file))
                        indexDoc(writer, file);
                }
                catch (IOException ignore) {
                    // Ignorujemy pliki, z których indeksowaniem jest problem.
                }
                return FileVisitResult.CONTINUE;
            }
        });
    }

    /**
     * Parsuje podany plik za pomocą Tiki
     */
    private String parseDocument(Path file) throws IOException, SAXException, TikaException {
        AutoDetectParser parser = new AutoDetectParser();
        BodyContentHandler handler = new BodyContentHandler();
        Metadata metadata = new Metadata();

        try (InputStream stream = Files.newInputStream(file)) {
            parser.parse(stream, handler, metadata);
            return handler.toString();
        }
    }

    /**
     * Rozpoznaje języki występujące w pliku.
     */
    private List<LanguageResult> recognizeLanguages(String contents) {
        LanguageDetector detector = new OptimaizeLangDetector().loadModels();
        detector.addText(contents);
        return detector.detectAll();
    }

    /**
     * Dodaje do dokumentu odpowiednie pola na podstawie wykrytego języka.
     */
    private void createProperFields(Document doc, String fieldPrefix, String contents, String fileName) {
        doc.add(new TextField(fieldPrefix + "Name", fileName, Field.Store.YES));
        doc.add(new TextField(fieldPrefix + "Contents", contents, Field.Store.YES));
    }

    /**
     * Indeksuje pojedynczy dokument.
     */
    public void indexDoc(IndexWriter writer, Path file) throws IOException {
        Document doc = fillDocument(file);

        if (doc != null)
            writer.addDocument(doc);
    }

    /**
     * Tworzy i wypełnia odpowiednie pola dokumentu
     */
    public Document fillDocument(Path file) throws IOException {
        Document doc = null;

        try {
            doc = new Document();

            Field canonicalPathField = new StringField("canonicalPath",
                    file.toAbsolutePath().normalize().toString(), Field.Store.YES);
            doc.add(canonicalPathField);

            String fileName = file.getFileName().toString();
            String contents = parseDocument(file);
            List<LanguageResult> presentLanguages = recognizeLanguages(contents);

            for (LanguageResult language : presentLanguages) {
                String fieldPrefix = recognizeLangName(language.getLanguage());
                createProperFields(doc, fieldPrefix, contents, fileName);
            }
        } catch (SAXException e) {
            System.err.println("SAXException has occurred while parsing the document: " + file.toString());
        } catch (TikaException e) {
            System.err.println("TikaException has occurred while parsing the document: " + file.toString());
        }

        return doc;
    }
}
