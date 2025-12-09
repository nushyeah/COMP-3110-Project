package Tool_Classes;

import java.util.*;
import java.util.stream.Collectors;

/**
 * Step 4: COMPUTE SIMILARITY
 *
 * Computes:
 *   - content similarity between two lines (based on token Jaccard)
 *   - context similarity between neighborhoods around two lines
 *   - combined similarity = 0.6 * content + 0.4 * context
 */
public class SimilarityCalculator { // this is for similarity calculation

    private final int contextWindow; // number of lines above/below to use as context

    public SimilarityCalculator(int contextWindow) { // we set the context window here
        this.contextWindow = contextWindow;
    }

    public double contentSimilarity(LineRecord oldLine, LineRecord newLine) { // to compute content similarity
        Set<String> tokens1 = tokenize(oldLine.getNormalizedText());
        Set<String> tokens2 = tokenize(newLine.getNormalizedText());
        return jaccard(tokens1, tokens2);
    }

    public double contextSimilarity(FileVersion oldFile, int oldLineNum, // to compute context similarity
                                    FileVersion newFile, int newLineNum) {

        Set<String> contextOld = collectContextTokens(oldFile, oldLineNum);
        Set<String> contextNew = collectContextTokens(newFile, newLineNum);

        return jaccard(contextOld, contextNew);
    }

    public double combinedSimilarity(FileVersion oldFile, int oldLineNum, // we combine both similarities here
                                     FileVersion newFile, int newLineNum) {

        LineRecord oldLine = getLine(oldFile, oldLineNum);
        LineRecord newLine = getLine(newFile, newLineNum);

        double contentSim = contentSimilarity(oldLine, newLine);
        double contextSim = contextSimilarity(oldFile, oldLineNum, newFile, newLineNum);

        
        return 0.6 * contentSim + 0.4 * contextSim; // weighted combination
    }

    // ----- helpers -----

    private LineRecord getLine(FileVersion file, int lineNumber) { // getting line by line number
        return file.getLines().get(lineNumber - 1);
    }

    private Set<String> collectContextTokens(FileVersion file, int centerLine) { // collecting context tokens
        int size = file.getLines().size();
        int start = Math.max(1, centerLine - contextWindow);
        int end   = Math.min(size, centerLine + contextWindow);

        Set<String> tokens = new HashSet<>(); // collect tokens in the context window
        for (int lineNum = start; lineNum <= end; lineNum++) {
            String text = getLine(file, lineNum).getNormalizedText();
            tokens.addAll(tokenize(text));
        }
        return tokens;
    }

    private Set<String> tokenize(String text) { // we tokenize the text here
        if (text == null || text.isBlank()) {
            return Collections.emptySet();
        }
        return Arrays.stream(text.split("\\W+"))
                .filter(s -> !s.isBlank())
                .collect(Collectors.toSet());
    }

    private double jaccard(Set<String> a, Set<String> b) {
        if (a.isEmpty() && b.isEmpty()) {
            return 1.0;
        }
        Set<String> intersection = new HashSet<>(a);
        intersection.retainAll(b);

        Set<String> union = new HashSet<>(a);
        union.addAll(b);

        if (union.isEmpty()) return 0.0;
        return (double) intersection.size() / union.size();
    }
}
