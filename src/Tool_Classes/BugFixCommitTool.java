package tool;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.*;

/**
 * BONUS TOOL EXTENSION:
 * Detects bug-fix commits by scanning commit messages
 
 * Input:
  - A text file where each line represents one comit
  - Each line is expected to look like: HASH|message
 * (for example: 9a7c21|Fix login error (#123))
 
 * Output:
  - A plain text file that contains only the commits that look like bug fixes.
  - We use a simple comma-separated format for each line:
     *      hash,message
 
 */
public class BugFixCommitTool {

    // Simple keyword list to decide if a commit is a "bug fix"
    private static final List<String> BUG_FIX_KEYWORDS = Arrays.asList(
            "fix", "bug", "error", "defect", "issue", "patch", "hotfix"
    );

    /**
     * Main entry point for the bonus tool.
     *
     * Usage:
       *   java tool.BugFixCommitTool <inputCommitsTxt> <outputBugFixesTxt>
     
     * Example:
       *   java tool.BugFixCommitTool commits.txt bug_fix_commits.txt
     */
    public static void main(String[] args) throws IOException {
        if (args.length < 2) {
            System.out.println("Usage: java tool.BugFixCommitTool <inputCommitsTxt> <outputBugFixesTxt>");
            return;
        }

        String inputPath = args[0];
        String outputPath = args[1];

        // Read all commit lines from the input file
        List<String> allCommits = Files.readAllLines(Path.of(inputPath));

        List<String> bugFixCommits = new ArrayList<>();
        // header row
        bugFixCommits.add("hash,message");

        for (String line : allCommits) {
            line = line.trim();
            if (line.isEmpty()) {
                continue;
            }

            // Expecting: HASH|message
            String[] parts = line.split("\\|", 2);
            if (parts.length < 2) {
                // If the format is not exactly HASH|message, just skip this line
                continue;
            }

            String hash = parts[0].trim();
            String message = parts[1].trim();

            if (isBugFixMessage(message)) {
                // We write it out as: hash,message
                bugFixCommits.add(hash + "," + message);
            }
        }

        Files.write(Path.of(outputPath), bugFixCommits);

        System.out.println("Total commits read: " + allCommits.size());
        System.out.println("Bug-fix commits detected: " + (bugFixCommits.size() - 1));
        System.out.println("Results written to: " + outputPath);
    }

    /**
     * Checks if a commit message looks like a bug fix
     * We simply look for common keywords or an issue number pattern (#123).
     
     */
    private static boolean isBugFixMessage(String message) {
        String msg = message.toLowerCase();

        // If the message refers to an issue ID, we treat it as a fix
        // Example: "Fix login error (#123)"
        if (msg.contains("#")) {
            return true;
        }

        // Check for keywords like "fix", "bug", "error", etc.
        for (String keyword : BUG_FIX_KEYWORDS) {
            if (msg.contains(keyword)) {
                return true;
            }
        }

        return false;
    }
}
