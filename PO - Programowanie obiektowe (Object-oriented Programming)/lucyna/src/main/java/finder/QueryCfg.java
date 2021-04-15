package finder;

import org.apache.lucene.search.Query;

import java.io.IOException;

public interface QueryCfg {
    /**
     * Tworzy Query odpowiedniego typu do zadanego napisu i języka.
     */
    public Query createQuery(String queryStr, String languagePref) throws IOException;
}
