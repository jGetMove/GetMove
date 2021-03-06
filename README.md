CONTENU DU REPERTOIRE
--------------------------------------------------- 

Le fichier makefile permet de compiler tous les programmes :
Il est possible :
- d'effacer tous les .o : `make clean`
- d'effacer tous les .o et les executables : `make purge`
- de lancer toutes les compilations : `make all`
- de lancer des compilations par programme :
	- `make install`
	- `make getmax`
	- `make genjson`
	- `make generator`

Outre le fichier Mafile, Ce répertoire contient 4 différents executables
- `lcm`: extraction de trajectoire
- `getmax`: extraction des itemsets maximaux et des trajectoires associées pour le résultat de lcm. La sortie a la même format que celle de lcm
- `genjson` : générateur de fichier Json de la sortie de lcm
- `generator` : génerateur de fichier de données


LCM
---------------------------------------------------

Les formats d'entrée de lcm contiennent la liste des clusters dans lesquels appartiennent les objets. Rappel : chaque ligne va correspondre à 
un objet (la ligne commence à 0) c'est le premier objet.
Exemple : 
le fichier `mini.dat`

	0 1 2 3
	0 1 2 3
	0   2 3
	
- Le premier objet est dans le cluster 0 puis 1 puis 2 puis 3
- Le dernier objet est dans le cluster 0 puis 2 puis 3
- Le séparateur est un espace ou une tabulation.

`nomtimendex.dat`

Attention le fichier doit avoir le même nom+timeindex.dat 
Par exemple pour le fichier `mini.dat`, le fichier associé est `minitimeindex.dat`

Ce fichier permet de connaitre pour un temps donné le/les cluster associé.
Les séparateurs sont des tabulations.
Par exemple le fichier `minitimeindex.dat` associé au précédent exemple est :

 	1      0
  	2      1
  	3      2
  	4      3
	
La première colonne correspond au temps, la seconde au numéro de cluster associé. Ici au temps 1, il y a le cluster 0. Plusieurs clusters peuvent 
apparaitre au même temps. Par exemple

 	1      0
  	1      1
  	2      2
  	3      3
	
indique qu'au temps 1, il y a le cluster 0 et 1.
Attention il faut respecter l'ordre croissant des clusters sans en sauter. 
Ainsi 

 	1      0
  	1      2
  	2      3
  	3      4

est faux car il n'y a pas le cluster 1.


Pour compiler le programme : 
- `make clean` (pour effacer les potentiels fichiers .obj)
- `make install` qui va créer le fichier executable lcm à la racine du répertoire


Pour l'utiliser : 
- `./lcm`

Exemple : 
`./lcm 1 1 0 1 "files/mini.dat" 0.01`

- Le premier paramètre (`1`) correspond au support minimal (exprimé en nombre d'itemsets)
- Le second paramètre (`1`) correspond à la taille maximale des patterns à extraire
- Le troisième paramètre (`0`) correspond à min_t (temps minimal entre 2 clusters)
- Le quatrième paramètre (`1`) correspond au nombre de block (par défaut 1)
- Le cinquième paramètre (`"files/mini.dat"`) correspond au fichier source des objets - attention le fichier associé "files/minitimeindex.dat" doit 
- être dans le même répertoire
- Le sixième paramètre (`0.01`) correspond à min_w (le pourcentage d'objets qui doivent être en commun) pour les graduels

Le fichier de sortie est appelé : `minisortie_1mint0minw0.01.dat` (fichier mini auquel sont ajoutés les paramètres).
Il contient la liste des items et trajectoires avec les objets et temps associés
par exemple :

```
newitemset
------------
itemset: 0 2 3 
objects: 0 1 2 
Time: 1 3 4 
Swarm
fCS: 
Object: 0 1 2 
Time: 1 3 4 
CS: 
Object: 0 1 2 
Time: 1 3 4 
-------------------
Convoy
CV: 
Object: 0 1 2 
Time: 1 
CV: 
Object: 0 1 2 
Time: 3 4 
-------------------
Group pattern
GP: 
Object: 0 1 2 
Time: 1 3 4 
-------------------
end newitemset
```

GETMAX
-------
getmax permet d'extraire les maximaux des itemsets fréquents

Pour le compiler : `make getmax`

pour l'executer il faut lui donner en paramètre un fichier de sortie de lcm
Par exemple : `minisortie_1mint0minw0.01.dat` (de l'exemple précédent).

Usage : `./getmax files/minisortie_1mint0minw0.dat`
va générer dans le repertoire files le fichier `files/minisortie_1mint0minw0Max.dat` qui a la même structure que la sortie de lcm


GENJSON
-------
`genjson` est un générateur de fichier Json à partir d'une sortie de lcm

Pour le compiler : `make genjson`

Pour l'executer : `./genjson files/fichiersortielcm files/donnees.dat`

où `files/fichiersortielcm` est la sortie du fichier généré par `lcm` (ou `getmax`) et `files/donnees.dat` est le fichier contenant les objets sur lequel lcm a été appliqué. 

Attention dans le répertoire files, il faut impérativement avoir associé le fichier des temps pour les clusters : `files/donneestimeindex.dat`

La sortie générée est un fichier `files/fichiersortielcm.json`.

exemple :
```json
{
"links": [
	{"source":"0","target":"7","value":"10","label":"35,36,37,38,39,75,76,77,78,79"},
	/*...*/
	{"source":"1","target":"4","value":"15","label":"0,1,2,3,4,40,41,42,43,44,55,56,57,58,59"}],
"nodes": [
	{"name":"0","label":"35,36,37,38,39,50,51,52,53,54,75,76,77,78,79,90,91,92,93,94","time":"1"},
/*...*/
	{"name":"17","label":"20,21,22,23,24,30,31,32,33,34,35,36,37,38,39,45,46,47,48,49,50,51,52,53,54,65,66,67,68,69,75,76,77,78,79,90,91,92,93,94","time":"9"}
	]
}
```

Exemple : `./genjson files/minisortie_1mint0minw0.dat files/mini.dat`

generera le fichier : `files/minisortie_1mint0minw0.json`


GENERATOR
---------
generator est un générateur de fichier de données au format utilisable par lcm.

Pour le compiler : `make generator`

Usage :
`./generator nb_dates avg_nb_clusters multi_clusters nb_objects nb_itemsets support filename`

où:
- `nb_dates` précise le nombre de dates à générer
- `avg_nb_clusters` : nombre moyen de clusters par date
- `multi-clusters` : 0 si l'on veut qu'un objet n'appartient qu'à un cluster 1 autrement
- `nb_objects` : nombre d'objets que l'on souhaite générer
- `nb_itemsets` : nombre d'itemsets que l'on veut générer et intégrer directement dans les données comme présent
- `support` : nombre d'occurrence de l'itemset. Ceci est utile pour tester car l'on sait que l'on doit les retrouver dans les résultats
- `filename` : nom du fichier de sortie sans extension

Les fichiers générés sont : `filename.dat` et `filenametimeindex.dat`


Exemple : 

`./generator 100 3 O 100 5 2 toto`

genere 100 timestamps avec en moyenne 3 clusters par temps, un objet ne peut pas appartenir à plusieurs clusters (0), 100 objets 
seront genres dans toto.dat, 5 itemsets sont générés et apparaitront 2 fois dans toto.dat
