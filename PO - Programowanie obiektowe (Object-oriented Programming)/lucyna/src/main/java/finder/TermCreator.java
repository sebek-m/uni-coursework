package finder;

import org.apache.lucene.index.Term;
import org.apache.lucene.search.BooleanClause;
import org.apache.lucene.search.BooleanQuery;
import org.apache.lucene.search.Query;
import org.apache.lucene.search.TermQuery;

/**
 * Klasa tworząca TermQuery do zadanego napisu
 */
public class TermCreator implements QueryCfg {
    public TermCreator() {}

    public Query createQuery(String queryStr, String languagePref) {
        Term name = new Term(languagePref + "Name", queryStr.toLowerCase());
        Term contents = new Term(languagePref + "Contents", queryStr.toLowerCase());

        Query nameQ = new TermQuery(name);
        Query contentsQ = new TermQuery(contents);

        return new BooleanQuery.Builder()
                .add(nameQ, BooleanClause.Occur.SHOULD)
                .add(contentsQ, BooleanClause.Occur.SHOULD)
                .build();
    }
}
