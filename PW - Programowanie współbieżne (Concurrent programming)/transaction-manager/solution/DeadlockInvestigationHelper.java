package cp1.solution;

import cp1.base.ResourceId;

/*
    Stores information and provides methods that are essential
    for the process of determining whether a group of transactions
    may enter a deadlock, that is, the process of finding a cycle
    in the imaginary graph in which u->v if and only if u is waiting
    for a resource owned by v.
 */
public class DeadlockInvestigationHelper {
    private long performer;

    private ResourceId currentRid;
    private long currentOwner;
    private long currentOwnerStartTime;

    private long latestTransactionStartTime;
    private long latestTransactionOwner;

    private boolean complete = false;
    private boolean deadlock = false;

    public DeadlockInvestigationHelper(ResourceId desiredResourceId,
                                       long performer, long performerStartTime) {
        this.currentRid = desiredResourceId;
        this.performer = performer;
        this.currentOwner = performer;
        this.latestTransactionStartTime = performerStartTime;
        this.latestTransactionOwner = performer;
    }

    public boolean investigationComplete() {
        return complete;
    }

    public void finishInvestigation() {
        this.complete = true;
    }

    public ResourceId getCurrentRid() {
        return currentRid;
    }

    public long getLatestTransactionOwner() {
        return latestTransactionOwner;
    }

    public void updateOwner(long owner) {
        currentOwner = owner;
    }

    public void updateCurrentOwnerStartTime(long startTime) {
        currentOwnerStartTime = startTime;
    }

    public void updateCurrentRid(ResourceId rid) {
        currentRid = rid;
    }

    public void maybeUpdateLatest() {
        if (currentOwnerStartTime > latestTransactionStartTime) {
            latestTransactionOwner = currentOwner;
            latestTransactionStartTime = currentOwnerStartTime;
        }
        else if (currentOwnerStartTime == latestTransactionStartTime &&
                 currentOwner > latestTransactionOwner) {
            latestTransactionOwner = currentOwner;
        }
    }

    public long getCurrentOwner() {
        return currentOwner;
    }

    public void checkIfDeadlocked() {
        if (currentOwner == performer) {
            complete = true;
            deadlock = true;
        }
    }

    public boolean deadlockFound() {
        return deadlock;
    }
}
