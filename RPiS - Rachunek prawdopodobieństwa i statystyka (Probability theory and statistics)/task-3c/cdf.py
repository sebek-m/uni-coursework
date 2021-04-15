import numpy as np
import matplotlib.pyplot as plt
import math
import random

def generateProbabilities(min, max):
    k = np.arange(max - min + 1) + min

    p = 1 / np.where(k != 0, abs(k) * (abs(k) + 1) * (abs(k) + 2), 2)

    return p, k

# Dystrybuanta zmiennej losowej z zadania dla
# k >= 1 całkowitych to 1/4 - 1/(2(k + 1)(k + 2)),
# co wynika z tego, jak teleskopuje się suma P(Y = x)
# dla x z [1, k]. InverseCDF to po prostu funkcja odwrotna
# do dystrybuanty.
def inverseCDF(p):
    # Nie chcemy 0, bo wtedy funkcja zwróciłaby 0, a wiemy, że P(k = 0) = 1/2.
    # A ponieważ prawdopodobieństwo wylosowania 0 za pomocą funkcji uniform,
    # która wywoływana jest w funkcji sample, jest podobno bardzo małe,
    # ewentualne ponowne przelosowanie zmiennej nie będzie zbyt kosztowne
    while p == 0:
        p = random.uniform(0, 1/4)
        
    return np.ceil((-12 * p - math.sqrt((4 * p - 1) * (4 * p - 9)) + 3) / (2 * (4 * p - 1)))

def prefixMedians(arr):
    medians = list()
    
    medians.append(arr[0])
    for i in range(1, arr.size):
        medians.append(np.median(arr[0:i]))
    
    return np.array(medians)

def sample(sampleSize):
    # P(k = 0) = 1/2; P(k < 0) = 1/4, P(k > 0) = 1/4
    possibleSigns = np.array([0, 0, -1, 1])
    signIds = np.random.randint(0, 4, size = sampleSize)
    signs = possibleSigns[signIds]

    # Wg dokumentacji funkcji uniform(), 1/4 nie zostanie wylosowane,
    # więc dzielenie przez 0 w inverseCDF nie będzie miało miejsca
    randomProbs = np.random.uniform(0, 1/4, size = sampleSize)

    inverseCDFVec = np.vectorize(inverseCDF)

    sample = np.where(signs == 0, 0, signs * inverseCDFVec(randomProbs))

    return np.array(sample, dtype=int)

sampleSize = 10000

# Robimy wykres prawdopodobieństw na przedziale [-10, 10]
tmp = generateProbabilities(-10, 10)
probsTo10 = tmp[0]
labels = tmp[1]

plt.xticks(np.arange(21), labels)
plt.plot(probsTo10)
plt.xlabel('Wartość k', fontsize=12)
plt.ylabel('P(X = k)', fontsize=12)

plt.show()

# Generujemy próbę rozmiaru 10000 z rozkładu w treści zadania
sample = sample(sampleSize)

# Robimy wykres średnich "częściowych"
prefixSums = np.cumsum(sample)
indexes = np.arange(1, sampleSize + 1)

prefixMeans = prefixSums / indexes

plt.plot(prefixMeans)
plt.xlabel('Wartość i', fontsize=12)
plt.ylabel('Średnia z {x1,...,xi}', fontsize=12)

plt.show()

# Robimy wykres median "częściowych"
prefixMedians = prefixMedians(sample)

plt.plot(prefixMedians)
plt.xlabel('Wartość i', fontsize=12)
plt.ylabel('Mediana z {x1,...,xi}', fontsize=12)

plt.show()
