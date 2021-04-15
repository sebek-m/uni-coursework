package cp1.solution;

import java.util.concurrent.Semaphore;
import java.util.concurrent.atomic.AtomicInteger;

/*
  Stores information and objects that are useful in the process
  of acquiring access to a resource but do not need to
  be accessed directly from the map every time.
 */
public class OperationHelper {
    private boolean access;

    private Semaphore resourceSemaphore;

    private AtomicInteger waitingCounter;

    public OperationHelper() {

    }

    public void setAccess(boolean access) {
        this.access = access;
    }

    public void saveSemaphore(Semaphore sem) {
        this.resourceSemaphore = sem;
    }

    public void saveCounter(AtomicInteger counter) {
        waitingCounter = counter;
    }

    public void decrementWaitingCounter() {
        waitingCounter.decrementAndGet();
    }

    public boolean accessGranted() {
        return access;
    }

    public void waitForResource() throws InterruptedException {
        resourceSemaphore.acquire();
    }
}
