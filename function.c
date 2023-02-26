#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <stdbool.h>
#include <unistd.h>
#include "structure.h"
#include "const.h"
//Cette fonction nous permet d'initialiser la structure de données contenues de station
dataStation dataStationFunc(uint32_t vId,char* nameLigne,uint32_t cost,_Bool change)
{
    dataStation vDataStation;
    vDataStation.id=vId;
    vDataStation.change=change;
    vDataStation.cost=cost;
    vDataStation.nameLigne=malloc(sizeof(char)*100);
    assert(vDataStation.nameLigne!=NULL);
    strcpy(vDataStation.nameLigne,nameLigne);
    return vDataStation;
}

//Cette fonction alloue une nouvelle station
station* newStation(dataStation vData){
    station* newStation=malloc(sizeof(station));
    //Verification du bon déroulement de malloc
    assert(newStation!=NULL);
    newStation->data=vData;
    newStation->next=NULL;
    return newStation;
}

//Cette fonction alloue une nouvelle liste de station
listStation* newListStation(){
    listStation* newListStation=malloc(sizeof(listStation));
    //Verification du bon déroulement du malloc
    assert(newListStation!=NULL);
    newListStation->size=0;
    newListStation->head=NULL;
    return newListStation;
}

//Cette fonction ajoute une station dans la liste
void addStation(listStation* l,dataStation vData){
    station* station=newStation(vData);
    station->next=l->head;
    l->head=station;
    l->size++;
}

//Cette fonction nous permet d'extraire la tête de la liste des stations
dataStation popStation(listStation* l){
    assert(l->head!=NULL);
    dataStation vDataStation=l->head->data;
    station* newHead=l->head;
    l->head=l->head->next;
    l->size--;
    free(newHead);
    return vDataStation;
}

//Cette fonction vérifie si une liste de station est vide
_Bool isEmptyListStation(listStation *l){
    return l->head==NULL;
}

//Cette fonction libère la liste de stations
void freeListStation(listStation *l){
    while (!isEmptyListStation(l)) {
        popStation(l);
    }
    free(l);
}

//Cette fonction alloue une nouvelle station utilisé dans l'algorithme
stationFinale* newStationFinale(station *vStation){
    stationFinale* vStationFinale=malloc(sizeof(stationFinale));
    assert(vStationFinale!=NULL);
    vStationFinale->station=vStation;
    vStationFinale->previous=newListStation();
    vStationFinale->next=NULL;
    return vStationFinale;
}

//Cette fonction initialise une nouvelle liste finale utilisée dans l'algo
listStationFinale* newListStationFinale(){
    listStationFinale *vListStationFinale=malloc(sizeof(listStationFinale));
    vListStationFinale->head=NULL;
    vListStationFinale->size=0;

    return vListStationFinale;
}

//Cette fonction nous permet d'ajouter une satation dans la liste d'algo avec un tri décroissant
void addStationFinale(listStationFinale* l,station *vStation,stationFinale* vStationFinalePrevious){
    stationFinale *vStationFinale=newStationFinale(vStation);
    vStationFinale->previous->head=vStationFinalePrevious->previous->head;
    addStation(vStationFinale->previous, vStationFinalePrevious->station->data);
    stationFinale *tmp;
    if (l->size==0 || vStation->data.cost < l->head->station->data.cost) {
        vStationFinale->next=l->head;
        l->head=vStationFinale;
        l->size++;
    }    
    else {
        vStationFinale->next=l->head;
        while (vStationFinale->next->next!=NULL && vStationFinale->station->data.cost > vStationFinale->next->next->station->data.cost) {
            vStationFinale->next=vStationFinale->next->next;
        }
        tmp=vStationFinale->next;
        vStationFinale->next=tmp->next;
        tmp->next=vStationFinale;
        l->size++;
    }
}

//Cette fonction nous permet d'extraire la tête de la liste contenant 
//les stations finaux
stationFinale* popStationFinale(listStationFinale* l){
    if (l->size==0) {
        assert(0);
    }
    stationFinale *vStationFinale=l->head;
    l->head=vStationFinale->next;
    l->size--;
    return vStationFinale;
}

//Cette fonction vérifie si une liste de station finale est vide
_Bool isEmptyListStationFinale(listStationFinale *l){
    return l->head==NULL;
}

//Cette fonction nous donne le nombre de station de notre FILE_ID
uint32_t getFileElem(void){
    FILE *fileId;
    fileId=fopen(FILE_ID_STATION, "rb");
    uint32_t count=0;
    char c;
    while ((c = getc(fileId)) != EOF) {
    if (c == '\n') {
      count++;
    }
  }
    fclose(fileId);
    return count;
}

//Cette fonction alloue une nouvelle table de hashage de liste de station
//Très important la fonction getSizeStation nous retourne le nombre des stations
tabHashageListStation* newtabHLS(uint32_t vSize){
    tabHashageListStation* newTabHash=malloc(sizeof(tabHashageListStation));
    assert(newTabHash!=NULL);
    newTabHash->tab=malloc(vSize*sizeof(listStation*));
    //Verification du bon déroulement de malloc
    assert(newTabHash->tab);
    for (uint32_t i=0;i<vSize; i++) {
        newTabHash->tab[i]=newListStation();
    }
    newTabHash->size=vSize;
    return newTabHash;
}

//Cette fonction nous permet d'extraire les données d'une station
dataStation extractDataStation(station* station){
    return station->data;
}

//Cette fonction fait le hashage sur notre table de hashage de liste de station
uint32_t hashageFunction(uint32_t id){
    return id-1;
}

//Cette fonction est considéré comme une base de donnée, 
//autrement dit c'est la fonction qui remplit la table
//de hashage.
void addStationTableHashage(tabHashageListStation* th) {
    FILE *fileId, *fileArrets;
    //Ouvrir les deux fichiers en mode lecture
    fileId = fopen(FILE_ID_STATION, "rb");
    fileArrets = fopen(FILE_ARRETS, "rb");
    if (fileId == NULL || fileArrets == NULL) {//Vérifier existence fichier
        fprintf(stderr, "Error: file not found in current directory\n");
        assert(0);
    }
    uint32_t idStation1;
    char nameStation1[100];
    char ligne[100];
    //Utilisation dans la deuxième boucle
    uint32_t idStation2,idStation1Retrouve;
    char *nameLigne=malloc(sizeof(char)*100);
    //Pour ignorer la première ligne
    fgets(ligne, 100, fileId);
    while (fgets(ligne, 100, fileId)!=NULL) {
        rewind(fileArrets);
        //Récupérer chaque champ du fichier
        sscanf(ligne, "%[^,],%d\n", nameStation1, &idStation1);
        uint32_t positionHash=hashageFunction( idStation1);            
        fgets(ligne, 100, fileArrets);
        // Pour faire la différence entre les deux variables, nous avons ajouté le signe '_'
        while (fgets(ligne, 100, fileArrets)!=NULL) {
            sscanf(ligne, "%d,%d,%s\n",&idStation1Retrouve,&idStation2, nameLigne);
            if (idStation1Retrouve==idStation1) {
                addStation(th->tab[positionHash], dataStationFunc(idStation2,nameLigne,0,0));                        
            }
        }
    }
    fclose(fileId);
    fclose(fileArrets);
}

//Cette fonction nous permet de librérer la mémoire après l'utilisation de la table de hashage
void freeThLS(tabHashageListStation* th){
    for (uint32_t i=0; i<th->size; i++) {
        freeListStation(th->tab[i]);
    }
    free(th->tab);
    free(th);
}

//Cette fonction gère les cas particuliers 
_Bool casParticulier(listStation* l,station* vStation){
    //Cas particulier ligne 13
    if ((l->head->data.id==36 && vStation->data.id==112) || (l->head->data.id==112 && vStation->data.id==36) ) {
        return 1;
    }
    //Cas particulier ligne 10
    if ((l->head->data.id==213 && vStation->data.id==172) || (l->head->data.id==173 && vStation->data.id==86) ) {
       return 1;
    }
    //Cas particulier ligne 7bis
    if ((l->head->data.id==202 && vStation->data.id==77) || (l->head->data.id==77 && vStation->data.id==202)) {
        return 1;
    }
    //Cas particulier ligne 7
    if ((l->head->data.id==214 && vStation->data.id==135) || (l->head->data.id==135 && vStation->data.id==214)) {
        return 1;
    }
    return 0;
}

//Cette fonction vérifie si une station est déjà marquée
_Bool verifyIdMarque(listStation* l, dataStation data){
    station * vStation=l->head;
    _Bool value=0;
    while (vStation!=NULL) {
        if (vStation->data.cost<data.cost) {
            value=1;
            return value;
        }
        vStation=vStation->next;
    }
    return value;
}

//Cette fonction est le cerveau de notre programme autrement dit
//c'est elle la responsable du calcul de l'algorithme
stationFinale* principal(tabHashageListStation* th,uint32_t idSource,uint32_t idDestination){
    listStation* vListStationMarque=newListStation();
    listStationFinale* vListStationFinale=newListStationFinale();
    _Bool destionationNotFound=0;
    addStation(vListStationMarque, dataStationFunc(idSource, "laligneinit", 0, 0));
    //Cette ligne nous permet de gérer une erreur de ségmentation dans le premier appel de la fonction addStationFinale
    //Tout d'abord nous avons essayer de lui donner NULL mais une erreur de segmentation apparaissait
    //Donc la solution était de créer une station fictive avec toutes les valeurs à 0
    station* stationToResolvProb=newStation(dataStationFunc(0, "0", 0, 0));
    stationFinale* vStationFinale=newStationFinale(stationToResolvProb);
    addStationFinale(vListStationFinale, vListStationMarque->head,vStationFinale);
    //Cette Station est considérée comme le pivot qui se trouve tjrs dans la tête de la liste utilisée dans l'algo dijkstra
    stationFinale* vStationFinaleCurrent;
    while(!destionationNotFound){
        vStationFinaleCurrent=popStationFinale(vListStationFinale);
        //condition d'arret de boucle si : on trouve la station de destination avec le plus petit coût
        if (vStationFinaleCurrent->station->data.id==idDestination) {
            destionationNotFound=1;
        }
        station* vStationCurrent=vStationFinaleCurrent->station;
       
        uint32_t k=hashageFunction(vStationCurrent->data.id);
        station* vStationTemp=th->tab[k]->head;
        for (uint32_t i=0; i< th->tab[k]->size ; i++) {  
            if (!verifyIdMarque(vListStationMarque,vStationTemp->data) ) { 
                if (casParticulier(vStationFinaleCurrent->previous,vStationTemp)) {
                    vStationTemp->data.cost=CHANGE_LINE+DIRECT_LINE+vStationCurrent->data.cost; 
                    vStationTemp->data.change=1;                  
                } 
                else if ((!strcmp(vStationCurrent->data.nameLigne, vStationTemp->data.nameLigne) || !strcmp(vStationCurrent->data.nameLigne, "laligneinit")) ) {
                    vStationTemp->data.cost=DIRECT_LINE+vStationCurrent->data.cost;
                }
                else {
                    vStationTemp->data.cost=CHANGE_LINE+DIRECT_LINE+vStationCurrent->data.cost;                      
                } 
                addStation(vListStationMarque, vStationTemp->data);
                addStationFinale(vListStationFinale, vStationTemp,vStationFinaleCurrent);
            }
            vStationTemp=vStationTemp->next;    
        }
    }
    freeListStation(vListStationMarque);
    return vStationFinaleCurrent;
}

//Cette fonction inverse la liste retournées par principal
listStation* inverser(listStation *vListStation){
    listStation *vListStationInv=newListStation();
    station* vStation=vListStation->head;      
    //condition pour supprimer la station fictive
    while (vStation->next!=NULL) { 
        addStation(vListStationInv,vStation->data);
        vStation=vStation->next;       
    }
    return vListStationInv;
}

//Cette fonction retourne le nom de la station
char* getNameStationById(uint32_t id){
    FILE *fileId;
    //Ouvrir les deux fichiers en mode lecture
    fileId = fopen(FILE_ID_STATION, "rb");
    if (fileId == NULL ) {//Vérifier existence fichier
        fprintf(stderr, "Error: file not found in current directory\n");
        assert(0);
    }
    uint32_t idStation1;
    char ligne[100];
    //Utilisation dans la deuxième boucle
    char *nameLigne=malloc(sizeof(char)*100);
    //Pour ignorer la première ligne
    fgets(ligne, 100, fileId);
    while (fgets(ligne, 100, fileId)!=NULL) {
        //Récupérer chaque champ du fichier
        sscanf(ligne, "%[^,],%d\n", nameLigne, &idStation1);
        if (id==idStation1) {
            return nameLigne;
        }
    }
    fclose(fileId);
    //Cas impossible car forcèment id existe
    return NULL;
}

//Cette fonction affiche la liste
void printList(listStation* vListStation){
    station* vStation=vListStation->head;
    vStation->data.nameLigne=vStation->next->data.nameLigne;
    couleur("33");
    printf(" |%d %s : Ligne[%s] \n",vStation->data.id,getNameStationById(vStation->data.id),vStation->next->data.nameLigne);
    couleur("0");
    while (vStation->next!=NULL) {
        if (!strcmp(vStation->next->data.nameLigne,vStation->data.nameLigne)) {
            if (vStation->next->next!=NULL && !strcmp(vStation->next->data.nameLigne,vStation->next->next->data.nameLigne) && vStation->next->next->data.change==0) {
                printf("\t|%d %s\n",vStation->next->data.id,getNameStationById(vStation->next->data.id));
            }
            else {
                if (vStation->next->next==NULL) {
                    couleur("33");
                    printf(" |%d %s \n",vStation->next->data.id,getNameStationById(vStation->next->data.id));
                    couleur("0");
                }
                else {
                    couleur("33");
                    printf(" |%d %s\n",vStation->next->data.id,getNameStationById(vStation->next->data.id));
                    couleur("31");
                    printf("\033[1;31m");
                    printf("\t\tAttention Changement de ligne!!!\n");
                    printf("\033[0m");
                    couleur("33");
                    printf(" |%d %s : Ligne[%s] \n",vStation->next->data.id,getNameStationById(vStation->next->data.id),vStation->next->next->data.nameLigne);
                    couleur("0");
                }
            }
        }
        else {
            if (vStation->next->next!=NULL) {
                printf("\t|%d %s \n",vStation->next->data.id,getNameStationById(vStation->next->data.id));
            }
            else{
                couleur("33");
                printf(" |%d %s \n",vStation->next->data.id,getNameStationById(vStation->next->data.id));
                couleur("0");
            }
            
        }
        vStation=vStation->next;    
    }
    couleur("35");
    printf("Votre trajet dure %d minutes\n",vStation->data.cost);
    couleur("0");    
}

//Cette fonction nous permet de faire l'affichage de l'algorithme
void affichageFinale(stationFinale* vStationFinale){
    addStation(vStationFinale->previous, vStationFinale->station->data);
    vStationFinale->previous=inverser(vStationFinale->previous);
    printList(vStationFinale->previous);
    freeListStation(vStationFinale->previous);
}

//Cette fonction affiche le logo de la RATP
void printFun(void){
    FILE *fileFun;
    //Ouvrir les deux fichiers en mode lecture
    fileFun = fopen(FILE_FUN, "rb");
    if (fileFun == NULL ) {//Vérifier existence fichier
        fprintf(stderr, "Error: file not found in current directory\n");
        assert(0);
    }
    char *ligne=malloc(sizeof(char));
    while (fgets(ligne, 100, fileFun)!=NULL) {
        couleur("32");
        printf("%s",ligne);
        couleur("0");
    }
    free(ligne);
}

//Cette fonction affiche le fichier de ids
void printFileId(void){
    FILE *fileId;
    //Ouvrir les deux fichiers en mode lecture
    fileId = fopen(FILE_ID_STATION, "rb");
    if (fileId == NULL ) {//Vérifier existence fichier
        fprintf(stderr, "Error: file not found in current directory\n");
        assert(0);
    }
    char *ligne=malloc(sizeof(char));
    fgets(ligne, 100, fileId);
    printf("%s",ligne);
    while (fgets(ligne, 100, fileId)!=NULL) {
        couleur("33");
        printf("%s",ligne);
        couleur("0");
    }
    free(ligne);
    fclose(fileId);
}

//Cette fonction affiche le menu et gère saisies de mauvaises valeus
void menu(void){
    uint32_t choice=0,lineFile=getFileElem();
    tabHashageListStation* vTabHashageListStation;
    vTabHashageListStation=newtabHLS(getFileElem());
    printFun();
    do{
        addStationTableHashage(vTabHashageListStation);
        uint32_t idDepart,idDestination;
        printf("\033[1;32m");
        couleur("32");
        printf("\n***********************************************\n\t\t\tMenu:\n");
        printf("\033[0m");
        couleur("32");
        printf("\n1-Ou allons-nous ?\n");
        printf("2-Affichez la liste des stations\n");
        printf("3-Quittez l'application\n");
        couleur("0");
        scanf("%d",&choice);
        switch (choice) {
            case 1:
                couleur("37");
                printf("Merci de choisir un id entre 1 - %d",lineFile);
                printf("\nMerci de saisir l'id de départ: ");
                scanf("%d",&idDepart);
                printf("\nMerci de saisir l'id de destination:");
                scanf("%d",&idDestination);
                printf("\n");
                couleur("0");
                if (idDestination==idDepart) {
                    couleur("31");
                    printf("Vous avez choisis le même id\n");
                    couleur("0");
                }
                else if(idDestination>lineFile || idDepart>lineFile || idDestination==0 || idDepart==0) {
                    couleur("31");
                    printf("Merci de Mettre le bon id\n");
                    couleur("0");
                }
                else {
                    printf("\033[H\033[2J");
                    affichageFinale(principal(vTabHashageListStation, idDepart, idDestination));
                    printf("Merci pour votre confiance et à très bientôt");
                    
                }
                break;
            case 2:
                printFileId();
            break;
            case 3:
                couleur("32");
                printf("Au revoir\n");
                couleur("0");
            break;
            default:
                couleur("31");
                printf("Merci de donner la bonne valeur\n");
                couleur("0");
            break;
            
        }
    }while(choice!=3);
    freeThLS(vTabHashageListStation);
}