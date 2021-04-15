package finder;

import org.apache.lucene.search.highlight.Encoder;

/**
 * Encoder, używany w highlighterze, który pozwala uniknąć wypisywania podwójnego
 * znaku nowej linii.
 */
public class CustomEncoder implements Encoder {
    public CustomEncoder() {
    }

    public String encodeText(String originalText) {
        return encodeGiven(originalText);
    }

    /**
     * Jeśli napotkane są dwa znaki nowej linii z rzędu, to dolącza do napisu tylko jedną.
     */
    private String encodeGiven(String plainText) {
        if (plainText != null && plainText.length() != 0) {
            StringBuilder result = new StringBuilder(plainText.length());
            boolean newLine = false;

            for(int index = 0; index < plainText.length(); ++index) {
                char ch = plainText.charAt(index);

                if (ch == '\n') {
                    if (!newLine) {
                        result.append(System.lineSeparator());
                        newLine = true;
                    }
                    else {
                        newLine = false;
                    }
                }
                else {
                    newLine = false;
                    result.append(ch);
                }
            }

            return result.toString();
        } else {
            return "";
        }
    }
}
