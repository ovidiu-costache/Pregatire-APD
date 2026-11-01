Pentru a testa scalabilitatea solutiei paralele trebuie aflati timpii de rulare:
- serial
- paralel cu P thread-uri

Pentru a afla timpul serial, se va rula comanda time ./matrice_seriala N (N = dimensiunea matricei)
Pentru a afla timpul paralel cu P thread-uri, se va rula time ./matrice N P

Pentru a calcula cat mai corect timpul de executie, vom comenta toate afisarile.

Rezultat obtinut pentru solutia seriala cu N = 1000:
ovidiu05@Asus-TUF:~/Facultate/Anul_3/Sem_1/APD/laboratoare/Practic_1/ex4$ time ./matrice_serial 1000

real    0m3.452s
user    0m3.733s
sys     0m0.005s

Rezultat obtinut pentru solutia paralela cu N = 1000 si P = 1:
ovidiu05@Asus-TUF:~/Facultate/Anul_3/Sem_1/APD/laboratoare/Practic_1/ex4$ time ./matrice 1000 1

real    0m2.469s
user    0m2.666s
sys     0m0.005s

Rezultat obtinut pentru solutia paralela cu N = 1000 si P = 2:
ovidiu05@Asus-TUF:~/Facultate/Anul_3/Sem_1/APD/laboratoare/Practic_1/ex4$ time ./matrice 1000 2

real    0m1.222s
user    0m2.468s
sys     0m0.088s

Rezultat obtinut pentru solutia paralela cu N = 1000 si P = 3:
ovidiu05@Asus-TUF:~/Facultate/Anul_3/Sem_1/APD/laboratoare/Practic_1/ex4$ time ./matrice 1000 3

real    0m1.080s
user    0m2.888s
sys     0m0.253s

Rezultat obtinut pentru solutia paralela cu N = 1000 si P = 4:
ovidiu05@Asus-TUF:~/Facultate/Anul_3/Sem_1/APD/laboratoare/Practic_1/ex4$ time ./matrice 1000 4

real    0m1.037s
user    0m3.376s
sys     0m0.527s


Observatii:
Se observa cu timpul paralel se micosreaza cu cat crestem numarul de thread-uri, asta inseamna ca
acceleratia este din ce in ce mai mare cu cat scade timpul paralel conform formulei:
S(P) = T_serial / T_paralel(P)

Eficienta este data de formula:
E(P) = S(P) / P

