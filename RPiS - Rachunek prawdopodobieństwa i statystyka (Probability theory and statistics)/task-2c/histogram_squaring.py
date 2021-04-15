import numpy as np
import matplotlib.pyplot as plt

def howManyStepsFull(dates):
    steps = list()

    stepCount = 0
    currentDates = set()

    for date in dates:
        stepCount += 1
        
        if date not in currentDates:
            currentDates.add(date)
        else:
            steps.append(stepCount)
            stepCount = 0
            currentDates.clear()

    return steps

def squareHistogram(probs, meanProb):
    # Domyślnie ustawiamy, że każda data "bierze" prawdopodobieństwo od siebie samej.
    # Dzięki temu w końcowym wyniku nie pominiemy informacji o tym, kto "dał" prawdopodobieństwo datom,
    # które już przed wykonaniem algorytmu mają prawdopodobieństwo równe średniej.
    whoGaveMe = np.arange(np.size(probs))

    less = np.array(np.where(probs < meanProb)[0]).tolist()
    more = np.array(np.where(probs > meanProb)[0]).tolist()

    # Zmieniamy typ danych tablicy z prawdziwymi prawdopodobieństwami na float,
    # bo bez tego przy odejmowaniu program zaokrąglał wartości w probs do intów
    probs = np.array(probs, dtype=np.float_)

    while less:
        # Wybieramy pierwszego, któremu brakuje i pierwszego, który ma za dużo
        lacking = less.pop(0)
        overflowing = more[0]

        whoGaveMe[lacking] = overflowing
        probs[overflowing] -= meanProb - probs[lacking]

        # Po zabraniu może się okazać, że trzeba wykreślić/przenieść datę, która miała wcześniej za dużo
        if probs[overflowing] < meanProb:
            less.append(more.pop(0))
        elif probs[overflowing] == meanProb:
            more.pop(0)

    return probs, whoGaveMe
            
# Z takiego rozmiaru próby wylosowanych dat i prawdopodobieństw otrzymamy
# trochę ponad 100 000 powtórzeń daty urodzenia, czyli tyle, ile razy mieliśmy powtórzyć algorytm
# w poprzednich zadaniach.
randomSampleSize = 3000000

stats = np.loadtxt("us_births_69_88.csv", skiprows = 1,
				   delimiter = ',', dtype = int)

# Wyciągamy same prawdopodobieństwa (liczby urodzeń danego dnia)
onlyProbs = stats[::, 2]

meanProb = np.mean(onlyProbs)

newProbsAndBuckets = squareHistogram(onlyProbs, meanProb)
probs = newProbsAndBuckets[0]
whoGaveMe = newProbsAndBuckets[1]

# Losujemy daty
randDates = np.random.randint(0, np.size(onlyProbs), randomSampleSize)

# Do wylosowanych dat dopasowujemy te,
# od których "wzięły" prawdopodobieństwo
# podczas algorytmu squareHistogram
otherDates = whoGaveMe[randDates]

# Losujemy prawdopodobieństwa
randProbs = np.random.uniform(0, meanProb, randomSampleSize)

# Tworzymy tablicę z informacją, dla których wylosowanych
# wartości powinniśmy wybrać datę odpowiadającą "lewej", czyli oryginalnej części kubełka
leftBucketProbs = probs[randDates]
whichGoIn = randProbs < leftBucketProbs

# Jeśli wylosowane prawdopodobieństwo jest mniejsze od "ustalonego" to bierzemy oryginalną datę,
# wpp bierzemy tę, od której ta data dobrała prawdopodobieństwo
sampledDates = np.where(whichGoIn, randDates, otherDates)

howManySteps = howManyStepsFull(sampledDates)

plt.hist(howManySteps, bins = range(1, 61))
plt.xlabel('Number of steps', fontsize=12)
plt.ylabel('Number of occurences', fontsize=12)

plt.show()
