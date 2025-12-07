package Tool_Classes;

/**
 * Represents a single line in a file
 * We store:
    * The line number in the file (1-based)
    * The original text
    * The normalized text (used for matching)
 */

public class LineRecord {

    private final int lineNumber;
    private final String originalText;
    private final String normalizedText;

    public LineRecord(int lineNumber, String originalText, String normalizedText) {
        this.lineNumber = lineNumber;
        this.originalText = originalText;
        this.normalizedText = normalizedText;
    }

    public int getLineNumber() {
        return lineNumber;
    }

    public String getOriginalText() {
        return originalText;
    }

    public String getNormalizedText() {
        return normalizedText;
    }
}

