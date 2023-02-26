//Les données dans la structure du maillon
//Nous avons expliquer ce choix dans le rapport
typedef struct {
    uint32_t id;
    char* nameLigne;
    uint32_t cost;
    _Bool change;
} dataStation;

//La structure du maillon station
typedef struct _station{
    dataStation data;
    struct _station* next;
}station;

//La strucutre de la liste de station
typedef struct{
    uint32_t size;
    station* head;
}listStation;

//La structure de station utilisé dans l'algo de minimisation
typedef struct _stationFinale{
    station *station;
    struct _stationFinale* next;
    listStation *previous;
}stationFinale;

//La structure de la liste de station finale utilisée dans l'algo
typedef struct {
    uint32_t size;
    stationFinale *head;
} listStationFinale;

//La strucutre de la table de hashage
typedef struct {
    uint32_t size;
    listStation** tab;
}tabHashageListStation;