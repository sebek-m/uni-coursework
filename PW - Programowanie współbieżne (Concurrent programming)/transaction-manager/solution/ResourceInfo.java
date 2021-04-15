package cp1.solution;

import cp1.base.Resource;

import java.util.concurrent.Semaphore;
import java.util.concurrent.atomic.AtomicInteger;

/*
    Stores variables and objects that together represent
    the state of a resource.
    Also provides methods useful when called in computeIfPresent
    in methods from TransactionManagerImpl.
 */
public class ResourceInfo {
    private enum State {
        free,
        taken,
        beingPassed
    }

    private Resource resource;

    private State state = State.free;
    private long usedBy = 0; // According to the documentation, thread IDs are positive.
    private AtomicInteger howManyWaiting = new AtomicInteger(0);

    private Semaphore waiting = new Semaphore(0, true);

    public ResourceInfo(Resource resource) {
        this.resource = resource;
    }

    public ResourceInfo maybeGetAccess(OperationHelper helper) {
        if (state != State.free) {
            helper.setAccess(false);
            helper.saveSemaphore(waiting);
            helper.saveCounter(howManyWaiting);
            howManyWaiting.incrementAndGet();
        }
        else {
            state = State.taken;
            usedBy = Thread.currentThread().getId();
            helper.setAccess(true);
        }

        return this;
    }

    public ResourceInfo inheritAccess() {
        usedBy = Thread.currentThread().getId();
        state = State.taken;

        return this;
    }

    public ResourceInfo freeResource() {
        this.usedBy = 0;

        if (howManyWaiting.get() == 0) {
            this.state = State.free;
        }
        else {
            this.state = State.beingPassed;
            waiting.release();
        }

        return this;
    }

    public ResourceInfo investigateResource(DeadlockInvestigationHelper helper) {
        if (state != State.taken) {
            helper.finishInvestigation();
        }
        else {
            helper.updateOwner(usedBy);
        }

        return this;
    }

    public Resource getResource() {
        return resource;
    }
}
