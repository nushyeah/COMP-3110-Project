package Tool_Classes;

import java.util.*;

/**
 * Step 4 
 *  - Choosing best matches for unmatched old lines based on combined similarity.
 *  - If score >= threshold, accept; otherwise we will mark as deleted.
 *  - Produce final List<MappingEntry> for all old lines.
 */
public class Mapper { // Mapper class for line mapping

    private final SimilarityCalculator similarityCalculator;
    private final double similarityThreshold; // this is for accepting a match
    private final boolean enableSplitRefinement; // this is for Step 5
    private final int maxSplitLength; // for split refinement

    public Mapper(SimilarityCalculator similarityCalculator, // similarity calculator
                  double similarityThreshold,
                  boolean enableSplitRefinement,
                  int maxSplitLength) {
        this.similarityCalculator = similarityCalculator;
        this.similarityThreshold = similarityThreshold;
        this.enableSplitRefinement = enableSplitRefinement;
        this.maxSplitLength = maxSplitLength;
    }

    
    private static class CandidateMatch { // this is for internal use because we need to sort by score
        final int oldLine;
        final int newLine;
        final double score; // combined similarity score

        CandidateMatch(int oldLine, int newLine, double score) {
            this.oldLine = oldLine;
            this.newLine = newLine;
            this.score = score;
        }
    }

    /**
     * Main entry point:
     *
     * @param oldFile           preprocessed old file 
     * @param newFile           preprocessed new file
     * @param unchangedMapping  oldLine -> newLine for exact unchanged matches
     * @param candidateLists    oldLine -> list of candidate new lines
     * @return list of MappingEntry (one per old line)
     */
    // this is for line mapping
    public List<MappingEntry> mapLines( // finally map lines
            FileVersion oldFile,
            FileVersion newFile,
            Map<Integer, Integer> unchangedMapping, // unchanged lines mapping
            Map<Integer, List<Integer>> candidateLists // candidate new lines for unmatched old lines
    ) {
        int oldSize = oldFile.getLines().size(); // size of old file
        int newSize = newFile.getLines().size(); // size of new file

        // Collect all old/new line numbers
        Set<Integer> allOldLines = new HashSet<>(); // here we collect all old lines
        for (int i = 1; i <= oldSize; i++) {
            allOldLines.add(i);
        }

        Set<Integer> allNewLines = new HashSet<>(); // here we collect all new lines
        for (int i = 1; i <= newSize; i++) {
            allNewLines.add(i);
        }

        // Used new lines start as those in unchanged mapping
        Set<Integer> usedNewLines = new HashSet<>(unchangedMapping.values()); // this is where we track used new lines

        // Old lines not in unchanged -> unmatched
        Set<Integer> unmatchedOldLines = new HashSet<>(allOldLines);
        unmatchedOldLines.removeAll(unchangedMapping.keySet());

        // New lines not used by unchanged
        Set<Integer> unmatchedNewLines = new HashSet<>(allNewLines); 
        unmatchedNewLines.removeAll(usedNewLines);

        // Build candidate matches with scores
        List<CandidateMatch> matches = new ArrayList<>();
        for (int oldLine : unmatchedOldLines) {
            List<Integer> candidates = candidateLists.getOrDefault(oldLine, List.of()); // here we get candidates
            for (int newLine : candidates) {
                double score = similarityCalculator.combinedSimilarity( // calculate combined similarity
                        oldFile, oldLine,
                        newFile, newLine
                );
                matches.add(new CandidateMatch(oldLine, newLine, score));
            }
        }

        // Sort by score descending (best first)
        matches.sort((a, b) -> Double.compare(b.score, a.score));

        Map<Integer, Integer> finalMapping = new HashMap<>(unchangedMapping); // this is the final mapping we will build
        Map<Integer, Double> bestScores = new HashMap<>();

        // Unchanged lines treated as perfect matches
        for (int oldLine : unchangedMapping.keySet()) {
            bestScores.put(oldLine, 1.0);
        }

        Set<Integer> mappedOldLines = new HashSet<>(unchangedMapping.keySet());

        
        for (CandidateMatch m : matches) { // here we select the best matches
            if (m.score < similarityThreshold) {
                // because matches list is sorted descending by score
                break;
            }
            if (mappedOldLines.contains(m.oldLine)) continue;
            if (usedNewLines.contains(m.newLine)) continue;

            finalMapping.put(m.oldLine, m.newLine);
            bestScores.put(m.oldLine, m.score);
            mappedOldLines.add(m.oldLine);
            usedNewLines.add(m.newLine);
        }

        
        for (int oldLine : allOldLines) {    // here we mark unmatched old lines as deleted
            finalMapping.putIfAbsent(oldLine, -1);
            bestScores.putIfAbsent(oldLine, 0.0);
        }

        
        if (enableSplitRefinement) {    // this is the step we refine splits
            refineSplits(oldFile, newFile, finalMapping);
        }

        
        List<MappingEntry> result = new ArrayList<>();  // here we build the final list of MappingEntry
        for (int oldLine = 1; oldLine <= oldSize; oldLine++) {
            int newLine = finalMapping.getOrDefault(oldLine, -1);
            String status;

            if (unchangedMapping.containsKey(oldLine)) {
                status = "unchanged";
            } else if (newLine == -1) {
                status = "deleted";
            } else {
                double score = bestScores.getOrDefault(oldLine, 0.0);
                if (score >= 0.9) {
                    status = "modified(minor)";
                } else {
                    status = "modified";
                }
            }

            result.add(new MappingEntry(oldLine, newLine, status)); // here we add the mapping entry
        }

        return result;
    }

    /**
     * Step 5: refinement for line splits.
     *
     * For now, we only compute the best group [newLine .. newLine + k]
     * but we do NOT change the numeric mapping (still oldLine -> first new line).
     * You can store the groups in some data structure for the GUI if needed.
     */
    private void refineSplits(FileVersion oldFile,  // we will split lines here
                              FileVersion newFile,
                              Map<Integer, Integer> mapping) {

        int newSize = newFile.getLines().size();
        Map<Integer, List<Integer>> splitGroups = new HashMap<>();

        for (Map.Entry<Integer, Integer> entry : mapping.entrySet()) {  // over here we find split groups
            int oldLine = entry.getKey();
            int newLine = entry.getValue();

            if (newLine == -1) continue; // deleted

            LineRecord oldRec = getLine(oldFile, oldLine);
            String oldNorm = oldRec.getNormalizedText();

            int bestEnd = newLine;
            double bestScore = similarityCalculator.contentSimilarity(
                    oldRec, getLine(newFile, newLine));

            StringBuilder concatenated = new StringBuilder(
                    getLine(newFile, newLine).getNormalizedText()
            );

            for (int next = newLine + 1;
                 next <= newSize && next <= newLine + maxSplitLength;
                 next++) {
                concatenated.append(" ")
                        .append(getLine(newFile, next).getNormalizedText());

                double newScore = contentSimilarityForString(oldNorm, concatenated.toString());

                if (newScore > bestScore) {
                    bestScore = newScore;
                    bestEnd = next;
                } else {
                    // if similarity drops, stop extending
                    break;
                }
            }

            if (bestEnd > newLine) { // finally we store the split group
                List<Integer> group = new ArrayList<>();
                for (int i = newLine; i <= bestEnd; i++) {
                    group.add(i);
                }
                splitGroups.put(oldLine, group);
            }
        }

        // For now, we do not change the numeric mapping in 'mapping'.
    }

    // ----- helpers -----

    private LineRecord getLine(FileVersion file, int lineNumber) {
        return file.getLines().get(lineNumber - 1);
    }

    // simple Jaccard for old line vs concatenation of new lines
    private double contentSimilarityForString(String normOld, String normNewGroup) {
        Set<String> tokensOld = tokenize(normOld);
        Set<String> tokensNew = tokenize(normNewGroup);

        if (tokensOld.isEmpty() && tokensNew.isEmpty()) return 1.0;
        Set<String> intersection = new HashSet<>(tokensOld);
        intersection.retainAll(tokensNew);

        Set<String> union = new HashSet<>(tokensOld);
        union.addAll(tokensNew);

        if (union.isEmpty()) return 0.0;
        return (double) intersection.size() / union.size();
    }

    private Set<String> tokenize(String text) {
        if (text == null || text.isBlank()) return Collections.emptySet();
        String[] parts = text.split("\\W+");
        Set<String> tokens = new HashSet<>();
        for (String p : parts) {
            if (!p.isBlank()) tokens.add(p);
        }
        return tokens;
    }
}


