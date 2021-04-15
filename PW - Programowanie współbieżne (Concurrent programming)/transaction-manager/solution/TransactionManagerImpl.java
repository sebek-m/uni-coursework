package cp1.solution;

import cp1.base.*;

import java.util.Collection;
import java.util.Set;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.Semaphore;

/**
 * My implementation of the provided TransactionManager interface.
 *
 * In the methods of this class I use an abundance of computeIfPresent calls in the form of:
 * concurrentHashMapInstance.computeIfPresent(key, (id, info) -> info.simpleMethodExecutedAtomically());
 *
 * This is because, according to the documentation, the whole method,
 * including the call to simpleMethodExecutedAtomically(),
 * is executed atomically.
 * I define the methods I call instead of simpleMethodExecutedAtomically()
 * (in TransactionInfo and ResourceInfo classes) so that they return the object
 * on which they are called (info), since they only modify the members of this object
 * which does not affect the mapping of a given key to the object.
 *
 */
public class TransactionManagerImpl implements TransactionManager {
    private LocalTimeProvider timeProvider;

    private ConcurrentHashMap<ResourceId, ResourceInfo> resources = new ConcurrentHashMap<>();
    private ConcurrentHashMap<Long, TransactionInfo> transactions = new ConcurrentHashMap<>();

    private Semaphore mutex = new Semaphore(1, true);

    public TransactionManagerImpl(
            Collection<Resource> resources,
            LocalTimeProvider timeProvider) {
        this.timeProvider = timeProvider;

        for (Resource resource : resources) {
            this.resources.put(resource.getId(), new ResourceInfo(resource));
        }
    }

    public void startTransaction(
    ) throws
            AnotherTransactionActiveException {
        long currentTime = timeProvider.getTime();

        if (isTransactionActive()) {
            throw new AnotherTransactionActiveException();
        }

        TransactionInfo info = new TransactionInfo(currentTime);

        transactions.put(Thread.currentThread().getId(), info);
    }

    /*
        Calls a method that aborts the transaction that helper
        stores as the latest transaction in a group of would-be-deadlocked
        transactions.
     */
    private void resolveDeadlock(DeadlockInvestigationHelper helper) {
        transactions.computeIfPresent(helper.getLatestTransactionOwner(),
                                        (id, info) -> info.abortTransaction());
    }

    /*
        Calls a method that saves the owner of a given resource to helper
        or sets the flag that informs that there is no cycle.
     */
    private void checkResourceNode(DeadlockInvestigationHelper helper) {
        resources.computeIfPresent(helper.getCurrentRid(),
                (id, info) -> info.investigateResource(helper));
    }

    /*
        Calls a method that checks if the state of a given transaction suggests
        that there is no deadlock and if it does not, it saves the information
        essential for continuing the deadlock investigation to helper.
     */
    private void checkTransactionNode(DeadlockInvestigationHelper helper) {
        transactions.computeIfPresent(helper.getCurrentOwner(),
                (id, info) -> info.investigateTransaction(helper));
    }

    /*
        Determines whether a deadlock would occur if the calling transaction
        were to wait for rid and, if it would, aborts the suitable transaction.
     */
    private void investigateDeadlock(ResourceId rid, long myStartTime) {
        DeadlockInvestigationHelper helper = new DeadlockInvestigationHelper(rid,
                                                Thread.currentThread().getId(),
                                                myStartTime);

        while (true) {
            checkResourceNode(helper);
            if (helper.investigationComplete())
                break;

            helper.checkIfDeadlocked();
            if (helper.investigationComplete())
                break;

            checkTransactionNode(helper);
            if (helper.investigationComplete())
                break;

            helper.maybeUpdateLatest();
        }

        if (helper.deadlockFound()) {
            resolveDeadlock(helper);
        }
    }

    private void maybeThrowInterrupted(Thread me) throws InterruptedException {
        if (me.isInterrupted()) {
            throw new InterruptedException();
        }
    }

    /*
        Given that the calling transaction owns the resource identified by id,
        executes operation on the resource and acts accordingly to its result.
     */
    private void performOperation(ResourceId rid,
                                  ResourceOperation operation,
                                  TransactionInfo myTransaction)
            throws ResourceOperationException, InterruptedException {
        Resource resource = resources.get(rid).getResource();

        operation.execute(resource);

        /*
         Since the execute method does not throw InterruptedException,
         it cannot be determined whether the interrupt flag detected here
         has been set during the execution of execute or right after it.
         Thus, I assume that an interrupt that's occurred right after it
         can be considered as an interrupt that's happened during its execution.
         This implies that the operation has to be undone in such situation.
         We do not save this operation in our history since it shouldn't have
         been executed fully due to the interrupt.

         If the interrupt has happened any time between the moment when the
         transaction has acquired the resource and the moment of calling execute,
         execute is called anyway but the resource remains unchanged since
         we detect the interrupt and call undo here.
         */
        if (Thread.currentThread().isInterrupted()) {
            operation.undo(resource);
            throw new InterruptedException();
        }

        myTransaction.saveOperation(operation, rid);
    }

    /*
        Checks whether a deadlock would occur if it were to wait for the resource,
        potentially resolves the deadlock and waits for the resource.
     */
    private void waitForAccess(OperationHelper helper,
                              TransactionInfo myTransaction,
                              ResourceId rid)
            throws InterruptedException, ActiveTransactionAborted {
        Thread me = Thread.currentThread();
        long myId = me.getId();

        try {
            mutex.acquire();
        }
        catch (InterruptedException e) {
            // Detecting an interrupt that took place before calling acquire
            // or while the thread was waiting on the semaphore.
            helper.decrementWaitingCounter();
            throw e;
        }

        myTransaction.aimForResource(rid);

        // Checking for an interrupt called during aimForResource
        if (me.isInterrupted()) {
            helper.decrementWaitingCounter();
            transactions.computeIfPresent(myId, (id, info) -> info.stopWaiting());
            mutex.release();
            throw new InterruptedException();
        }

        investigateDeadlock(rid, myTransaction.getStartTime());

        mutex.release();

        try {
            helper.waitForResource();
        }
        catch (InterruptedException e) {
            // Detecting an interrupt that took place during the deadlock investigation,
            // right after it or while the thread was waiting for the resource.
            // According to the forum answer: https://moodle.mimuw.edu.pl/mod/forum/discuss.php?d=4002#p10183,
            // if the detected interrupt occurred due to the transaction being aborted,
            // ActiveTransactionAborted is thrown instead of InterruptedException.
            if (myTransaction.isAborted()) {
                me.interrupt();
                throw new ActiveTransactionAborted();
            }
            else {
                throw e;
            }
        }
        finally {
            helper.decrementWaitingCounter();
            transactions.computeIfPresent(myId, (id, info) -> info.stopWaiting());
        }

        resources.computeIfPresent(rid, (id, info) -> info.inheritAccess());
    }

    public void operateOnResourceInCurrentTransaction(
            ResourceId rid,
            ResourceOperation operation
    ) throws
            NoActiveTransactionException,
            UnknownResourceIdException,
            ActiveTransactionAborted,
            ResourceOperationException,
            InterruptedException {
        if (!isTransactionActive()) {
            throw new NoActiveTransactionException();
        }

        if (!resources.containsKey(rid)) {
            throw new UnknownResourceIdException(rid);
        }

        if (isTransactionAborted()) {
            throw new ActiveTransactionAborted();
        }

        Thread me = Thread.currentThread();
        long myId = me.getId();
        TransactionInfo myTransaction = transactions.get(myId);

        if (!myTransaction.ownsResource(rid)) {
            OperationHelper helper = new OperationHelper();

            // Checking for an interrupt called before accessing the resource.
            maybeThrowInterrupted(me);

            resources.computeIfPresent(rid, (id, info) -> info.maybeGetAccess(helper));

            if (!helper.accessGranted()) {
                waitForAccess(helper, myTransaction, rid);
            }

            if (me.isInterrupted()) {
                // I assume that an interrupt detected here can be
                // considered as one that took place during the process
                // of accessing (maybeGetAccess or inheritAccess) the resource.
                resources.computeIfPresent(rid, (id, info) -> info.freeResource());
                throw new InterruptedException();
            }
        }

        // At this point the transaction has acquired access to the desired resource.

        myTransaction.saveResourceAccess(rid);
        performOperation(rid, operation, myTransaction);
    }

    /*
        rollback == false => commits the calling transaction
        rollback == true => rollbacks the calling transaction
     */
    private void finishTransaction(boolean rollback) {
        long tId = Thread.currentThread().getId();

        TransactionInfo myTransaction = transactions.get(tId);

        if (!rollback) {
            myTransaction.clearHistory();
        }
        else {
            while (myTransaction.operationsAvailable()) {
                SavedOperation latestOperation = myTransaction.getLatestOperation();
                ResourceId rid = latestOperation.getRid();

                ResourceOperation op = latestOperation.getOperation();
                op.undo(resources.get(rid).getResource());
            }
        }

        Set<ResourceId> ownedResources = myTransaction.getOwnedResources();
        for (ResourceId rid : ownedResources) {
            resources.computeIfPresent(rid, (id, info) -> info.freeResource());
        }

        ownedResources.clear();

        transactions.remove(tId);
    }

    public void commitCurrentTransaction(
    ) throws
            NoActiveTransactionException,
            ActiveTransactionAborted {
        if (!isTransactionActive()) {
            throw new NoActiveTransactionException();
        }

        if (isTransactionAborted()) {
            throw new ActiveTransactionAborted();
        }

        finishTransaction(false);
    }

    public void rollbackCurrentTransaction() {
        if (!isTransactionActive()) {
            return;
        }

        finishTransaction(true);
    }

    public boolean isTransactionActive() {
        long id = Thread.currentThread().getId();

        return transactions.containsKey(id);
    }

    public boolean isTransactionAborted() {
        long id = Thread.currentThread().getId();

        if (!isTransactionActive()) {
            return false;
        }

        return transactions.get(id).isAborted();
    }
}
