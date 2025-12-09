package tool;

import java.io.BufferedWriter;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.Comparator;
import java.util.List;

/**
 * Step 6: OUTPUT THE MAPPING
 *
 * LHDiff-style plain text mapping:
 *
 *   ORIG NEW
 *   1 1
 *   2 2
 *   3 -1
 *   ...
 *
 * - ORIG = line number in the old file
 * - NEW  = corresponding line in the new file, or -1 if deleted
 * - Insertions in the new file are *not* listed explicitly; they can be
 *   inferred from gaps in the NEW column.
 */
public class MappingWriter {

    public void writeMapping(String outputPath, List<MappingEntry> mappingEntries) throws IOException {
        Path out = Path.of(outputPath);

        try (BufferedWriter writer = Files.newBufferedWriter(out)) {
            // Optional header (keep it if your prof likes it, remove if they don't)
            writer.write("ORIG NEW");
            writer.newLine();

            // Sort by old line number for readability
            mappingEntries.stream()
                    .sorted(Comparator.comparingInt(e -> e.oldLine))
                    .forEach(entry -> {
                        try {
                            // Only "old new" — no status label
                            writer.write(entry.oldLine + " " + entry.newLine);
                            writer.newLine();
                        } catch (IOException ex) {
                            // Wrap checked exception so we can use forEach
                            throw new RuntimeException(ex);
                        }
                    });
        }
    }
}
