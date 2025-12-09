package tool;


import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

/**
 * Step 2: DETECT UNCHANGED LINES
 *
 * We will find lines that are exactly the same (based on normalized text) in the
 * old and new files.
 *
 * Logic:
 *  - For each old line:
 *      - Scan through all new lines.
 *      - If normalized text matches AND the new line has not been used yet,
 *        we treat this pair as "unchanged".
 *  - Store mapping as: oldLineNumber -> newLineNumber
 *
 *  This matches the description:
 *      - outer loop: old lines
 *      - inner loop: new lines
 *      - first available exact normalized match wins
 */
public class UnchangedDetector { // we detect unchanged lines

    /**
     * We Detect unchanged lines between two file versions.
     *
     * @param oldFile the original version
     * @param newFile the new version
     * @return a map oldLineNumber -> newLineNumber for unchanged lines
     */
    public Map<Integer, Integer> detectUnchanged(FileVersion oldFile, FileVersion newFile) { // we basically compare lines
        Map<Integer, Integer> unchangedMapping = new HashMap<>(); // we store unchanged mapping here

        List<LineRecord> oldLines = oldFile.getLines(); // get old lines
        List<LineRecord> newLines = newFile.getLines();// get new lines

        // Track which new lines are already matched
        Set<Integer> usedNewLines = new HashSet<>();

        for (LineRecord oldLine : oldLines) { // we loop through old lines to find matches
            String oldNorm = oldLine.getNormalizedText();
            int oldLineNum = oldLine.getLineNumber();

            // Try to find a matching new line (normalized text equal, unused)
            for (LineRecord newLine : newLines) {
                int newLineNum = newLine.getLineNumber();

                if (usedNewLines.contains(newLineNum)) {
                    continue; // this means new line is already used
                }

                String newNorm = newLine.getNormalizedText(); // get normalized text of new line

                if (oldNorm.equals(newNorm)) {  
                    // Found an unchanged pair
                    unchangedMapping.put(oldLineNum, newLineNum); // to store mapping
                    usedNewLines.add(newLineNum);
                    break; // move to next old line
                }
            }
        }

        return unchangedMapping;
    }
}


