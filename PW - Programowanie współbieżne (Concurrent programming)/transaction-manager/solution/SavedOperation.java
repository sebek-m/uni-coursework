package cp1.solution;

import cp1.base.ResourceId;
import cp1.base.ResourceOperation;

/*
    Used for storing a history of operations executed by
    a transaction.
 */
public class SavedOperation {
    private ResourceOperation operation;
    private ResourceId rid;

    public SavedOperation(ResourceOperation operation, ResourceId rid) {
        this.operation = operation;
        this.rid = rid;
    }

    public ResourceId getRid() {
        return this.rid;
    }

    public ResourceOperation getOperation() {
        return operation;
    }
}
