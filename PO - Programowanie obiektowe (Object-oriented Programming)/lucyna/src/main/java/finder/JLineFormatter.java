package finder;

import org.apache.lucene.search.highlight.Formatter;
import org.apache.lucene.search.highlight.TokenGroup;
import org.jline.utils.AttributedStringBuilder;
import org.jline.utils.AttributedStyle;

/**
 * Formatter, który formatuje tekst przy użyciu biblioteki Jline, w sposób opisany w treści.
 */
public class JLineFormatter implements Formatter {
    private boolean color;

    public JLineFormatter(boolean color) {
        this.color = color;
    }

    public String highlightTerm(String originalText, TokenGroup tokenGroup) {
        if (tokenGroup.getTotalScore() <= 0.0F) {
            return originalText;
        } else {
            if (color) {
                return new AttributedStringBuilder()
                        .style(AttributedStyle.BOLD.foreground(AttributedStyle.RED))
                        .append(originalText)
                        .toAnsi();
            }
            else {
                return new AttributedStringBuilder()
                        .style(AttributedStyle.BOLD)
                        .append(originalText)
                        .toAnsi();
            }
        }
    }
}
