# Sebastian Miller, Rachunek Prawdopodobieństwa i Statystyka, zadanie 5c

import numpy as np
import matplotlib.pyplot as plt
from scipy.stats import chi2

# Sumuje wartości z tablic na osi 0 w tablicy data (liczby głosów z próby dla danej płci/na daną partię)
def calculateAxisTotals(data):
    axisSum = lambda i: np.sum(data[i])
    vecAxisSum = np.vectorize(axisSum)
    
    axisTotals = np.fromfunction(vecAxisSum, (data.shape[0],), dtype=int)

    return axisTotals

# Oblicza oczekiwane częstości
def calculateExpectedCounts(rowTotals, columnTotals, N):
    singleField = lambda i, j: (rowTotals[i] * columnTotals[j]) / N
    vecSingleField = np.vectorize(singleField)

    expectedCounts = np.fromfunction(vecSingleField, (rowTotals.size, columnTotals.size), dtype=int)

    return expectedCounts

# Oblicza statystykę Chi-kwadrat dla zaobserwowanej próby i oczekiwanych częstości
def calculateTestStatistic(sampledCounts):
    rowTotals = calculateAxisTotals(sampledCounts)
    columnTotals = calculateAxisTotals(sampledCounts.transpose())

    totalN = np.sum(sampledCounts)

    expectedCounts = calculateExpectedCounts(rowTotals, columnTotals, totalN)
    
    singleField = lambda i, j: ((expectedCounts[i][j] - sampledCounts[i][j])**2) / expectedCounts[i][j]
    vecSingleField = np.vectorize(singleField)

    # Wypełniamy tablicę składnikami statystyki Chi-kwadrat, policzonymi przy pomocy wartości
    # z sampledCounts i expectedCounts
    summands = np.fromfunction(vecSingleField, sampledCounts.shape, dtype=int)

    return np.sum(summands)

# Rows: women, men
# Columns: PiS, PO, Kukiz, Nowoczesna, Lewica, PSL, Razem, KORWiN
# data = np.array([ [39.7,26.4,7.5,7.1,6.6,5.0,4.2,2.8], 
#                  [38.5,20.3,10.6,7.1,6.6,5.4,3.5,7.1]])
data = np.array([[ 17508, 11642,  3308,  3131,  2911,  2205,  1852, 1235],
 [ 17672,  9318,  4865,  3259,  3029,  2479,  1606, 3259]])

S = calculateTestStatistic(data)
print("Chi-Square statistic:", S)

degreesOfFreedom = (data.shape[0] - 1) * (data.shape[1] - 1)

chi2S = chi2.cdf(S, degreesOfFreedom)
print("p-value:", 1 - chi2S)

dataWithoutKorwin = data[:, 0:-1]

SWithoutKorwin = calculateTestStatistic(dataWithoutKorwin)
print("Chi-Square statistic without KORWiN:", SWithoutKorwin)

degreesOfFreedomWOKorwin = (dataWithoutKorwin.shape[0] - 1) * (dataWithoutKorwin.shape[1] - 1)

chi2SWithoutKorwin = chi2.cdf(SWithoutKorwin, degreesOfFreedomWOKorwin)
print("p-value without KORWiN:", 1 - chi2SWithoutKorwin)

# Funkcja chi2.sf to tak naprawdę 1 - CDF, ale wg
# dokumentacji zwraca dokładniejszy wynik i tak też jest w tym przypadku
print("\np-values obtained from chi2.sf function:")
print("p-value:", chi2.sf(S, degreesOfFreedom))
print("p-value without KORWiN:", chi2.sf(SWithoutKorwin, degreesOfFreedomWOKorwin))

# W prawie wszystkich przypadkach p-value = 0, a we wszystkich p-value < 0.05.
# Z tego powodu odrzucamy hipotezę zerową, czyli "Płeć nie ma wpływu
# na preferencje głosowania".
# Warto jednak zauważyć, że choć w przypadku chi2.sf
# bez partii korwin p-value jest bardzo bliskie 0, to nie jest mu równe,
# co sugerowałoby, że możemy tu odrzucić hipotezę zerową z nieco mniejszą pewnością
# niż gdy uwzględnimy tę partię w danych.
# Można więc powiedzieć, że dane dotyczące tej partii rzeczywiście mogą mieć
# wpływ na wyniki testu, aczkolwiek nie tak istotny jak by sie wydawało.
