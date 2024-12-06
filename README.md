# TESTTING_EPITA
## Sujet :
Le but de ce TP est d'interpréter des séquences de bits simulées par un fichier 'input_FW.txt'


Le comportement de ces séquences de bits peuvent être retrouvées dans 'FW_Expression_de_besoins.pdf'
## Utilisation
Pour tester notre solution :
```bash
make
```
Pour executer les tests-suites (test : lecture des trames) (testAction : test unitaire de la fonction action) (testReplace : test unitaire de la fonction Replace):
```bash
make test
make testAction
make testReplace
```

## Pré-requis :
```bash
gcc
make
libcunit1-dev
```

