

# -------------------------------------------------------------------
#  makefile plus elabore
#  fichier source, fichier objet et headers separes
#  creation d'un dossier pour les fichiers objet et pour l'executable
#
#
#  options
# -------------------------------------------------------------------

# Nom de l'executable :
EXECUTABLE1 = lcm
EXECUTABLE2 = getmax
EXECUTABLE3 = genjson
EXECUTABLE4 = generator

# Choix du compilateur :
COMPILATEUR = g++

# Options
COPTIONS = -O3 -DNDEBUG -W -Wall -Wno-deprecated -pedantic -ansi -finline-functions
LOPTIONS = -lm

# -------------------------------------------------------------------
#  Emplacement des sources et des objets, des includes et de l'executable
# -------------------------------------------------------------------

# Dossier racine du code :
CODE := $(shell pwd)

# dossier de l'executable
#EXEDIR := $(CODE)/bin/#$(shell arch)

# Adresse des sources, des objets et des includes :
MAINOBJDIR = $(CODE)/obj # repertoire principal des fichiers obj
SRCDIR = $(CODE)/src/Lcm
OBJDIR = $(CODE)/obj/Lcm
INCDIR = $(CODE)/include
SRCDIRGETMAX = $(CODE)/src/getmax
OBJDIRGETMAX = $(CODE)/obj/getmax
SRCDIRGENJSON = $(CODE)/src/generateJson
OBJDIRGENJSON = $(CODE)/obj/generateJson
SRCDIRGENERATOR = $(CODE)/src/generator
OBJDIRGENERATOR = $(CODE)/obj/generator


# creation de la liste des fichiers sources :
SRC = $(wildcard $(SRCDIR)/*.cpp)
SRCGETMAX=$(wildcard $(SRCDIRGETMAX)/*.cpp)
SRCGENJSON=$(wildcard $(SRCDIRGENJSON)/*.cpp)
SRCGENERATOR =$(wildcard $(SRCDIRGENERATOR)/*.cpp)

# si $(SRC) contient plusieurs dossiers
#SRC = $(foreach nom, $(SRCDIR), $(wildcard $(nom)/*.c))

# Liste des fichiers objets :
NOM = $(basename $(notdir $(SRC)))
OBJ = $(addprefix $(OBJDIR)/, $(addsuffix .o, $(NOM)))
NOMGETMAX = $(basename $(notdir $(SRCGETMAX)))
OBJGETMAX = $(addprefix $(OBJDIRGETMAX)/, $(addsuffix .o, $(NOMGETMAX)))
NOMGETMAX = $(basename $(notdir $(SRCGETMAX)))
OBJGETMAX = $(addprefix $(OBJDIRGETMAX)/, $(addsuffix .o, $(NOMGETMAX)))
NOMGENJSON = $(basename $(notdir $(SRCGENJSON)))
OBJGENJSON = $(addprefix $(OBJDIRGENJSON)/, $(addsuffix .o, $(NOMGENJSON)))
NOMGENERATOR = $(basename $(notdir $(SRCGENERATOR)))
OBJGENERATOR = $(addprefix $(OBJDIRGENERATOR)/, $(addsuffix .o, $(NOMGENERATOR)))


# -------------------------------------------------------------------
#  regle par defaut = aide
#  @ evite d'afficher la commande avant de l'executer
# -------------------------------------------------------------------

.PHONY: help

help:
	@echo
	@echo " Program ... "
	@echo
	@echo "--------------------------------------------------------------------------"
	@echo " Available rules"
	@echo
	@echo " install: compile and create the lcm software"
	@echo " getmax: compile and create the getmax software"
	@echo " genjson: compile and create the genjson software"
	@echo " generator: compile and create the generator software"
	@echo " clean: remove object files"
	@echo " purge: remove object files and programs"
	@echo " help: these comments (defaut)"
	@echo "--------------------------------------------------------------------------"
	@echo

# -------------------------------------------------------------------
#  compilation
# -------------------------------------------------------------------

# regle pour fabriquer les dossiers;
make_directory: dirobj dirobjgetmax dirobjgenson dirobjgenerator direxe
	@echo
	@echo " directories obj have been created "
	@echo

# regle edition de liens
install: 	make_directory $(OBJ)
	$(COMPILATEUR) $(LOPTIONS) $(OBJ) -o $(EXECUTABLE1)

getmax: 	make_directory $(OBJGETMAX)
	$(COMPILATEUR) $(LOPTIONS) $(OBJGETMAX) -o $(EXECUTABLE2)

genjson: 	make_directory $(OBJGENJSON)
	$(COMPILATEUR) $(LOPTIONS) $(OBJGENJSON) -o $(EXECUTABLE3)

generator: 	make_directory $(OBJGENERATOR)
	$(COMPILATEUR) $(LOPTIONS) $(OBJGENERATOR) -o $(EXECUTABLE4)

all:
	make install
	make genjson
	make getmax
	make generator


# regle de compilation des sources
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@echo
	@echo "                      Compiling LCM"
	@echo
	$(COMPILATEUR) $(COPTIONS) -I$(INCDIR) -c -o $@ $<

$(OBJDIRGETMAX)/%.o: $(SRCDIRGETMAX)/%.cpp
	@echo
	@echo "                      Compiling GETMAX"
	@echo
	@$(COMPILATEUR) $(COPTIONS) -c -o $@ $<

$(OBJDIRGENJSON)/%.o: $(SRCDIRGENJSON)/%.cpp
	@echo
	@echo "                      Compiling GENJSON"
	@echo
	$(COMPILATEUR) $(COPTIONS) -c -o $@ $<

$(OBJDIRGENERATOR)/%.o: $(SRCDIRGENERATOR)/%.cpp
	@echo
	@echo "                      Compiling GENERATOR"
	@echo
	$(COMPILATEUR) $(COPTIONS) -c -o $@ $<

#$(COMPILATEUR) $(COPTIONS) -I$(INCDIR) -c  $<
#$(COMPILATEUR) -c $(COPTIONS) -I$(INCDIR) -o $< $@
#$@ $<

# -------------------------------------------------------------------
#  regles de creation des dossiers
# -------------------------------------------------------------------

.PHONY: dirobj dirobjgetmax dirobjgenson dirobjgenerator direxe bin

# creation du dossier $(OBJDIR) si besoin :
ifeq ($(strip $( $(wildcard $(OBJDIR)) ) ), )
dirobj:
	mkdir -p $(MAINOBJDIR)
	mkdir -p $(OBJDIR)
else
dirobj:
endif

ifeq ($(strip $( $(wildcard $(OBJDIRGETMAX)) ) ), )
dirobjgetmax:
	mkdir -p $(MAINOBJDIR)
	mkdir -p $(OBJDIRGETMAX)
else
dirobjgetmax:
endif

ifeq ($(strip $( $(wildcard $(OBJDIRGENJSON)) ) ), )
dirobjgenson:
	mkdir -p $(MAINOBJDIR)
	mkdir -p $(OBJDIRGENJSON)
else
dirobjgenson:
endif

ifeq ($(strip $( $(wildcard $(OBJDIRGENERATOR)) ) ), )
dirobjgenerator:
	mkdir -p $(MAINOBJDIR)
	mkdir -p $(OBJDIRGENERATOR)
else
dirobjgenerator:
endif

# creation du dossier $(EXEDIR) si besoin :
#ifeq ($(strip $($(wildcard $(EXEDIR)))), )
#direxe: bin
#	mkdir $(EXEDIR)
#else
#direxe:
#endif

# test si $(CODE)/bin existe et le cree si non
#ifeq ($(strip $( $(wildcard $(CODE)/bin) ) ), )
#bin:
#	mkdir $(CODE)/bin
#else
#bin:
#endif

# -------------------------------------------------------------------
#  regles de nettoyage
# -------------------------------------------------------------------

.PHONY: clean purge

# pour effacer tous les objet :
clean:
	@echo "                      Delete object files"
	@rm -rf $(OBJDIR)/*.o $(OBJDIRGETMAX)/*.o $(OBJDIRGENJSON)/*.o $(OBJDIRGENERATOR)/*.o


# pour effacer tous les objet et les executables :
purge:
	@echo "                      Delete object files and softwares"
	@rm -rf $(OBJDIR)/*.o $(OBJDIRGETMAX)/*.o $(OBJDIRGENJSON)/*.o $(OBJDIRGENERATOR)/*.o $(EXECUTABLE1) $(EXECUTABLE2) $(EXECUTABLE3) $(EXECUTABLE4)

# ------------------------------------------------
# $@ 		Le nom de la cible
# $<		Le nom de la premiÃ¨re dÃ©pendance
# $^		La liste des dÃ©pendances
# shell		pour executer une commande
# wildecard	equivalent de * dans le terminal
# basename	prend le nom sans le .c ou .o ...
# notdir	supprime le chemin devant un fichier
# strip		supprime les blancs => ifeq( $(strip $(VAR) ) , ) = si $(VAR) est vide
# ------------------------------------------------
