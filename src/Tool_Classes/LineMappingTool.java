package tool;


import java.io.IOException;
import java.util.*;

/**
 * LineMappingTool
 *
 * Main class that:
 *  Step 1: uses Preprocessor to load + normalize files
 *  Step 2: uses UnchangedDetector to find exact matches
 *  Step 3: uses CandidateGenerator to generate candidate new lines
 *  Step 4+5: uses Mapper + SimilarityCalculator to compute final mappings
 *  Step 6: uses MappingWriter to write the TXT mapping file
 */
public class LineMappingTool {

    
    public LineMappingTool() {
    }

    /**
     * High-level pipeline.
     */
    public void run(String oldFilePath, String newFilePath, String outputMappingPath) throws IOException { // to run the tool
        // Step 1: read + normalize
        Preprocessor preprocessor = new Preprocessor(); // we preprocess files
        FileVersion oldFile = preprocessor.loadFile(oldFilePath); // we load old file
        FileVersion newFile = preprocessor.loadFile(newFilePath); // we load new file

        UnchangedDetector unchangedDetector = new UnchangedDetector(); // Step 2: detect unchanged lines
        Map<Integer, Integer> unchangedMapping = unchangedDetector.detectUnchanged(oldFile, newFile);
        // unchangedMapping: oldLine -> newLine

        
        Set<Integer> unmatchedOld = new HashSet<>(); // we store unmatched old lines
        for (LineRecord lr : oldFile.getLines()) {
            if (!unchangedMapping.containsKey(lr.getLineNumber())) { // to find unmatched old lines
                unmatchedOld.add(lr.getLineNumber());
            }
        }

        Set<Integer> usedNew = new HashSet<>(unchangedMapping.values()); // we store used new lines
        Set<Integer> unmatchedNew = new HashSet<>();
        for (LineRecord lr : newFile.getLines()) {
            if (!usedNew.contains(lr.getLineNumber())) {
                unmatchedNew.add(lr.getLineNumber());
            }
        }


        CandidateGenerator candidateGenerator = new CandidateGenerator( // Step 3: generate candidates
                15,   // window size (tweak if needed)
                true  // require token overlap
        );
        Map<Integer, List<Integer>> candidates =
                candidateGenerator.generateCandidates(oldFile, newFile, unmatchedOld, unmatchedNew); // we generate candidates

        // Step 4: similarity + mapping
        SimilarityCalculator similarityCalculator = new SimilarityCalculator( // we set up similarity calculator
                2    // context window size (lines above/below)
        );
        Mapper mapper = new Mapper( // to map lines
                similarityCalculator,
                0.6,  // similarity threshold
                true, // enableSplitRefinement 
                3    // maxSplitLength (used only if enableSplitRefinement=true)
        );

        List<MappingEntry> finalMappings =
                mapper.mapLines(oldFile, newFile, unchangedMapping, candidates);

        // Step 6: write TXT mapping
        MappingWriter mappingWriter = new MappingWriter();
        mappingWriter.writeMapping(outputMappingPath, finalMappings);
    }

    /**
     * Simple CLI: java Tool_Classes.LineMappingTool old.java new.java mapping.txt
     */
    public static void main(String[] args) throws IOException { // main method to run the tool
        if (args.length < 3) {
            System.err.println("Usage: java Tool_Classes.LineMappingTool <oldFile> <newFile> <outputMappingFile>");
            System.exit(1);
        }

        String oldFile = args[0];
        String newFile = args[1];
        String outFile = args[2];

        LineMappingTool tool = new LineMappingTool();
        tool.run(oldFile, newFile, outFile);
    }
}

