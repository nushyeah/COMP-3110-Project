package tool;


import java.util.List;

/**
 * Wraps all the lines for one version of a file (old or new)
 * It just stores the file name and a list of LineRecord objects
 */

public class FileVersion {

    private final String fileName;
    private final List<LineRecord> lines;

    public FileVersion(String fileName, List<LineRecord> lines) {
        this.fileName = fileName;
        this.lines = lines;
    }

    public String getFileName() {
        return fileName;
    }

    public List<LineRecord> getLines() {
        return lines;
    }
}
