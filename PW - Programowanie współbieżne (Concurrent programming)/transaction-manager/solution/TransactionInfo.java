package cp1.solution;

import cp1.base.ResourceId;
import cp1.base.ResourceOperation;

import java.util.ArrayDeque;
import java.util.Deque;
import java.util.HashSet;
import java.util.Set;

/*
    Stores variables and objects that together represent
    the state of a transaction.
    Also provides methods useful when called in computeIfPresent
    in methods from TransactionManagerImpl.
 */
public class TransactionInfo {
    private long startTime;
    private Thread owner;

    private Deque<SavedOperation> savedOperations = new ArrayDeque<>();
    private Set<ResourceId> ownedResources = new HashSet<>();

    private boolean aborted = false;
    private boolean waiting = false;

    private ResourceId targetId;

    public TransactionInfo(long startTime) {
        this.startTime = startTime;
        this.owner = Thread.currentThread();
    }

    public boolean isAborted() {
        return aborted;
    }

    public boolean ownsResource(ResourceId rid) {
        return ownedResources.contains(rid);
    }

    public void aimForResource(ResourceId rid) {
        targetId = rid;
        waiting = true;
    }

    public void saveOperation(ResourceOperation operation,
                              ResourceId rid) {
        savedOperations.addFirst(new SavedOperation(operation, rid));
    }

    public void saveResourceAccess(ResourceId rid) {
        ownedResources.add(rid);
    }

    public SavedOperation getLatestOperation() {
        return savedOperations.removeFirst();
    }

    public boolean operationsAvailable() {
        return !savedOperations.isEmpty();
    }

    public Set<ResourceId> getOwnedResources() {
        return ownedResources;
    }

    public TransactionInfo investigateTransaction(DeadlockInvestigationHelper helper) {
        if (aborted || !waiting) {
            helper.finishInvestigation();
        }
        else {
            helper.updateCurrentOwnerStartTime(startTime);
            helper.updateCurrentRid(targetId);
        }

        return this;
    }

    public TransactionInfo abortTransaction() {
        aborted = true;
        owner.interrupt();

        return this;
    }

    public TransactionInfo stopWaiting() {
        waiting = false;

        return this;
    }

    public long getStartTime() {
        return startTime;
    }

    public void clearHistory() {
        savedOperations.clear();
    }
}
