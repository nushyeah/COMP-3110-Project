package Tool_Classes;

import java.io.BufferedWriter;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.Comparator;
import java.util.List;

/**
 * Step 6: OUTPUT THE MAPPING
 *
 * Writes the mapping into a simple TXT file.
 * oldLine newLine status
 * where newLine = -1 means deleted.
 */
public class MappingWriter {

    public void writeMapping(String outputPath, List<MappingEntry> mappingEntries) throws IOException {
        Path out = Path.of(outputPath);

        try (BufferedWriter writer = Files.newBufferedWriter(out)) {
            // Sorting by old line number for readability
            mappingEntries.stream()
                    .sorted(Comparator.comparingInt(e -> e.oldLine))
                    .forEach(entry -> {
                        try {
                            // Basic LHDiff-compatible format: "old new"
                            writer.write(entry.oldLine + " " + entry.newLine + " " + entry.status);
                            writer.newLine();

                        } catch (IOException ex) {
                            throw new RuntimeException(ex);
                        }
                    });
        }
    }
}
