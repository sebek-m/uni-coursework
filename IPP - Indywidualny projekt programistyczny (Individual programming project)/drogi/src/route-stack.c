/** @file
 * Funkcje potrzebne do obsługi struktury RouteStack.
 *
 * @author Sebastian Miller
 * @date 28.04.2019
 */

#include "route-stack.h"
#include "utils.h"

#include <stdlib.h>
#include <limits.h>
#include <string.h>

RouteStack* createStack(City* target, RoadSeg* targetRoad) {
    RouteStack* new = (RouteStack*)malloc(sizeof(RouteStack));
    if (new) {
        new->first = NULL;
        new->last = NULL;
        new->nextCandidate = NULL;
        new->lengthSum = 0;
        new->target = target;
        new->targetRoad = targetRoad;
    }

    return new;
}

bool push(RouteStack* stack, RoadSeg* road) {
    if (stack->first == NULL) {
        stack->first = (Route*)malloc(sizeof(Route));
        if (stack->first == NULL)
            return false;

        (stack->first)->roadSeg = road;
        stack->last = stack->first;
        (stack->last)->nextSegment = NULL;
    }
    else {
        Route* new;

        new = (Route*)malloc(sizeof(Route));
        if (new == NULL)
            return false;

        new->roadSeg = road;
        new->nextSegment = stack->first;
        stack->first = new;
    }

    stack->lengthSum += road->length;

    return true;
}

RoadSeg* pop(RouteStack* stack) {
    Route *tmp;
    RoadSeg* firstRoad;

    tmp = stack->first;
    firstRoad = tmp->roadSeg;

    stack->first = tmp->nextSegment;
    free(tmp);

    stack->lengthSum -= firstRoad->length;

    return firstRoad;
}

bool empty(RouteStack* stack) {
    return stack->first ==  NULL;
}

void clear(RouteStack* stack) {
    while (!empty(stack))
        pop(stack);
}

void removePreviousCandidates(RouteStack** candidates) {
    RouteStack *curr, *next;

    curr = *candidates;
    next = (*candidates)->nextCandidate;

    while (next != NULL) {
        clear(curr);
        free(curr);
        curr = next;
        next = next->nextCandidate;
    }

    *candidates = curr;
}

RouteStack* copyRouteStack(RouteStack* stack) {
    Route* curr;
    RouteStack* copy;
    bool memoryLeft;

    curr = stack->first;
    copy = createStack(stack->target, stack->targetRoad);
    memoryLeft = true;

    while (curr != NULL && memoryLeft) {
        memoryLeft = push(copy, curr->roadSeg);
        curr = curr->nextSegment;
    }

    if (!memoryLeft)
        clear(copy);
    else
        copy->lengthSum = stack->lengthSum;

    return copy;
}

bool addCandidate(RouteStack* stack, RouteStack** candidates) {
    RouteStack* newCandidate;

    newCandidate = copyRouteStack(stack);
    if (!newCandidate)
        return false;

    newCandidate->nextCandidate = *candidates;
    *candidates = newCandidate;

    return true;
}

WholeRoute* transform(RouteStack* stack) {
    WholeRoute* new;

    new = (WholeRoute*)malloc(sizeof(WholeRoute));
    if (!new)
        return NULL;

    new->first = stack->first;
    new->last = stack->last;

    return new;
}

void clearCandidates(RouteStack* candidates, RouteStack* winner) {
    RouteStack *current, *tmp;

    current = candidates;

    while (current != NULL) {
        tmp = current->nextCandidate;

        if (current != winner)
            clear(current);
        free(current);

        current = tmp;
    }
}

/** @brief Znajduje najstarszy odcinek w drodze.
 * Wyznacza najdawniejszą datę budowy lub remontu
 * występującą w odcinkach w drodze.
 *
 * @param[in] candidate     - pierwszy odcinek w drodze;
 * @return Rok budowy/remontu najstarszego odcinka w drodze.
 */
static int findOldest(Route* candidate) {
    int min, currentYear;
    Route* current;

    current = candidate->nextSegment;
    min = (candidate->roadSeg)->lastRepairYear;

    while (current != NULL) {
        currentYear = (current->roadSeg)->lastRepairYear;

        if (currentYear < min) {
            min = currentYear;
        }

        current = current->nextSegment;
    }

    return min;
}

RouteStack* chooseWinner(RouteStack* candidates) {
    int winningYear, currentMinYear;
    RouteStack *current, *next, *winner;
    unsigned timesEqual, candidateNumber;

    current = candidates;
    winner = current;
    next = current->nextCandidate;
    winningYear = INT_MIN;
    timesEqual = 0;
    candidateNumber = 0;

    while (next != NULL) {
        currentMinYear = findOldest(current->first);
        candidateNumber++;

        if (currentMinYear > winningYear) {
            winningYear = currentMinYear;
            winner = current;
        }
        else if (currentMinYear == winningYear) {
            timesEqual++;
        }

        current = next;
        next = next->nextCandidate;
    }

    if (timesEqual > 0 && timesEqual == candidateNumber - 1)
        winner = NULL;

    return winner;
}

void recognizeCities(RouteStack* stack, City** city1,
                     City** city2, const char* city1Name,
                     const char* city2Name) {
    RoadSeg *first, *last;


    first = stack->first->roadSeg;
    last = stack->last->roadSeg;

    if (strcmp(first->city1->name, city1Name) == 0)
        *city1 = first->city1;
    else
        *city1 = first->city2;

    if (strcmp(last->city1->name, city2Name) == 0)
        *city2 = last->city1;
    else
        *city2 = last->city2;
}

void reverseStack(RouteStack* stack) {
    Route *current, *next, *previous;

    previous = NULL;
    current = stack->first;

    while (current != NULL) {
        next = current->nextSegment;

        current->nextSegment = previous;

        previous = current;
        current = next;
    }

    stack->last = stack->first;
    stack->first = previous;
}