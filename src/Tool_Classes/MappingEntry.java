package Tool_Classes;
public class MappingEntry {
    public final int oldLine;   // -1 if added
    public final int newLine;   // -1 if deleted
    public final String status; // "unchanged", "added", "deleted", "modified(minor)" etc.

    public MappingEntry(int oldLine, int newLine, String status) {
        this.oldLine = oldLine;
        this.newLine = newLine;
        this.status = status;
    }
}

