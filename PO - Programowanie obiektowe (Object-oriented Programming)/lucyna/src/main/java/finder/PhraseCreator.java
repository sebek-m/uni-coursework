package finder;

import org.apache.lucene.index.Term;
import org.apache.lucene.search.*;

import java.io.IOException;

/**
 * Klasa tworząca PhraseQuery do zadanego napisu
 */
public class PhraseCreator implements QueryCfg {
    public PhraseCreator() {}

    public Query createQuery(String queryStr, String languagePref) throws IOException {
        String[] split = queryStr.toLowerCase().split(" ");
        String nameField = languagePref + "Name";
        String contentsField = languagePref + "Contents";

        PhraseQuery.Builder builder1 = new PhraseQuery.Builder();
        for (String term : split)
            builder1.add(new Term(nameField, term));

        Query nameQ = builder1.build();

        PhraseQuery.Builder builder2 = new PhraseQuery.Builder();
        for (String term : split)
            builder2.add(new Term(contentsField, term));

        Query contentsQ = builder2.build();

        /* Użycie QueryParsera dla fraz, tak, jak poniżej, dawałoby lepsze wyniki,
         * ale w treści było sprecyzowane, żeby użyć PhraseQuery. ("analyzer" byłby
         * atrybutem tej klasy i byłby po prostu analyzerem otrzymywanym przez Utils.createAnalyzer())
         *
         *   QueryParser nameParser = new QueryParser(nameField, analyzer);
         *   QueryParser contentsParser = new QueryParser(contentsField, analyzer);
         *
         *   Query contentsQ;
         *   Query nameQ;
         *   try {
         *       nameQ = nameParser.parse(queryStr);
         *       contentsQ = contentsParser.parse(queryStr);
         *   } catch (ParseException e) {
         *       throw new IOException();
         *   }
         *
         */

        return new BooleanQuery.Builder()
                .add(nameQ, BooleanClause.Occur.SHOULD)
                .add(contentsQ, BooleanClause.Occur.SHOULD)
                .build();
    }
}
