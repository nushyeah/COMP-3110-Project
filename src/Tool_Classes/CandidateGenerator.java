package tool;



import java.util.*;
import java.util.stream.Collectors;

/**
 * Step 3: GENERATE CANDIDATES FOR CHANGED LINES
 *
 * For each unmatched old line, we shall build a list of candidate new lines.
 * Candidates are:
 *   - within a fixed window around the same line number, and
 *   - optionally filtered by token overlap on normalized text.
 */
public class CandidateGenerator { // this is for candidate generation

    private final int windowSize;              // e.g., 10 lines above/below
    private final boolean requireTokenOverlap; // true = filter by token overlap

    public CandidateGenerator(int windowSize, boolean requireTokenOverlap) { // to set parameters
        this.windowSize = windowSize;
        this.requireTokenOverlap = requireTokenOverlap; // we set whether to require token overlap
    }

    /**
     * Generate candidate lists:
     *   oldLineNumber -> list of new line numbers that are candidates.
     */
    public Map<Integer, List<Integer>> generateCandidates( // we generate candidates to map lines
            FileVersion oldFile,
            FileVersion newFile,
            Set<Integer> unmatchedOldLines,
            Set<Integer> unmatchedNewLines
    ) {
        Map<Integer, List<Integer>> candidates = new HashMap<>(); // this is for storing candidates

        int newSize = newFile.getLines().size();

        for (int oldLineNum : unmatchedOldLines) {
            LineRecord oldLine = getLine(oldFile, oldLineNum);

            int start = Math.max(1, oldLineNum - windowSize); // for window calculation
            int end   = Math.min(newSize, oldLineNum + windowSize); // for window calculation

            List<Integer> candidateList = new ArrayList<>(); // to store candidate new lines

            for (int newLineNum = start; newLineNum <= end; newLineNum++) { // loop through window
                if (!unmatchedNewLines.contains(newLineNum)) {
                    continue; // already used in unchanged mapping or mapped earlier
                }

                LineRecord newLine = getLine(newFile, newLineNum); // so we can check token overlap

                if (!requireTokenOverlap || hasTokenOverlap(oldLine, newLine)) { 
                    candidateList.add(newLineNum); // we add candidate if it passes the filter
                }
            }

            candidates.put(oldLineNum, candidateList); // store candidate list for this old line
        }

        return candidates;
    }

    // ----- helpers -----

    private LineRecord getLine(FileVersion file, int lineNumber) { // this is to get line by line number
        // assumes list is in order and lineNumber is 1-based
        return file.getLines().get(lineNumber - 1);
    }

    private boolean hasTokenOverlap(LineRecord oldLine, LineRecord newLine) { // to check token overlap
        Set<String> oldTokens = tokenize(oldLine.getNormalizedText());
        Set<String> newTokens = tokenize(newLine.getNormalizedText());

        oldTokens.retainAll(newTokens);
        return !oldTokens.isEmpty();
    }

    private Set<String> tokenize(String text) {
        if (text == null || text.isBlank()) {
            return Collections.emptySet();
        }
        return Arrays.stream(text.split("\\W+"))
                .filter(s -> !s.isBlank())
                .collect(Collectors.toSet());
    }
}
