package finder;

import org.apache.lucene.index.Term;
import org.apache.lucene.search.BooleanClause;
import org.apache.lucene.search.BooleanQuery;
import org.apache.lucene.search.FuzzyQuery;
import org.apache.lucene.search.Query;

/**
 * Klasa tworząca FuzzyQuery do zadanego napisu
 */
public class FuzzyCreator implements QueryCfg {
    public FuzzyCreator() {}

    public Query createQuery(String queryStr, String languagePref) {
        Term name = new Term(languagePref + "Name", queryStr);
        Term contents = new Term(languagePref + "Contents", queryStr);

        Query nameQ = new FuzzyQuery(name);
        Query contentsQ = new FuzzyQuery(contents);

        return new BooleanQuery.Builder()
                .add(nameQ, BooleanClause.Occur.SHOULD)
                .add(contentsQ, BooleanClause.Occur.SHOULD)
                .build();
    }
}
