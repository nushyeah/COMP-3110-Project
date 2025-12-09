package tool;


import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.List;

/**
 * Step 1: READ AND NORMALIZE LINES
 * Loads a file from disk, reads it line by line, and builds a FileVersion
 * Each line is converted into a LineRecord with both original and normalized text
 */

public class Preprocessor {

    /**
     * Read the file from the given path and return a FileVersion object.
     */
    public FileVersion loadFile(String path) throws IOException {
        List<String> rawLines = Files.readAllLines(Path.of(path));
        List<LineRecord> records = new ArrayList<>();

        int lineNo = 1;
        for (String line : rawLines) {
            String normalized = normalize(line);
            records.add(new LineRecord(lineNo, line, normalized));
            lineNo++;
        }

        return new FileVersion(path, records);
    }

    /**
     * Very simple normalization to ignore small formatting changes:
        * Trim leading/trailing spaces
        * Collapse multiple spaces/tabs into a single space
        * Convert everything to lowercase
     * Same step shown in the professor's PPT
     */
    private String normalize(String line) {
        if (line == null) {
            return "";
        }
        return line
                .trim()
                .replaceAll("\\s+", " ")
                .toLowerCase();
    }
}
