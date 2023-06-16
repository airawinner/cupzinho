#include <GL/freeglut.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <stdio.h>
#include "SOIL/SOIL.h"
#include <string.h>
#define QUADRADO_SIZE 15
#define LARGURA_MUNDO 600
#define ALTURA_MUNDO 600
#define TIROPROTAGONISTA_SIZE 200
#define MAX_QUADRADOS 1 //define uma quantidade máxima de tiros

#define SKY_SPEED 0.0001  // Velocidade de movimento do céu


// Variáveis de controle
GLuint texturaCeu;  // Texturas dos céus
GLuint idTexturaNovoJogoMenu;
GLuint idTexturaNovoJogoCompleta;
GLuint idTexturaInstrucoes;
GLuint idTexturaNevoa[5];
GLuint idTexturaMostrarInstrucoes;
GLuint listaNovoJogoNormal;
GLuint listaNovoJogoAlterado;
GLuint listaInstrucoesNormal;
GLuint listaInstrucoesAlterada;
GLfloat PosicaoCeu = 0.0;  // Posição vertical do céu
GLuint texturaNuvem1; // ID da textura fase 1
GLuint texturaNuvem2; // ID da textura fase 2 
GLuint texturaNuvem3; // ID da textura fase 3
GLuint texturaNuvem4; // ID da textura gameover
GLuint texturaFinal; // ID da textura final



float rectPosX[MAX_QUADRADOS];
float rectPosY[MAX_QUADRADOS];
int numQuadrados = 0;

int  colisaoSize=TIROPROTAGONISTA_SIZE;
float targetX = 0.0f;
float targetY = 0.0f;
bool targetReached = false;
float nuvemY = 1.1f; 
//Cenas e menu
struct
{
    int menu,gameOver,instrucoes,finalVitoria;
    int fase_1,fase_2,fase_3;
    int abertura_fase_1,abertura_fase_2,abertura_fase_3;
}cena;
struct
{
    int emCimaNovoJogo,emCimaInstrucao;
    int cliqueNovoJogo,cliqueInstrucao;
}acoesMenu;

//structs da plataforma
struct{
    float red,green,blue;
}rgbPLataforma;
struct{
    int x;
    int y;
}platParada[4][4];

struct{
   int y1;
   int y2;
   int y3;
   int y4;
   int y5;
   int y6;
}yPlatParada;

//vida e pontuação
int points;
struct {
    GLuint barraDeVidas;
    float x, y;
    float largura, altura;
    float spriteX, spriteY;
    int vidatotal;
    float Danos;
    int invulneravel;
}vida;


//Inimigos
struct{
    float x, y;
    float largura, altura;
    float spriteX, spriteY;
    float movx, movy;
    float rotate;
    int libera;
    GLuint torpedo;
    int vida;
}torpedo;

struct{
    GLuint projetil;
    float x, y;
    float altura, largura;
    float spriteX, spriteY;
    float movx, movy;
    float xper; //centro do personagem
    float yper; //centro do personagem
    int libera;
    int vida;
}passaroQuePersegue;

struct{
    GLuint inimigo;
    float x, y;
    float largura, altura;
    float spriteX, spriteY;
    float movx, movy;
    int ataque;
    int libera;
    float xper;
    float yper;
    int vida;
}abelha;

struct{
    GLuint projetil;
    float movx, movy;
    float x, y;
    float altura, largura;
    float spriteX, spriteY;
    int vida;
}ataqueAbelha;

struct{
    int auxNevoa;
    int texNevoa;
}controlNevoa;

struct {
    int x, y, largura, altura;
    double spriteX, spriteY;
    GLuint passaro;
    int vida;
} passaro1;

struct {
    int x, y, largura, altura;
    double spriteX, spriteY;
    GLuint passaro;
    int libera;
    int vida;
} passaro2;

struct {
    int x, y, largura, altura;
    double spriteX, spriteY;
    GLuint passaro;
    int vida;
} passaro3;

struct {
    float posX, posY, deslocamentoemX, deslocamentoemY;
    float spriteX, spriteY;
    int largura, altura;
    GLuint textura;
    GLuint Enemy;
    float rotacao;
    int spriteIndex;
    float velocidade;
    GLuint estrelinha;
    float TranslateY;
    float TranslateX;
    int vida;
} boss;

struct {
    int x, y, largura, altura;
    double spriteX, spriteY;
    GLuint protagonista;
    bool isFalling;            //Indica se o personagem está caindo após pular
    bool isJumping;           // Indica se o personagem está pulando
    bool isFreeFall;          //Indica se o personagem saiu de uma plataformae entrou em "queda livre"
    float velocityX;         // Velocidade horizontal do personagem
    float velocityY;         // Velocidade vertical do personagem
} protagonista;
struct 
{   
    float movx, movy;
    float x, y;
    float altura, largura;
    int cima,baixo,direita,esquerda;
    int libera;
     
} tiroProtagonista;

struct{
    int w,s,a,d;

}teclado;

 struct {
    float x;        // Posição x do quadrado
    float y;        // Posição y do quadrado
    float speed;    // Velocidade do quadrado
    bool active;  
    GLuint TexturaTiro;     // Indica se o quadrado está ativo ou não
} tiroBoss;

int numTiros = 5;    

float angle = 0.0f; 

bool keyPressed = false;
float velocidade = 100.0f;
int direcaoX = 0; // -1 para esquerda, 1 para direita, 0 para nenhum
int direcaoY = 0; // -1 para baixo, 1 para cima, 0 para nenhum

int spriteIndex = 0;
int frameInterval = 120; // Intervalo de 120ms para 8 frames por segundo
float texCoordX = 0.0f;
float texCoordY = 0.8f;

int invertAnimationDirection = 0; // Variável para controlar a direção da animação
int lineIndex = 3; // Variável para controlar a linha dos quadradinhos

int frameCount = 0;
int ataqueEnemy=0;

char frasePontos[6];

GLuint carregaTextura(const char* arquivo) {
    GLuint idTextura = SOIL_load_OGL_texture(
        arquivo,
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_INVERT_Y
    );

    if (idTextura == 0) {
        printf("Erro do SOIL: '%s'\n", SOIL_last_result());
    }

    return idTextura;
}

//Lista novo jogoo normal
void carregaListaNovoJogoNormal(){
    listaNovoJogoNormal = glGenLists(1);
    glNewList(listaNovoJogoNormal, GL_COMPILE);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, idTexturaNovoJogoCompleta);
            glBegin(GL_TRIANGLE_FAN);
                glTexCoord2f(0,0);
                glVertex3f(100, 350, 0);
                glTexCoord2f(0,1);
                glVertex3f(100, 500, 0);
                glTexCoord2f(0.5,1);
                glVertex3f(500, 500, 0);
                glTexCoord2f(0.5,0);
                glVertex3f(500, 350, 0);
            glEnd();
        glDisable(GL_TEXTURE_2D); 
    glEndList();
}
//Lista novo jogo mouse por cima
void carregaListaNovoJogoAlterado(){
    listaNovoJogoAlterado = glGenLists(1);
    glNewList(listaNovoJogoAlterado, GL_COMPILE);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, idTexturaNovoJogoCompleta);
            glBegin(GL_TRIANGLE_FAN);
                glTexCoord2f(0.5,0);
                glVertex3f(100, 350, 0);
                glTexCoord2f(0.5,1);
                glVertex3f(100, 500, 0);
                glTexCoord2f(1,1);
                glVertex3f(500, 500, 0);
                glTexCoord2f(1,0);
                glVertex3f(500, 350, 0);
            glEnd();
        glDisable(GL_TEXTURE_2D); 
    glEndList();
}
//Lista instucoes normal
void carregaListaInstrucoesNormal(){
    listaInstrucoesNormal = glGenLists(1);
    glNewList(listaInstrucoesNormal, GL_COMPILE);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, idTexturaInstrucoes);
            glBegin(GL_TRIANGLE_FAN);
                glTexCoord2f(0,0);
                glVertex3f(100, 100, 0);
                glTexCoord2f(0,1);
                glVertex3f(100, 250, 0);
                glTexCoord2f(0.5,1);
                glVertex3f(500, 250, 0);
                glTexCoord2f(0.5,0);
                glVertex3f(500, 100, 0);
            glEnd();
        glDisable(GL_TEXTURE_2D); 
    glEndList();
}
//Lista instrucoes mouse por cima
void carregaListaInstrucoesAlterada(){
    listaInstrucoesAlterada = glGenLists(1);
    glNewList(listaInstrucoesAlterada, GL_COMPILE);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, idTexturaInstrucoes);
            glBegin(GL_TRIANGLE_FAN);
                glTexCoord2f(0.5,0);
                glVertex3f(100, 100, 0);
                glTexCoord2f(0.5,1);
                glVertex3f(100, 250, 0);
                glTexCoord2f(1,1);
                glVertex3f(500, 250, 0);
                glTexCoord2f(1,0);
                glVertex3f(500, 100, 0);
            glEnd();
        glDisable(GL_TEXTURE_2D); 
    glEndList();
}

//inicializa o opengl
void init() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}


int primo(int num){
    for(int i=2; i<num/2; i++){
        if((num%i)==0){
            return 0;
        }
    }
    return 1;
}

void setup() {
    glClearColor(1, 1, 1, 0); // branco
    
    platParada[0][0].x=40;
    platParada[0][0].y=-5;
    platParada[0][1].x=100;
    platParada[0][1].y=-5;
    platParada[0][2].x=100;
    platParada[0][2].y=5;
    platParada[0][3].x=40;
    platParada[0][3].y=5;

    platParada[1][0].x=190;
    platParada[1][0].y=-5;
    platParada[1][1].x=250;
    platParada[1][1].y=-5;
    platParada[1][2].x=250;
    platParada[1][2].y=5;
    platParada[1][3].x=190;
    platParada[1][3].y=5;

    platParada[2][0].x=340;
    platParada[2][0].y=-5;
    platParada[2][1].x=400;
    platParada[2][1].y=-5;
    platParada[2][2].x=400;
    platParada[2][2].y=5;
    platParada[2][3].x=340;
    platParada[2][3].y=5;

    platParada[3][0].x=490;
    platParada[3][0].y=-5;
    platParada[3][1].x=550;
    platParada[3][1].y=-5;
    platParada[3][2].x=550;
    platParada[3][2].y=5;
    platParada[3][3].x=490;
    platParada[3][3].y=5;

    yPlatParada.y1 = 570;
    yPlatParada.y2 = 470;
    yPlatParada.y3 = 370;
    yPlatParada.y4 = 270;
    yPlatParada.y5 = 170;
    yPlatParada.y6 = 70;

    rgbPLataforma.blue=0;
    rgbPLataforma.green=0;
    rgbPLataforma.red=1;
    
}

void plataformaParada(){
    glPushMatrix();
        glTranslatef(0, yPlatParada.y1, 0);
        glColor3f(rgbPLataforma.red,rgbPLataforma.green,rgbPLataforma.blue);
            glBegin(GL_TRIANGLE_FAN);
            glVertex3f(platParada[0][0].x, platParada[0][0].y, 0);
            glVertex3f(platParada[0][1].x, platParada[0][1].y, 0);
            glVertex3f(platParada[0][2].x, platParada[0][2].y, 0);
            glVertex3f(platParada[0][3].x, platParada[0][3].y, 0);
        glEnd();

        glBegin(GL_TRIANGLE_FAN);
            glVertex3f(platParada[1][0].x, platParada[1][0].y, 0);
            glVertex3f(platParada[1][1].x, platParada[1][1].y, 0);
            glVertex3f(platParada[1][2].x, platParada[1][2].y, 0);
            glVertex3f(platParada[1][3].x, platParada[1][3].y, 0);
        glEnd();

        glBegin(GL_TRIANGLE_FAN);
            glVertex3f(platParada[2][0].x, platParada[2][0].y, 0);
            glVertex3f(platParada[2][1].x, platParada[2][1].y, 0);
            glVertex3f(platParada[2][2].x, platParada[2][2].y, 0);
            glVertex3f(platParada[2][3].x, platParada[2][3].y, 0);
        glEnd();

        glBegin(GL_TRIANGLE_FAN);
            glVertex3f(platParada[3][0].x, platParada[3][0].y, 0);
            glVertex3f(platParada[3][1].x, platParada[3][1].y, 0);
            glVertex3f(platParada[3][2].x, platParada[3][2].y, 0);
            glVertex3f(platParada[3][3].x, platParada[3][3].y, 0);
        glEnd();
    glPopMatrix();
    /////////////////////
    glPushMatrix();
        glTranslatef(0, yPlatParada.y2, 0);
        glColor3f(rgbPLataforma.red,rgbPLataforma.green,rgbPLataforma.blue);
            glBegin(GL_TRIANGLE_FAN);
            glVertex3f(platParada[0][0].x, platParada[0][0].y, 0);
            glVertex3f(platParada[0][1].x, platParada[0][1].y, 0);
            glVertex3f(platParada[0][2].x, platParada[0][2].y, 0);
            glVertex3f(platParada[0][3].x, platParada[0][3].y, 0);
        glEnd();

        glBegin(GL_TRIANGLE_FAN);
            glVertex3f(platParada[1][0].x, platParada[1][0].y, 0);
            glVertex3f(platParada[1][1].x, platParada[1][1].y, 0);
            glVertex3f(platParada[1][2].x, platParada[1][2].y, 0);
            glVertex3f(platParada[1][3].x, platParada[1][3].y, 0);
        glEnd();

        glBegin(GL_TRIANGLE_FAN);
            glVertex3f(platParada[2][0].x, platParada[2][0].y, 0);
            glVertex3f(platParada[2][1].x, platParada[2][1].y, 0);
            glVertex3f(platParada[2][2].x, platParada[2][2].y, 0);
            glVertex3f(platParada[2][3].x, platParada[2][3].y, 0);
        glEnd();

        glBegin(GL_TRIANGLE_FAN);
            glVertex3f(platParada[3][0].x, platParada[3][0].y, 0);
            glVertex3f(platParada[3][1].x, platParada[3][1].y, 0);
            glVertex3f(platParada[3][2].x, platParada[3][2].y, 0);
            glVertex3f(platParada[3][3].x, platParada[3][3].y, 0);
        glEnd();
    glPopMatrix();
    /////////////////////
    glPushMatrix();
        glTranslatef(0, yPlatParada.y3, 0);
        glColor3f(rgbPLataforma.red,rgbPLataforma.green,rgbPLataforma.blue);
            glBegin(GL_TRIANGLE_FAN);
            glVertex3f(platParada[0][0].x, platParada[0][0].y, 0);
            glVertex3f(platParada[0][1].x, platParada[0][1].y, 0);
            glVertex3f(platParada[0][2].x, platParada[0][2].y, 0);
            glVertex3f(platParada[0][3].x, platParada[0][3].y, 0);
        glEnd();

        glBegin(GL_TRIANGLE_FAN);
            glVertex3f(platParada[1][0].x, platParada[1][0].y, 0);
            glVertex3f(platParada[1][1].x, platParada[1][1].y, 0);
            glVertex3f(platParada[1][2].x, platParada[1][2].y, 0);
            glVertex3f(platParada[1][3].x, platParada[1][3].y, 0);
        glEnd();

        glBegin(GL_TRIANGLE_FAN);
            glVertex3f(platParada[2][0].x, platParada[2][0].y, 0);
            glVertex3f(platParada[2][1].x, platParada[2][1].y, 0);
            glVertex3f(platParada[2][2].x, platParada[2][2].y, 0);
            glVertex3f(platParada[2][3].x, platParada[2][3].y, 0);
        glEnd();

        glBegin(GL_TRIANGLE_FAN);
            glVertex3f(platParada[3][0].x, platParada[3][0].y, 0);
            glVertex3f(platParada[3][1].x, platParada[3][1].y, 0);
            glVertex3f(platParada[3][2].x, platParada[3][2].y, 0);
            glVertex3f(platParada[3][3].x, platParada[3][3].y, 0);
        glEnd();
    glPopMatrix();
    /////////////////////
    glPushMatrix();
        glTranslatef(0, yPlatParada.y4, 0);
        glColor3f(rgbPLataforma.red,rgbPLataforma.green,rgbPLataforma.blue);
            glBegin(GL_TRIANGLE_FAN);
            glVertex3f(platParada[0][0].x, platParada[0][0].y, 0);
            glVertex3f(platParada[0][1].x, platParada[0][1].y, 0);
            glVertex3f(platParada[0][2].x, platParada[0][2].y, 0);
            glVertex3f(platParada[0][3].x, platParada[0][3].y, 0);
        glEnd();

        glBegin(GL_TRIANGLE_FAN);
            glVertex3f(platParada[1][0].x, platParada[1][0].y, 0);
            glVertex3f(platParada[1][1].x, platParada[1][1].y, 0);
            glVertex3f(platParada[1][2].x, platParada[1][2].y, 0);
            glVertex3f(platParada[1][3].x, platParada[1][3].y, 0);
        glEnd();

        glBegin(GL_TRIANGLE_FAN);
            glVertex3f(platParada[2][0].x, platParada[2][0].y, 0);
            glVertex3f(platParada[2][1].x, platParada[2][1].y, 0);
            glVertex3f(platParada[2][2].x, platParada[2][2].y, 0);
            glVertex3f(platParada[2][3].x, platParada[2][3].y, 0);
        glEnd();

        glBegin(GL_TRIANGLE_FAN);
            glVertex3f(platParada[3][0].x, platParada[3][0].y, 0);
            glVertex3f(platParada[3][1].x, platParada[3][1].y, 0);
            glVertex3f(platParada[3][2].x, platParada[3][2].y, 0);
            glVertex3f(platParada[3][3].x, platParada[3][3].y, 0);
        glEnd();
    glPopMatrix();
    /////////////////////
    glPushMatrix();
        glTranslatef(0, yPlatParada.y5, 0);
        glColor3f(rgbPLataforma.red,rgbPLataforma.green,rgbPLataforma.blue);
            glBegin(GL_TRIANGLE_FAN);
            glVertex3f(platParada[0][0].x, platParada[0][0].y, 0);
            glVertex3f(platParada[0][1].x, platParada[0][1].y, 0);
            glVertex3f(platParada[0][2].x, platParada[0][2].y, 0);
            glVertex3f(platParada[0][3].x, platParada[0][3].y, 0);
        glEnd();

        glBegin(GL_TRIANGLE_FAN);
            glVertex3f(platParada[1][0].x, platParada[1][0].y, 0);
            glVertex3f(platParada[1][1].x, platParada[1][1].y, 0);
            glVertex3f(platParada[1][2].x, platParada[1][2].y, 0);
            glVertex3f(platParada[1][3].x, platParada[1][3].y, 0);
        glEnd();

        glBegin(GL_TRIANGLE_FAN);
            glVertex3f(platParada[2][0].x, platParada[2][0].y, 0);
            glVertex3f(platParada[2][1].x, platParada[2][1].y, 0);
            glVertex3f(platParada[2][2].x, platParada[2][2].y, 0);
            glVertex3f(platParada[2][3].x, platParada[2][3].y, 0);
        glEnd();

        glBegin(GL_TRIANGLE_FAN);
            glVertex3f(platParada[3][0].x, platParada[3][0].y, 0);
            glVertex3f(platParada[3][1].x, platParada[3][1].y, 0);
            glVertex3f(platParada[3][2].x, platParada[3][2].y, 0);
            glVertex3f(platParada[3][3].x, platParada[3][3].y, 0);
        glEnd();
    glPopMatrix();
    /////////////////////
    glPushMatrix();
        glTranslatef(0, yPlatParada.y6, 0);
        glColor3f(rgbPLataforma.red,rgbPLataforma.green,rgbPLataforma.blue);
            glBegin(GL_TRIANGLE_FAN);
            glVertex3f(platParada[0][0].x, platParada[0][0].y, 0);
            glVertex3f(platParada[0][1].x, platParada[0][1].y, 0);
            glVertex3f(platParada[0][2].x, platParada[0][2].y, 0);
            glVertex3f(platParada[0][3].x, platParada[0][3].y, 0);
        glEnd();

        glBegin(GL_TRIANGLE_FAN);
            glVertex3f(platParada[1][0].x, platParada[1][0].y, 0);
            glVertex3f(platParada[1][1].x, platParada[1][1].y, 0);
            glVertex3f(platParada[1][2].x, platParada[1][2].y, 0);
            glVertex3f(platParada[1][3].x, platParada[1][3].y, 0);
        glEnd();

        glBegin(GL_TRIANGLE_FAN);
            glVertex3f(platParada[2][0].x, platParada[2][0].y, 0);
            glVertex3f(platParada[2][1].x, platParada[2][1].y, 0);
            glVertex3f(platParada[2][2].x, platParada[2][2].y, 0);
            glVertex3f(platParada[2][3].x, platParada[2][3].y, 0);
        glEnd();

        glBegin(GL_TRIANGLE_FAN);
            glVertex3f(platParada[3][0].x, platParada[3][0].y, 0);
            glVertex3f(platParada[3][1].x, platParada[3][1].y, 0);
            glVertex3f(platParada[3][2].x, platParada[3][2].y, 0);
            glVertex3f(platParada[3][3].x, platParada[3][3].y, 0);
        glEnd();
    glPopMatrix();
}

void analisaNevoa(int periodo){
    controlNevoa.auxNevoa++;
    if(controlNevoa.auxNevoa == 1){
        controlNevoa.texNevoa=4;
    }
    if(controlNevoa.auxNevoa == 2){
        controlNevoa.texNevoa=3;
    }
    if(controlNevoa.auxNevoa == 3){
        controlNevoa.texNevoa=2;
    }
    if(controlNevoa.auxNevoa == 4){
        controlNevoa.texNevoa=1;
    }
    if(controlNevoa.auxNevoa == 5){
        controlNevoa.texNevoa=0;
        controlNevoa.auxNevoa=0;
    }

    glutTimerFunc(periodo, analisaNevoa, periodo);
}

void analisaAtaqueAbelha(int periodo){
    if(ataqueAbelha.movy>-10){
        ataqueAbelha.movy-=4;
    }else{
        abelha.ataque = 0;
    }
    if(abelha.ataque == 1){
    glutTimerFunc(periodo, analisaAtaqueAbelha, periodo);
    }
}
void analisaTiroProtagonista(int periodo){
    if(tiroProtagonista.movy<610){
        tiroProtagonista.movy+=8;
    }else{
        tiroProtagonista.libera = 0;
    }
    if(tiroProtagonista.libera == 1){
    glutTimerFunc(0, analisaAtaqueAbelha, periodo);
    }
}

void analisaTorpedo(int periodo){
    if((torpedo.rotate == 0) && ((torpedo.movx+5) < 570)){
        torpedo.movx +=4;
    }
    if((torpedo.rotate == 0) && ((torpedo.movx+5) >= 570)){
        torpedo.rotate = 90;
    }
    if((torpedo.rotate == 90) && ((torpedo.movx+5) >= 570) && (torpedo.movy<70)){
        torpedo.movy +=4;
    }
    if((torpedo.rotate == 90) && ((torpedo.movx+5) >= 570) && (torpedo.movy>=70) && (torpedo.movy<140)){
        torpedo.rotate = 180;
    }
    if((torpedo.rotate == 180) && ((torpedo.movx+5) > 30)){
        torpedo.movx -=4;
    }
    if((torpedo.rotate == 180) && ((torpedo.movx+5) <= 30)){
        torpedo.rotate = 90;
    }
    if((torpedo.rotate == 90) && ((torpedo.movx+5) <= 30) && (torpedo.movy>=70) && (torpedo.movy<140)){
        torpedo.movy +=4;
    }
    if((torpedo.rotate == 90) && ((torpedo.movx+5) <= 30) && (torpedo.movy>=140) && (torpedo.movy<210)){
        torpedo.rotate = 0;
    }

    if((torpedo.rotate == 90) && ((torpedo.movx+5) >= 570) && (torpedo.movy>=140) && (torpedo.movy<210)){
        torpedo.movy +=4;
    }
    if((torpedo.rotate == 90) && ((torpedo.movx+5) >= 570) && (torpedo.movy>=210) && (torpedo.movy<280)){
        torpedo.rotate = 180;
    }

    if((torpedo.rotate == 90) && ((torpedo.movx+5) <= 30) && (torpedo.movy>=210) && (torpedo.movy<280)){
        torpedo.movy +=4;
    }

    if((torpedo.rotate == 90) && ((torpedo.movx+5) <= 30) && (torpedo.movy>=280) && (torpedo.movy<350)){
        torpedo.rotate = 0;
    }

    if((torpedo.rotate == 90) && ((torpedo.movx+5) >= 570) && (torpedo.movy>=280) && (torpedo.movy<350)){
        torpedo.movy +=4;
    }

    if((torpedo.rotate == 90) && ((torpedo.movx+5) >= 570) && (torpedo.movy>=350) && (torpedo.movy<420)){
        torpedo.rotate = 180;
    }

    if((torpedo.rotate == 90) && ((torpedo.movx+5) <= 30) && (torpedo.movy>=350) && (torpedo.movy<420)){
        torpedo.movy +=4;
    }
    //  
    if((torpedo.rotate == 90) && ((torpedo.movx+5) <= 30) && (torpedo.movy>=420) && (torpedo.movy<490)){
        torpedo.rotate = 0;
    }
    if((torpedo.rotate == 90) && ((torpedo.movx+5) >= 570) && (torpedo.movy>=420) && (torpedo.movy<490)){
        torpedo.movy +=4;
    }
    if((torpedo.rotate == 90) && ((torpedo.movx+5) >= 570) && (torpedo.movy>=490) && (torpedo.movy<560)){
        torpedo.rotate = 180;
    }
    //
    if((torpedo.rotate == 90) && ((torpedo.movx+5) <= 30) && (torpedo.movy>=490) && (torpedo.movy<560)){
        torpedo.movy +=4;
    }
    if((torpedo.rotate == 90) && ((torpedo.movx+5) <= 30) && (torpedo.movy>=560) && (torpedo.movy<590)){
        torpedo.rotate = 0;
    }
    //
    if((torpedo.rotate == 90) && ((torpedo.movx+5) >= 570) && (torpedo.movy>=560) && (torpedo.movy<620)){
        torpedo.movy +=4;
    }
    if(torpedo.movy>=620){
        torpedo.libera = 0;
    }

    if(torpedo.libera == 1){
     glutTimerFunc(periodo, analisaTorpedo, periodo);
    }
}
void analisaPassaro2(int periodo){
    if(passaro2.x<600){
        passaro2.x+=10.0f;
    }
    if(passaro2.x>=600){
        passaro2.x=0.0;
    }
    if(passaro2.libera == 1){
     glutTimerFunc(periodo, analisaPassaro2, periodo);
    }
}

void analisaAbelha(int periodo){
    if(abelha.movx > -20){
        abelha.movx-=0.5;
    }else{
        abelha.libera = 0;
        
    }

    if(((abelha.movx >= abelha.xper-5) && (abelha.movx <=abelha.xper+5)) && (abelha.ataque == 0)){ // colocar no lugar de abelha.xper a posição x do personagem
        ataqueAbelha.movx = abelha.xper;
        ataqueAbelha.movy = abelha.movy;
        abelha.ataque = 1;
        abelha.xper-=100;
        
        glutTimerFunc(0, analisaAtaqueAbelha,16);
    }
    if(abelha.xper==0){
        abelha.xper=(float)LARGURA_MUNDO;
    }
    if(abelha.libera == 1){
        
        glutTimerFunc(periodo, analisaAbelha, periodo);
    }
}
void analisaPassaroQuePersegue(int periodo){
    passaroQuePersegue.movx += (passaroQuePersegue.xper - passaroQuePersegue.movx)/100.0f;
    passaroQuePersegue.movy += (passaroQuePersegue.yper - passaroQuePersegue.movy)/100.0f;

    if((passaroQuePersegue.movx >= passaroQuePersegue.xper-15) && (passaroQuePersegue.movx <= passaroQuePersegue.xper+15)){
        passaroQuePersegue.libera =0;
    }

    if(passaroQuePersegue.libera == 1){
    glutTimerFunc(periodo, analisaPassaroQuePersegue, periodo);
    }
}


void desenhaTiroBoss() {
    glPushMatrix();
    glScalef(1.0f, 1.0f, 1.0f);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tiroBoss.TexturaTiro);

    for (int i = 0; i < numQuadrados; i++) {
        glPushMatrix();
        glTranslatef(rectPosX[i], rectPosY[i], 0.0f);
        
        glBegin(GL_QUADS);
        
        glTexCoord2f(0.0f, 0.0f);
        glVertex2f(-QUADRADO_SIZE, -QUADRADO_SIZE);
        
        glTexCoord2f(1.0f, 0.0f);
        glVertex2f(QUADRADO_SIZE, -QUADRADO_SIZE);
        
        glTexCoord2f(1.0f, 1.0f);
        glVertex2f(QUADRADO_SIZE, QUADRADO_SIZE);
        
        glTexCoord2f(0.0f, 1.0f);
        glVertex2f(-QUADRADO_SIZE, QUADRADO_SIZE);
        
        glEnd();
        
        glPopMatrix();
    }
    
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

//alguma sprite do Boss
void trocarSprite(int value) {
    int spriteIndexX = spriteIndex % 5; // Índice da coluna
    int spriteIndexY = spriteIndex / 5; // Índice da linha

    texCoordX = spriteIndexX * 0.2f;
    texCoordY = spriteIndexY * 0.2f;

    if (lineIndex == 1 && spriteIndexY == 0 && spriteIndexX == 4) {
        // Se estiver na última sprite da linha 1, mude para a linha 2
        lineIndex = 2;
        spriteIndexY = 1;
        spriteIndexX = 0;
    } else if (lineIndex == 2 && spriteIndexY == 1 && spriteIndexX == 4) {
        // Se estiver na última sprite da linha 2, mude para a linha 3
        lineIndex = 3;
        spriteIndexY = 2;
        spriteIndexX = 0;
    } else {
        spriteIndex = (spriteIndex + 1) % 5; // Altera o índice do sprite (de 0 a 4)

        if (spriteIndex == 0) {
            // Inverte a direção da animação ao chegar na última sprite
            invertAnimationDirection = !invertAnimationDirection;
        }

        if (invertAnimationDirection) {
            spriteIndex = 4 - spriteIndex; // Inverte o índice para animação reversa
        }

        spriteIndexX = spriteIndex % 5; // Atualiza o índice da coluna
        spriteIndexY = spriteIndex / 5; // Atualiza o índice da linha
    }

    if (lineIndex == 2) {
        // Se lineIndex for igual a 2, ajusta as coordenadas de textura para a linha 2
        texCoordY = 0.6f;
    } else if (lineIndex == 3) {
        // Se lineIndex for igual a 3, ajusta as coordenadas de textura para a linha 3
        texCoordY = 0.4f;
    } else {
        // Caso contrário, mantém as coordenadas de textura para a linha 1 (valor padrão)
        texCoordY = 0.8f;
    }

        
    glutPostRedisplay();
       
    glutTimerFunc(frameInterval, trocarSprite, 0);
}



// Função de callback para o evento de desenho
void RenderizaBoss() {
    //glDisable(GL_TEXTURE_2D); 
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, boss.textura);
    glTranslatef(boss.TranslateX, boss.TranslateY, 0);

    glBegin(GL_QUADS);

    // Coordenadas de textura para os cinco quadrados
    texCoordX = spriteIndex * 0.2f;

    glTexCoord2f(texCoordX, texCoordY);
    glVertex2f(-boss.largura / 2, -boss.altura / 2);

    glTexCoord2f(texCoordX + 0.2f, texCoordY);
    glVertex2f(boss.largura / 2, -boss.altura / 2);

    glTexCoord2f(texCoordX + 0.2f, texCoordY + 0.15f);
    glVertex2f(boss.largura / 2, boss.altura / 2);

    glTexCoord2f(texCoordX, texCoordY + 0.2f);
    glVertex2f(-boss.largura / 2, boss.altura / 2);

    glDisable(GL_TEXTURE_2D);
    glEnd();
    glPopMatrix();
    
    
    //isso é para usar onde o boss começa para verificar a colisão com os tiros e com o protagonista
    boss.posX=boss.TranslateX-(boss.largura/2);
    boss.posY=boss.TranslateY-(boss.altura/2);

}
void desenhaProtagonista(){
    //glTranslatef(protagonista.x, protagonista.y, 0.0f);
    
    if(protagonista.x>570)
        protagonista.x-=5;
    if(protagonista.x<0)
        protagonista.x+=5;
    if(protagonista.y>600)
        protagonista.y-=5;
     if(protagonista.y<0)
        protagonista.y+=5;

    glColor3f (1, 1, 1);
    glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, protagonista.protagonista);
        glBegin(GL_QUADS);
            glTexCoord2f(protagonista.spriteX, protagonista.spriteY); // canto inferior esquerdo da textura
            glVertex2f(protagonista.x, protagonista.y);

            glTexCoord2f(protagonista.spriteX + 1.0/11.0, protagonista.spriteY); // canto inferior direito da textura
            glVertex2f(protagonista.x + protagonista.largura, protagonista.y);

            glTexCoord2f(protagonista.spriteX + 1.0/11.0, protagonista.spriteY +1.0/3.0); // canto superior direito da textura
            glVertex2f(protagonista.x + protagonista.largura, protagonista.y + protagonista.altura);

            glTexCoord2f(protagonista.spriteX , protagonista.spriteY +1.0/3.0); // canto superior esquerdo da textura
            glVertex2f(protagonista.x, protagonista.y + protagonista.altura);
        glEnd();
    glDisable(GL_TEXTURE_2D);
}


void desenhaTiroProtagonista() {
    glPushMatrix();
    glTranslatef(tiroProtagonista.movx,tiroProtagonista.movy,0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, boss.Enemy);
        
        glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 0.0f);
            glVertex2f(0, 0);
            
            glTexCoord2f(1.0f, 0.0f);
            glVertex2f(TIROPROTAGONISTA_SIZE, 0);
            
            glTexCoord2f(1.0f, 1.0f);
            glVertex2f(TIROPROTAGONISTA_SIZE, TIROPROTAGONISTA_SIZE);
            
            glTexCoord2f(0.0f, 1.0f);
            glVertex2f(0, TIROPROTAGONISTA_SIZE);
            
            glEnd();

        glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

bool colisaoProtagonistaObstaculos(float inimigoX, float inimigoY, float protagonistaX, float protagonistaY, float inimigolargura, float inimigoAltura) {

    float protagonistaDir = protagonistaX + (float)protagonista.largura;
    float protagonistaCima = protagonistaY + (float)protagonista.altura;

    if((protagonistaX>=inimigoX&&protagonistaX<=(inimigoX+inimigolargura))||(protagonistaDir>=inimigoX&&protagonistaDir<=(inimigoX+inimigolargura))){
        if((protagonistaY>=inimigoY&&protagonistaY<(inimigoY+inimigoAltura)||protagonistaCima>=inimigoY&&protagonistaCima<=(inimigoY+inimigoAltura))){
            return true;
        }   
    }
    if((inimigoX>=protagonistaX&&inimigoX<=protagonistaDir)||((inimigoX+inimigolargura)>=protagonistaX&&(inimigoX+inimigolargura)<=protagonistaDir)){
        if((inimigoY>=protagonistaY&&inimigoY<protagonistaCima||(inimigoY+inimigoAltura)>=protagonistaY&&(inimigoY+inimigoAltura)<=protagonistaCima)){
            return true;
        }  
    }

    return false; // Não há colisão
}

bool colisaoTiroProtagonistaAlvo(float inimigoX, float inimigoY, float tiroX, float tiroY, float inimigolargura, float inimigoAltura) {
    
    float tiroDir = tiroX + TIROPROTAGONISTA_SIZE/2;
    float tiroCima = tiroY + TIROPROTAGONISTA_SIZE/2;

    if((tiroX>=inimigoX&&tiroX<=(inimigoX+inimigolargura))||(tiroDir>=inimigoX&&tiroDir<=(inimigoX+inimigolargura))){
        if((tiroY>=inimigoY&&tiroY<(inimigoY+inimigoAltura)||tiroCima>=inimigoY&&tiroCima<=(inimigoY+inimigoAltura))){
            return true;
        }   
    }
    if((inimigoX>=tiroX&&inimigoX<=tiroDir)||((inimigoX+inimigolargura)>=tiroX&&(inimigoX+inimigolargura)<=tiroDir)){
        if((inimigoY>=tiroY&&inimigoY<tiroCima||(inimigoY+inimigoAltura)>=tiroY&&(inimigoY+inimigoAltura)<=tiroCima)){
            return true;
        }  
    }
    

    return false; // Não há colisão
}


void desenhaAbelha(){
    glPushMatrix();
        glTranslatef(abelha.movx, abelha.movy, 0);
        glColor3f(1.0, 1.0, 1.0);
        glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, abelha.inimigo);
            glBegin(GL_QUADS);
                glTexCoord2f(abelha.spriteX, abelha.spriteY); // canto inferior esquerdo da textura
                glVertex2f(abelha.x, abelha.y);

                glTexCoord2f(abelha.spriteX+0.15, abelha.spriteY); // canto inferior direito da textura
                glVertex2f(abelha.x + abelha.largura, abelha.y);

                glTexCoord2f(abelha.spriteX+0.15, abelha.spriteY+0.4); // canto superior direito da textura
                glVertex2f(abelha.x + abelha.largura, abelha.y + abelha.altura);

                glTexCoord2f(abelha.spriteX, abelha.spriteY+0.4); // canto superior esquerdo da textura
                glVertex2f(abelha.x, abelha.y + abelha.altura);
            glEnd();
        glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}


void desenhaAtaqueAbelha(){
    glPushMatrix();
        glTranslatef(ataqueAbelha.movx, ataqueAbelha.movy, 0);
        glColor3f(1.0, 1.0, 1.0);
        glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, ataqueAbelha.projetil);
            glBegin(GL_QUADS);
                glTexCoord2f(ataqueAbelha.spriteX, ataqueAbelha.spriteY); // canto inferior esquerdo da textura
                glVertex2f(ataqueAbelha.x, ataqueAbelha.y);

                glTexCoord2f(ataqueAbelha.spriteX+1.0, ataqueAbelha.spriteY); // canto inferior direito da textura
                glVertex2f(ataqueAbelha.x + ataqueAbelha.largura, ataqueAbelha.y);

                glTexCoord2f(ataqueAbelha.spriteX+1.0, ataqueAbelha.spriteY+1.0); // canto superior direito da textura
                glVertex2f(ataqueAbelha.x + ataqueAbelha.largura, ataqueAbelha.y + ataqueAbelha.altura);

                glTexCoord2f(ataqueAbelha.spriteX, ataqueAbelha.spriteY+1.0); // canto superior esquerdo da textura
                glVertex2f(ataqueAbelha.x, ataqueAbelha.y + ataqueAbelha.altura);
            glEnd();
        glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}


void desenhaLava(){
   glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, idTexturaNevoa[controlNevoa.texNevoa]);
            glColor3f(0.66,0.66,0.66);
            glBegin(GL_TRIANGLE_FAN);
                glTexCoord2f(0,0);
                glVertex3f(0, 0, 1);
                glTexCoord2f(1,0);
                glVertex3f(600, 0, 1);
                glTexCoord2f(1,1);
                glVertex3f(600, 40, 1);
                glTexCoord2f(0,1);
                glVertex3f(0, 40, 1);
            glEnd();
    glDisable(GL_TEXTURE_2D);
}
void desenhaPassaroQuePersegue(){
    glPushMatrix();
        glTranslatef(passaroQuePersegue.movx, passaroQuePersegue.movy, 0);
        glColor3f(1.0, 1.0, 1.0);
        glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, passaro1.passaro);
            glBegin(GL_QUADS);
                glTexCoord2f(passaroQuePersegue.spriteX, passaroQuePersegue.spriteY); // canto inferior esquerdo da textura
                glVertex2f(passaroQuePersegue.x, passaroQuePersegue.y);

                glTexCoord2f(passaroQuePersegue.spriteX+0.12, passaroQuePersegue.spriteY); // canto inferior direito da textura
                glVertex2f(passaroQuePersegue.x + passaroQuePersegue.largura, passaroQuePersegue.y);

                glTexCoord2f(passaroQuePersegue.spriteX+0.12, passaroQuePersegue.spriteY+0.13); // canto superior direito da textura
                glVertex2f(passaroQuePersegue.x + passaroQuePersegue.largura, passaroQuePersegue.y + passaroQuePersegue.altura);

                glTexCoord2f(passaroQuePersegue.spriteX, passaroQuePersegue.spriteY+0.13); // canto superior esquerdo da textura
                glVertex2f(passaroQuePersegue.x, passaroQuePersegue.y + passaroQuePersegue.altura);
            glEnd();
        glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}
void desenhaTorpedo(){
    glPushMatrix();
        glTranslatef(torpedo.movx, torpedo.movy, 0);
        glRotatef(torpedo.rotate,0,0,1);
        glColor3f(1.0, 1.0, 1.0);
        glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, torpedo.torpedo);
            glBegin(GL_QUADS);
                glTexCoord2f(torpedo.spriteX, torpedo.spriteY); // canto inferior esquerdo da textura
                glVertex2f(torpedo.x, torpedo.y);

                glTexCoord2f(torpedo.spriteX+1.0, torpedo.spriteY); // canto inferior direito da textura
                glVertex2f(torpedo.x + torpedo.largura, torpedo.y);

                glTexCoord2f(torpedo.spriteX+1.0, torpedo.spriteY+1.0); // canto superior direito da textura
                glVertex2f(torpedo.x + torpedo.largura, torpedo.y + torpedo.altura);

                glTexCoord2f(torpedo.spriteX, torpedo.spriteY+1.0); // canto superior esquerdo da textura
                glVertex2f(torpedo.x, torpedo.y + torpedo.altura);
            glEnd();
        glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}
void desenhaPassaro2(){
 glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, passaro2.passaro);
        glBegin(GL_QUADS);
        glTranslatef(passaro2.x,passaro2.y,0);
        glRotatef(angle,0,0,0);
            glTexCoord2f(passaro2.spriteX, passaro2.spriteY); // canto inferior esquerdo da textura
            glVertex2f(passaro2.x, passaro2.y);

            glTexCoord2f(passaro2.spriteX+0.1, passaro2.spriteY); // canto inferior direito da textura
            glVertex2f(passaro2.x + passaro2.largura, passaro2.y);

            glTexCoord2f(passaro2.spriteX+0.1, passaro2.spriteY+0.08); // canto superior direito da textura
            glVertex2f(passaro2.x + passaro2.largura, passaro2.y + passaro2.altura);

            glTexCoord2f(passaro2.spriteX, passaro2.spriteY+0.08); // canto superior esquerdo da textura
            glVertex2f(passaro2.x, passaro2.y + passaro2.altura);
        glEnd();
    glDisable(GL_TEXTURE_2D);
}

void TrocarFramesBoss(){
    
    frameCount++;
    if (frameCount >= 25 && lineIndex == 3) { // Troca de sprite a cada 25 frames (aproximadamente 1 segundo)
        frameCount = 0;
        spriteIndex = (spriteIndex + 1) % 5; // Altera o índice do sprite (de 0 a 4)
        glutPostRedisplay(); // Solicita o redesenho da cena
    } 
}

void trocacenasBoss(){
    if(lineIndex==3){
        if (!targetReached) {
                desenhaTiroBoss();   
                targetX=protagonista.x;
                targetY=protagonista.y;
                if (numQuadrados < MAX_QUADRADOS) {
                    rectPosX[numQuadrados] = boss.TranslateX;
                    rectPosY[numQuadrados] = boss.TranslateY;
                    numQuadrados++;
                } 

        }
    }
 glutTimerFunc(1800,trocacenasBoss,0);
 glutPostRedisplay();
}
void desenhaPortempoLine3(){
     lineIndex=3;
     trocacenasBoss();
    glutTimerFunc(1800,desenhaPortempoLine3,0);
}
void desenhaPortempo(){
    lineIndex=0;
    trocacenasBoss();
    glutTimerFunc(9000,desenhaPortempo,0);
}

void desenhaInimigos(){

    if(torpedo.libera == 1&&torpedo.vida==1){
        desenhaTorpedo();
    }
    if(passaroQuePersegue.libera == 1 && passaroQuePersegue.vida==1){
        desenhaPassaroQuePersegue();
    }
    if(abelha.libera == 1&&(cena.fase_2==1||cena.fase_3==1)&&abelha.vida==1){//acrescentrar no if a variavel de mudança de fase. Esse inimigo pertense a fase 2.
        desenhaAbelha(); 
        if(abelha.ataque == 1){
            desenhaAtaqueAbelha();
        }
    }
    if(passaro2.libera==1&&passaro2.vida==1){
        desenhaPassaro2();
    }
    if(cena.fase_3==1){
        if(boss.vida){
            TrocarFramesBoss();
            RenderizaBoss();
           trocacenasBoss();
        
        
        }
    }
    
    //Passaros
    if(passaro1.vida==1){
        glColor3f(1.0, 1.0, 1.0);
        glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, passaro1.passaro);
            glBegin(GL_QUADS);
                glTexCoord2f(passaro1.spriteX, passaro1.spriteY); // canto inferior esquerdo da textura
                glVertex2f(passaro1.x, passaro1.y);

                glTexCoord2f(passaro1.spriteX+0.12, passaro1.spriteY); // canto inferior direito da textura
                glVertex2f(passaro1.x + passaro1.largura, passaro1.y);

                glTexCoord2f(passaro1.spriteX+0.12, passaro1.spriteY+0.13); // canto superior direito da textura
                glVertex2f(passaro1.x + passaro1.largura, passaro1.y + passaro1.altura);

                glTexCoord2f(passaro1.spriteX, passaro1.spriteY+0.13); // canto superior esquerdo da textura
                glVertex2f(passaro1.x, passaro1.y + passaro1.altura);
            glEnd();
        glDisable(GL_TEXTURE_2D);
    }
    
    if(passaro3.vida==1){
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, passaro3.passaro);
        glBegin(GL_QUADS);
            glTexCoord2f(passaro3.spriteX, passaro3.spriteY); // canto inferior esquerdo da textura
            glVertex2f(passaro3.x, passaro3.y);

            glTexCoord2f(passaro3.spriteX+0.11, passaro3.spriteY); // canto inferior direito da textura
            glVertex2f(passaro3.x + passaro3.largura, passaro3.y);

            glTexCoord2f(passaro3.spriteX+0.11, passaro3.spriteY+0.1); // canto superior direito da textura
            glVertex2f(passaro3.x + passaro3.largura, passaro3.y + passaro3.altura);

            glTexCoord2f(passaro3.spriteX, passaro3.spriteY+0.1); // canto superior esquerdo da textura
            glVertex2f(passaro3.x, passaro3.y + passaro3.altura);
        glEnd();
        glDisable(GL_TEXTURE_2D);
    }
    
}

void desenhaVidaTextura(){
    switch (vida.vidatotal) {
    case 1:
        vida.barraDeVidas = carregaTextura("texturas/barraDeVidas/barra1.png");
        break;
    case 2:
        vida.barraDeVidas = carregaTextura("texturas/barraDeVidas/barra2.png");
        break;
    case 3:
        vida.barraDeVidas = carregaTextura("texturas/barraDeVidas/barra3.png");
        break;
    case 4:
        vida.barraDeVidas = carregaTextura("texturas/barraDeVidas/barra4.png");
        break;
    case 5:
        vida.barraDeVidas = carregaTextura("texturas/barraDeVidas/barra5.png");
        break;
    case 6:
        vida.barraDeVidas = carregaTextura("texturas/barraDeVidas/barra6.png");
        break;
    case 7:
        vida.barraDeVidas = carregaTextura("texturas/barraDeVidas/barra7.png");
        break;
    case 8:
        vida.barraDeVidas = carregaTextura("texturas/barraDeVidas/barra8.png");
        break;
    case 9:
        vida.barraDeVidas = carregaTextura("texturas/barraDeVidas/barra9.png");
        break;
    case 10:
        vida.barraDeVidas = carregaTextura("texturas/barraDeVidas/barra10.png");
        break;
    }

    glColor3f(1.0, 1.0, 1.0);
    glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, vida.barraDeVidas);
        glBegin(GL_QUADS);
            glTexCoord2f(vida.spriteX, vida.spriteY); // canto inferior esquerdo da textura
            glVertex2f(vida.x, vida.y);

            glTexCoord2f(vida.spriteX+1.0, vida.spriteY); // canto inferior direito da textura
            glVertex2f(vida.x + vida.largura, vida.y);

            glTexCoord2f(vida.spriteX+1.0, vida.spriteY+1.0); // canto superior direito da textura
            glVertex2f(vida.x + vida.largura, vida.y + vida.altura);

            glTexCoord2f(vida.spriteX, vida.spriteY+1.0); // canto superior esquerdo da textura
            glVertex2f(vida.x, vida.y + vida.altura);
        glEnd();
    glDisable(GL_TEXTURE_2D);
}

void desenhaCenario(){

    glDisable(GL_TEXTURE_2D); 
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texturaCeu);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, PosicaoCeu);
    glVertex3f(0.0, 0.0,-1.0);

    glTexCoord2f(1.0, PosicaoCeu);
    glVertex3f(600, 0.0,-1.0);

    glTexCoord2f(1.0, PosicaoCeu + 1.0/3.0);
    glVertex3f(600, 600,-1.0);

    glTexCoord2f(0.0, PosicaoCeu + 1.0/3.0);
    glVertex3f(0.0, 600.0,-1.0);
    glEnd();

    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    
}



void alteraMenu(){

    glColor3f(1,1,1);
        glBegin(GL_TRIANGLE_FAN);
        glVertex3f(0, 0, 0);
        glVertex3f(600, 0, 0);
        glVertex3f(600, 600, 0);
        glVertex3f(0, 600, 0);
        glEnd();
        desenhaCenario();

    if(acoesMenu.emCimaNovoJogo==0){
        glCallList(listaNovoJogoNormal);
    } 
    
    if(acoesMenu.emCimaInstrucao==0){
        glCallList(listaInstrucoesNormal);
    }
    
    if(acoesMenu.emCimaInstrucao==1){
        glCallList(listaInstrucoesAlterada);
    }
    if(acoesMenu.emCimaNovoJogo==1){
        glCallList(listaNovoJogoAlterado);
    }
    
    glutPostRedisplay();
}
void cenaAberturas() {
    if (cena.abertura_fase_1 == 1) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texturaNuvem1);
        glBegin(GL_TRIANGLE_FAN);
        glTexCoord2f(0 - 0.03f, 1 - nuvemY * 1.7); glVertex3f(0, 0, 0);
        glTexCoord2f(1 - 0.03f, 1 - nuvemY * 1.7); glVertex3f(600, 0, 0);
        glTexCoord2f(1 - 0.03f, 1 - (nuvemY - 1.25f)); glVertex3f(600, 600, 0);
        glTexCoord2f(0 - 0.03f, 1 - (nuvemY - 1.25f)); glVertex3f(0, 600, 0);
        glEnd();
        glDisable(GL_TEXTURE_2D);
        nuvemY -= 0.001f;
    }

    if (cena.abertura_fase_2 == 1) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texturaNuvem2);
        glBegin(GL_TRIANGLE_FAN);
        glTexCoord2f(0 - 0.01f, 1 - nuvemY * 2); glVertex3f(0, 0, 0);
        glTexCoord2f(1 - 0.01f, 1 - nuvemY * 2); glVertex3f(600, 0, 0);
        glTexCoord2f(1 - 0.01f, 1 - (nuvemY - 1.25f)); glVertex3f(600, 600, 0);
        glTexCoord2f(0 - 0.01f, 1 - (nuvemY - 1.25f)); glVertex3f(0, 600, 0);
        glEnd();
        glDisable(GL_TEXTURE_2D);
        nuvemY -= 0.001f;
    }

    if (cena.abertura_fase_3 == 1) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texturaNuvem3);
        glBegin(GL_TRIANGLE_FAN);
        glTexCoord2f(0 - 0.03f, 1 - nuvemY*1.7); glVertex3f(0, 0, 0);
        glTexCoord2f(1 - 0.03f, 1 - nuvemY*1.7); glVertex3f(600, 0, 0);
        glTexCoord2f(1 - 0.03f, 1 - (nuvemY - 1.25f)); glVertex3f(600, 600, 0);
        glTexCoord2f(0 - 0.03f, 1 - (nuvemY - 1.25f)); glVertex3f(0, 600, 0);
        glEnd();
        glDisable(GL_TEXTURE_2D);
        nuvemY -= 0.001f;
    }

    if (cena.finalVitoria == 1) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texturaFinal);
        glBegin(GL_TRIANGLE_FAN);
        glTexCoord2f(0, 1 - nuvemY); glVertex3f(0, 0, 0);
        glTexCoord2f(1, 1 - nuvemY); glVertex3f(600, 0, 0);
        glTexCoord2f(1, 1 - (nuvemY - 1.0f)); glVertex3f(600, 600, 0);
        glTexCoord2f(0, 1 - (nuvemY - 1.0f)); glVertex3f(0, 600, 0);
        glEnd();
        glDisable(GL_TEXTURE_2D);
        nuvemY -= 0.0005f;
    }
}

void CenasNiveis(){
    //glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(1.0, 1.0, 1.0, 1.0); // Define a cor de fundo como branco (RGB: 1, 1, 1)
    glColor3f(1.0, 1.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0,LARGURA_MUNDO,0, ALTURA_MUNDO,-1,1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    desenhaCenario();
    glColor3f(1.0f, 1.0f, 1.0f);    

    //plataforma1();
    glColor3f(1.0f, 1.0f, 1.0f);    
    
    plataformaParada();
    desenhaLava();
    desenhaInimigos();
    desenhaProtagonista();

  

    if(tiroProtagonista.libera==1){
        desenhaTiroProtagonista();
        glutTimerFunc(0, analisaTiroProtagonista,100);
    }
    
    //glFlush();
    //glutSwapBuffers();
    glutPostRedisplay();
}

void desabilitaInstrucao(){
    cena.instrucoes=0;
}

void instrucao(){
    desenhaCenario();
    glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, idTexturaMostrarInstrucoes);
            glBegin(GL_TRIANGLE_FAN);
                glTexCoord2f(0,0);
                glVertex3f(0,0,0);
                glTexCoord2f(1,0);
                glVertex3f(600,0,0);
                glTexCoord2f(1,1);
                glVertex3f(600,600,0);
                glTexCoord2f(0,1);
                glVertex3f(0,600,0);
            glEnd();
        glDisable(GL_TEXTURE_2D);
    glutTimerFunc(10000, desabilitaInstrucao, 0);
}
void desenhagameOver(){
    glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texturaNuvem4);
        glBegin(GL_TRIANGLE_FAN);
        glTexCoord2f(0 , 1 - nuvemY*1.3); glVertex3f(0, 0, 0);
        glTexCoord2f(1 , 1 - nuvemY*1.3); glVertex3f(600, 0, 0);
        glTexCoord2f(1 , 1 - (nuvemY - 1.25f)); glVertex3f(600, 600, 0);
        glTexCoord2f(0 , 1 - (nuvemY - 1.25f)); glVertex3f(0, 600, 0);
        glDisable(GL_TEXTURE_2D);
        nuvemY -= 0.001f; 
        glEnd();
}
void pontos(int periodo){
    printf("\n\n\n\n%d",points);
    points++;
    int aux=points;
    int digitos[6]={0,0,0,0,0,0};
    int i=0;

    while(aux>1){
        if((aux/100)<1){
            digitos[i+1]=aux/10;
        }
        digitos[i]=aux%10;
        aux=aux/10;
        i++;
    }

    int j=5;
    for(int i=0;i<6;i++){
        if(digitos[i]==0){
            frasePontos[j]='0';
        }
        else if(digitos[i]==1){
            frasePontos[j]='1';
        }
        else if(digitos[i]==2){
            frasePontos[j]='2';
        }
        else if(digitos[i]==3){
            frasePontos[j]='3';
        }
        else if(digitos[i]==4){
            frasePontos[j]='4';
        }
        else if(digitos[i]==5){
            frasePontos[j]='5';
        }
        else if(digitos[i]==6){
            frasePontos[j]='6';
        }
        else if(digitos[i]==7){
            frasePontos[j]='7';
        }
        else if(digitos[i]==8){
            frasePontos[j]='8';
        }
        else if(digitos[i]==9){
            frasePontos[j]='9';
        }
        j--;
    }

    glutTimerFunc(periodo, pontos, periodo);
}
void escreveTexto(void* fonte, char* texto, float x, float y){
    glRasterPos2f(x,y);

    for(int i = 0; i < strlen(texto) ; i++){
        glutBitmapCharacter(fonte, texto[i]);
    }

}

void desenhaMinhaCena(){
   
    glClearColor(0,0,0,1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    if(cena.menu==1){
        alteraMenu();
    }
    if(cena.instrucoes==1){
        instrucao();
        
    }
    if(cena.abertura_fase_1 == 1|| cena.abertura_fase_2 == 1||cena.abertura_fase_3 == 1||cena.finalVitoria==1){
        cenaAberturas();
    }
    

    if(cena.fase_1==1||cena.fase_2==1||cena.fase_3==1){
        CenasNiveis();
        desenhaVidaTextura();
        escreveTexto(GLUT_BITMAP_HELVETICA_18, frasePontos , 10 , ALTURA_MUNDO-20);
        if(cena.fase_1==1){
            escreveTexto(GLUT_BITMAP_HELVETICA_18, "Objetivo: 800" , 10 , ALTURA_MUNDO-40);
        }
        else if(cena.fase_2==1){
            escreveTexto(GLUT_BITMAP_HELVETICA_18, "Objetivo: 1600" , 10 , ALTURA_MUNDO-40);
        }
        else if(cena.fase_3==1){
            escreveTexto(GLUT_BITMAP_HELVETICA_18, "Objetivo: 3000" , 10 , ALTURA_MUNDO-40);
        }

    }
    if(cena.gameOver==1){
        desenhagameOver();
    }
    
    glutSwapBuffers();
}


void enemyRandom(int periodo){ //randomizar o surgimento de inimigos, ideia com time.h usando números primos e resto de divisão
    int controle = rand()%201;
    if((primo(controle) == 1) && abelha.libera == 0){ //acrescentar a variavel de mudança de fase no if. Esse inimigo so aparece na fase 2.
        abelha.movx = 620; // colocoar no enemyRandom
        abelha.movy = 560; // colocoar no enemyRandom
        /*colocar para setar a posição do boneco aqui
        EX: abelha.xper = personagem.x*/
        abelha.libera = 1;
        glutTimerFunc(0, analisaAbelha, 16);
        
    }

    if(((controle>=142) && (controle<=200)) && passaroQuePersegue.libera == 0){
        if((controle%2) == 0){
            passaroQuePersegue.movx = (600+(rand()%21));
            passaroQuePersegue.movy = rand()%601;
        }
        if((controle%2)!=0){
            passaroQuePersegue.movx = (-(rand()%21));
            passaroQuePersegue.movy = rand()%601;
        }
        passaroQuePersegue.xper =  protagonista.x; //randomizar na enemyRandom
        passaroQuePersegue.yper = protagonista.y; //randomizar na enemyRandom
        passaroQuePersegue.libera = 1;
        glutTimerFunc(0, analisaPassaroQuePersegue, 16);
    }

    if(((controle%2) == 0) && torpedo.libera == 0){
        torpedo.movx = 20;
        torpedo.movy = 20;
        torpedo.rotate = 0;
        torpedo.libera = 1;

        glutTimerFunc(0, analisaTorpedo, 16);
    }
    if(passaro2.libera == 0)
    {
        passaro2.x=0;
        passaro2.libera=1;
        glutTimerFunc(0, analisaPassaro2, 16);
    }

    passaro2.x+=10.f;

    if(passaro2.x==0){
       passaro2.x+=10.f;
    }

    if(passaro2.x==600){
       passaro2.x-=10.f;
    }
    targetReached=false;// o tiro do boss inicializado por padrão

    glutTimerFunc(periodo, enemyRandom, periodo);
}

//desenha o ceu
void updateCenario(int value)
{
    if(PosicaoCeu<0.6666){
    PosicaoCeu += SKY_SPEED;  // Ajuste esse valor para controlar a velocidade de movimento
    glutTimerFunc(32, updateCenario, 0);  // Agende a próxima atualização
    glutPostRedisplay();// Redesenha a cena
    }
}
void chamaMenu(){
    cena.finalVitoria=0;
    cena.gameOver=0;
    cena.menu=1;
}
void fimDeJogo(){
    cena.fase_3=0;

    /*if(points<3000){
        cena.gameOver=1;
    }
    else{*/
        cena.finalVitoria=1;
        glutTimerFunc(5000, chamaMenu, 0);
        nuvemY = 1.1f;
    //}

}
void fase3(){//mudar de fase, timerFunc ta setada para durar 3 min glutTimerFunc(180000, fase2, 0);. Repetir para trocar para fase 3.
    cena.abertura_fase_3=0;
    nuvemY = 1.1f;
    cena.fase_3=1;

    vida.vidatotal=10;

    abelha.vida=1;
    passaroQuePersegue.vida=1;
    torpedo.vida=1;
    passaro1.vida=1;
    passaro2.vida=1;
    passaro3.vida=1;
    boss.vida=5;
    
    rgbPLataforma.blue=0;
    rgbPLataforma.green=0;
    rgbPLataforma.red=1;

    glutTimerFunc(0, updateCenario, 0);
    glutTimerFunc(10000, fimDeJogo, 0);

}

void aberturaFase3(){
    
    cena.fase_2=0;
    
    /*if(points<1600){
        cena.gameOver=1;
    }
    else{*/
        cena.abertura_fase_3=1;
        glutTimerFunc(1800, fase3, 0); 
     
    //}
}

void fase2(){//mudar de fase, timerFunc ta setada para durar 3 min glutTimerFunc(180000, fase2, 0);. Repetir para trocar para fase 3.
    cena.abertura_fase_2=0;
    nuvemY = 1.1f;
    cena.fase_2=1;
    cena.fase_1=0;

    vida.vidatotal=10;

    abelha.vida=1;
    passaroQuePersegue.vida=1;
    torpedo.vida=1;
    passaro1.vida=1;
    passaro2.vida=1;
    passaro3.vida=1;

    rgbPLataforma.blue=0;
    rgbPLataforma.green=0;
    rgbPLataforma.red=1;

    glutTimerFunc(0, updateCenario, 0);
    glutTimerFunc(10000, aberturaFase3, 0);
}

void aberturaFase2(){
    cena.fase_1=0;

    /*if(points<800){
        cena.gameOver=1;
    }
    else{*/
        cena.abertura_fase_2=1;
        glutTimerFunc(1800, fase2, 0);   
    //}

}

void fase1(){
    cena.abertura_fase_1=0;   
    nuvemY = 1.1f;
    cena.fase_1=1;
    vida.vidatotal=10;
    

    //glutTimerFunc(0, movPlat1, 16); // idem ao debaixo
    glutTimerFunc(1000, enemyRandom, 1000); // iniciar assim que clicar no start e entrar na fase 1
    glutTimerFunc(10000, aberturaFase2, 0); // idem ao de cima
    glutTimerFunc(0, pontos, 5000); // idem também
    glutTimerFunc(1000, enemyRandom, 1000); // iniciar assim que clicar no start e entrar na fase 1
    glutTimerFunc(0, updateCenario, 0);
    glutTimerFunc(0, analisaNevoa, 150);
    //glutTimerFunc(0,tratamentoColisao,16);
    // Libera a memória alocada pelos quadrados
}

void tempoInvulneravel(){
    vida.invulneravel=0;
}
void tratamentoColisao(){
    
    /////// COLISAO DO TIRO DO PROTAGONISTA
    if(tiroProtagonista.libera==1){
        if(abelha.vida==1){
            if(colisaoTiroProtagonistaAlvo(abelha.movx, abelha.movy,  tiroProtagonista.movx,   tiroProtagonista.movy, abelha.largura, abelha.altura)){
                tiroProtagonista.libera=0;
                points+=10;
                abelha.vida--;
            }
        }
        if(passaroQuePersegue.vida==1){
            if(colisaoTiroProtagonistaAlvo(passaroQuePersegue.movx, passaroQuePersegue.movy,  tiroProtagonista.movx,   tiroProtagonista.movy, passaroQuePersegue.largura, passaroQuePersegue.altura)){
                tiroProtagonista.libera=0;
                points+=10;
                passaroQuePersegue.vida--;
            }
        }
        if(torpedo.vida==1){
            if(colisaoTiroProtagonistaAlvo(torpedo.movx, torpedo.movy,  tiroProtagonista.movx,   tiroProtagonista.movy, torpedo.largura, torpedo.altura)){
            tiroProtagonista.libera=0;
            points+=10;
            torpedo.vida--;
            }
        }
        
        if(passaro1.vida==1){
            if(colisaoTiroProtagonistaAlvo(passaro1.x, passaro1.y,  tiroProtagonista.movx,   tiroProtagonista.movy, passaro1.largura, passaro1.altura)){
            tiroProtagonista.libera=0;
            points+=10;
            passaro1.vida--;
            }
        }
        
        if(passaro2.vida==1){
            if(colisaoTiroProtagonistaAlvo(passaro2.x, passaro2.y,  tiroProtagonista.movx,   tiroProtagonista.movy, passaro2.largura, passaro2.altura)){
            tiroProtagonista.libera=0;
            points+=10;
            passaro2.vida--;
            }
        }
        
        if(passaro3.vida==1){
            if(colisaoTiroProtagonistaAlvo(passaro3.x, passaro3.y,  tiroProtagonista.movx,   tiroProtagonista.movy, passaro3.largura, passaro3.altura)){
            tiroProtagonista.libera=0;
            points+=10;
            passaro3.vida--;
            }
        }
        if(boss.vida>0){
            if(colisaoTiroProtagonistaAlvo( boss.posX, boss.posY,  tiroProtagonista.movx,   tiroProtagonista.movy, boss.largura, boss.altura)){
            tiroProtagonista.libera=0;
            points+=50;
            boss.vida--;
            }
        }
         if(boss.vida>0){
            for(int i=0;i<numQuadrados;i++){
             if(colisaoTiroProtagonistaAlvo(protagonista.x,protagonista.y, rectPosX[i], rectPosY[i], boss.largura, boss.altura)){
              tiroProtagonista.libera=0;
              boss.vida--;
             }
            }
        }
    }

    ///////// COLISAO COM OBSTACULOS
    if(colisaoProtagonistaObstaculos(ataqueAbelha.movx, ataqueAbelha.movy,  protagonista.x,   protagonista.y, ataqueAbelha.largura, ataqueAbelha.altura)){
        if(vida.invulneravel==0&&abelha.vida!=0){
            vida.vidatotal--;
            vida.invulneravel=1;
            glutTimerFunc(1000, tempoInvulneravel, 0);

        }
    }
    if(colisaoProtagonistaObstaculos(abelha.movx+2, abelha.movy+2,  protagonista.x,   protagonista.y, abelha.largura-2, abelha.altura-2)){
        if(vida.invulneravel==0&&abelha.vida!=0){
            vida.vidatotal--;
            vida.invulneravel=1;
            glutTimerFunc(1000, tempoInvulneravel, 0);
 
        }
    }
    if(colisaoProtagonistaObstaculos(passaroQuePersegue.movx+2, passaroQuePersegue.movy+2,  protagonista.x,   protagonista.y, passaroQuePersegue.largura-2, passaroQuePersegue.altura-2)){
        if(vida.invulneravel==0&&passaroQuePersegue.vida!=0){
            vida.vidatotal--;
            vida.invulneravel=1;
            glutTimerFunc(1000, tempoInvulneravel, 0);
 
        }
    }
    if(colisaoProtagonistaObstaculos(torpedo.movx+2, torpedo.movy+2,  protagonista.x,   protagonista.y, torpedo.largura-2, torpedo.altura-2)){
        if(vida.invulneravel==0&&torpedo.vida!=0){
            vida.vidatotal--;
            vida.invulneravel=1;
            glutTimerFunc(1000, tempoInvulneravel, 0);
        }
    }
    if(colisaoProtagonistaObstaculos(passaro1.x+2, passaro1.y+2,  protagonista.x,   protagonista.y, passaro1.largura-2, passaro1.altura-2)){
        if(vida.invulneravel==0&&passaro1.vida!=0){
            vida.vidatotal--;
            vida.invulneravel=1;
            glutTimerFunc(1000, tempoInvulneravel, 0);
 
        }
    }
    if(colisaoProtagonistaObstaculos(passaro2.x+2, passaro2.y+2,  protagonista.x,   protagonista.y, passaro2.largura-2, passaro2.altura-2)){
        if(vida.invulneravel==0&&passaro2.vida!=0){
            vida.vidatotal--;
            vida.invulneravel=1;
            glutTimerFunc(1000, tempoInvulneravel, 0);

        }
    }
    if(colisaoProtagonistaObstaculos(passaro3.x+2, passaro3.y+2,  protagonista.x,   protagonista.y, passaro3.largura-2, passaro3.altura-2)){
        if(vida.invulneravel==0&&passaro3.vida!=0){
            vida.vidatotal--;
            vida.invulneravel=1;
            glutTimerFunc(1000, tempoInvulneravel, 0);

        }
    }

    if(colisaoProtagonistaObstaculos(boss.TranslateX, boss.TranslateY,  protagonista.x,   protagonista.y, boss.largura-2, boss.altura-2)){
        if(vida.invulneravel==0&&boss.vida!=0){
            vida.vidatotal--;
            vida.invulneravel=1;
            glutTimerFunc(1500, tempoInvulneravel, 0);

        }
    }

    
    //caso caia na LAVA volta para a posicao inicial
    if(protagonista.x>=0&&protagonista.x<=600){
        if(protagonista.y>=0&&protagonista.y<=40){
            protagonista.x = platParada[0][0].x;
            protagonista.y = yPlatParada.y6;
            protagonista.isFalling=false;
            protagonista.isFreeFall=false;
            protagonista.isJumping=false;
             if(vida.invulneravel==0){
                vida.vidatotal--;
                vida.invulneravel=1;
                glutTimerFunc(1500, tempoInvulneravel, 0);
                printf("\n\n\nCOlisaoAtauqe abelha");
                printf("%d",vida.vidatotal);
            }
        }
    }
   
    /*if(vida.vidatotal==0){
        cena.fase_1=0;
        cena.fase_2=0;
        cena.fase_3=0;
        cena.gameOver=1;
        glutTimerFunc(18000, chamaMenu, 0);
    }*/
    
}

//quando o personagem chega na última fileira de plataformas, ele volta para a posicao inicial e a plataforma muda de cor 
//para dar a ideia de que ele "subiu"
void subirCenario(){
    if(protagonista.y==yPlatParada.y1){
        protagonista.y=yPlatParada.y6;
        if(rgbPLataforma.red==1){
            rgbPLataforma.red=0;
            rgbPLataforma.blue=0;
            rgbPLataforma.green=1;
        }
        else if(rgbPLataforma.green==1){
            rgbPLataforma.red=0;
            rgbPLataforma.blue=1;
            rgbPLataforma.green=0;
        }
        else if(rgbPLataforma.blue==1){
            rgbPLataforma.red=1;
            rgbPLataforma.blue=0;
            rgbPLataforma.green=0;
        }
        passaro1.vida=1;
        passaro2.vida=1;
        passaro3.vida=1;
        boss.vida=3;
        abelha.vida=1;
        passaroQuePersegue.vida=1;
        torpedo.vida=1;
        vida.invulneravel=1;
        glutTimerFunc(1000, tempoInvulneravel, 0);

    }
}
void movimentoProtagonista(){

    if(teclado.d==1){
        if(protagonista.x+3+protagonista.largura<LARGURA_MUNDO){
            protagonista.x +=3;
        }
        else{
            protagonista.x=0;
        }
        
        if(!protagonista.isJumping&&!protagonista.isFalling){
            if(!(((protagonista.x+protagonista.largura)>=platParada[0][0].x&&protagonista.x<=platParada[0][1].x)||
            ((protagonista.x+protagonista.largura)>=platParada[1][0].x&&protagonista.x<=platParada[1][1].x)||
            ((protagonista.x+protagonista.largura)>=platParada[2][0].x&&protagonista.x<=platParada[2][1].x)||
            ((protagonista.x+protagonista.largura)>=platParada[3][0].x&&protagonista.x<=platParada[3][1].x))){
                protagonista.isFreeFall=true;
            }
            else if(!(protagonista.y==(yPlatParada.y6)||protagonista.y==(yPlatParada.y5)
                ||protagonista.y==(yPlatParada.y4)||protagonista.y==(yPlatParada.y3)
                ||protagonista.y==(yPlatParada.y2)||protagonista.y==(yPlatParada.y1))){
                
                protagonista.isFreeFall=true;
            }
            else{
                protagonista.isFreeFall=false;
            }
        }
        
        
        if (protagonista.spriteX<10.0/11.0) {
            protagonista.spriteX += 1.0/11.0;
        }else{
            protagonista.spriteX= 6.0/11.0;
        }

    }
    if(teclado.a==1){
        if(protagonista.x-3>=0){
            protagonista.x -=3;
        }
        else{
            protagonista.x=600;
        }
        
         if(!protagonista.isJumping&&!protagonista.isFalling){
            if(!(((protagonista.x+protagonista.largura)>=platParada[0][0].x&&protagonista.x<=platParada[0][1].x)||
            ((protagonista.x+protagonista.largura)>=platParada[1][0].x&&protagonista.x<=platParada[1][1].x)||
            ((protagonista.x+protagonista.largura)>=platParada[2][0].x&&protagonista.x<=platParada[2][1].x)||
            ((protagonista.x+protagonista.largura)>=platParada[3][0].x&&protagonista.x<=platParada[3][1].x))){
                protagonista.isFreeFall=true;
            }
            else if(!(protagonista.y==(yPlatParada.y6)||protagonista.y==(yPlatParada.y5)
                ||protagonista.y==(yPlatParada.y4)||protagonista.y==(yPlatParada.y3)
                ||protagonista.y==(yPlatParada.y2)||protagonista.y==(yPlatParada.y1))){
                
                protagonista.isFreeFall=true;
            }
            else{
                protagonista.isFreeFall=false;
            }
        }
        if (protagonista.spriteX>0) {
            protagonista.spriteX -= 1.0/11.0;
        }else{
            protagonista.spriteX= 4.0/11.0;
        }
        
    }
    if(teclado.w>0){
        protagonista.y+=12;
        teclado.w--;
        if(teclado.w==0){
            protagonista.isJumping=false;
            protagonista.isFalling=true;
        }
    }
    if(protagonista.isFalling){
        protagonista.y-=5;
        protagonista.spriteX= 5.0/11.0;
            if((((protagonista.x+protagonista.largura)>=platParada[0][0].x&&protagonista.x<=platParada[0][1].x)||
            ((protagonista.x+protagonista.largura)>=platParada[1][0].x&&protagonista.x<=platParada[1][1].x)||
            ((protagonista.x+protagonista.largura)>=platParada[2][0].x&&protagonista.x<=platParada[2][1].x)||
            ((protagonista.x+protagonista.largura)>=platParada[3][0].x&&protagonista.x<=platParada[3][1].x))){
                if((protagonista.y==(yPlatParada.y6)||protagonista.y==(yPlatParada.y5)
                ||protagonista.y==(yPlatParada.y4)||protagonista.y==(yPlatParada.y3)
                ||protagonista.y==(yPlatParada.y2)||protagonista.y==(yPlatParada.y1))){
                    teclado.w=-10;
                    protagonista.isFalling=false;
                }
            }
            else{
                
                teclado.w--;
            }
        
        
    }
    if(protagonista.isFreeFall){
        protagonista.y-=5;
        if((((protagonista.x+protagonista.largura)>=platParada[0][0].x&&protagonista.x<=platParada[0][1].x)||
            ((protagonista.x+protagonista.largura)>=platParada[1][0].x&&protagonista.x<=platParada[1][1].x)||
            ((protagonista.x+protagonista.largura)>=platParada[2][0].x&&protagonista.x<=platParada[2][1].x)||
            ((protagonista.x+protagonista.largura)>=platParada[3][0].x&&protagonista.x<=platParada[3][1].x))){
                if((protagonista.y==(yPlatParada.y6)||protagonista.y==(yPlatParada.y5)
                ||protagonista.y==(yPlatParada.y4)||protagonista.y==(yPlatParada.y3)
                ||protagonista.y==(yPlatParada.y2)||protagonista.y==(yPlatParada.y1))){
                
                    protagonista.isFreeFall=false;
                }
            }
    }
}
void update() {
    if(cena.fase_1==1||cena.fase_2==1||cena.fase_3==1){
        movimentoProtagonista();
        subirCenario();
        tratamentoColisao();
    }
    if (!targetReached) {
        for (int i = 0; i < numQuadrados; i++) {
            float dx = targetX - rectPosX[i];
            float dy = targetY - rectPosY[i];
            float distance = sqrt(dx * dx + dy * dy);
            
            dx /= distance;
            dy /= distance;
            
            rectPosX[i] += dx * 5.0f;
            rectPosY[i] += dy * 5.0f;
            
            if (distance < 5.0f) {
                targetReached = true;
            }
        }
        
        if (targetReached && numQuadrados == MAX_QUADRADOS) {
            // Todos os quadrados atingiram o destino, resetar o vetor
            numQuadrados = 0;
        }
    }
    
    
    glutPostRedisplay();
}


void keyboard(unsigned char key, int x, int y)
{ 
    
    targetReached = false;  // Defi
    // Pressionar a barra de espaço faz o personagem pular
     switch(key){

        case 's':
        case 'S':
        teclado.s=1;
        
        break;
        

        case 'w':
        case 'W':
        if(!protagonista.isJumping&&!protagonista.isFalling&&!protagonista.isFreeFall){
            teclado.w=10;
            protagonista.isJumping = true;
        }
        
        break;

        case 'd':
        case 'D':
        teclado.d=1;
        
        break;

        case 'a':
        case 'A':
        teclado.a=1;
        
        break;

        case 27:
        exit(0);
        break;


    }
     glutPostRedisplay();
  
}

void keyboardUp(unsigned char key, int x, int y)
{
   switch(key){

        case 'd':
        case 'D':
        teclado.d=-1;
    
        break;

        case 'a':
        case 'A':
        teclado.a=-1;

        break;


    }
    glutPostRedisplay();
}

void MouseAndandoNaoPressionado (int x, int y)
{     // início da conversão de coordenadas da tela para do mundo
    // retirado de https://community.khronos.org/t/converting-
    //window-coordinates-to-world-coordinates/16029/8

    GLint viewport[4]; //var to hold the viewport info
    GLdouble modelview[16]; //var to hold the modelview info
    GLdouble projection[16]; //var to hold the projection matrix info
    GLfloat winX, winY, winZ; //variables to hold screen x,y,z coordinates
    GLdouble worldX, worldY, worldZ; //variables to hold world x,y,z coordinates

    glGetDoublev(GL_MODELVIEW_MATRIX, modelview); //get the modelview info
    glGetDoublev(GL_PROJECTION_MATRIX, projection); //get the projection matrix info
    glGetIntegerv(GL_VIEWPORT, viewport); //get the viewport info

    winX = (float)x;
    winY = (float)viewport[3] - (float)y;
    winZ = 0;
    
    //get the world coordinates from the screen coordinates
    gluUnProject(winX, winY, winZ, modelview, projection, viewport, &worldX, &worldY, &worldZ);

    // fim da conversão de coordenadas da tela para do mundo

    
    if(worldX>100&&worldX<500){
        if(worldY>100&&worldY<250){
            acoesMenu.emCimaInstrucao=1;
        }
        else if(worldY>350&&worldY<500){
            acoesMenu.emCimaNovoJogo=1;
        }
        else{
            acoesMenu.emCimaInstrucao=0;
            acoesMenu.emCimaNovoJogo=0;
        }
    }
    else{
        acoesMenu.emCimaInstrucao=0;
        acoesMenu.emCimaNovoJogo=0; 
    }
    glutPostRedisplay();
    
}
void mouse(int button, int state, int x, int y){
    /*Conversão das coordenadas da tela para as coordenadas do mundo
    A solução abaixo foi encontrada em: https://community.khronos.org/t/converting-window-coordinates-to-world-coordinates/16029/8
    Foi necessário para que o quadrado e o círculo continuassem centrados onde o mouse foi clicado
    */
    GLint viewport[4];
    GLdouble modelview[16]; //var to hold the modelview info
    GLdouble projection[16]; //var to hold the projection matrix info
    GLfloat winX, winY, winZ; //variables to hold screen x,y,z coordinates
    GLdouble worldX, worldY, worldZ; //variables to hold world x,y,z coordinates

    glGetDoublev( GL_MODELVIEW_MATRIX, modelview ); //get the modelview info
    glGetDoublev( GL_PROJECTION_MATRIX, projection ); //get the projection matrix info
    glGetIntegerv( GL_VIEWPORT, viewport ); //get the viewport info

	winX = (float)x;
    winY = (float)viewport[3] - (float)y;
	winZ = 0;

    gluUnProject( winX, winY, winZ, modelview, projection, viewport, &worldX, &worldY, &worldZ);
    // fim da conversão
    
    switch (button)
    {
        case GLUT_LEFT_BUTTON: printf("Botao esquerdo.");
                               break;
        case GLUT_RIGHT_BUTTON: printf("Botao direito.");
                               break;
        case GLUT_MIDDLE_BUTTON: printf("Botao do meio.");
                               break;
    }
    if (state == GLUT_UP) {
         if (cena.fase_3 == 1) {
          desenhaPortempoLine3();
          glutPostRedisplay();
       }
  }
    if (state == GLUT_DOWN){
       
        if(cena.menu==1){
            if(worldX>100&&worldX<500){
                if(worldY>100&&worldY<250){
                    cena.instrucoes=1;
                }
                else if(worldY>350&&worldY<500){
                    cena.menu=0;
                    cena.abertura_fase_1=1;

                    glutTimerFunc(1800, fase1, 0);
                    
                }
            }
            else{
                acoesMenu.emCimaInstrucao=0;
                acoesMenu.emCimaNovoJogo=0; 
            }
        }
        if((cena.fase_1==1||cena.fase_2==1||cena.fase_3==1)&&tiroProtagonista.libera==0){

            switch (button)
            {
                case GLUT_LEFT_BUTTON:
                    tiroProtagonista.libera=1;
                    tiroProtagonista.movx=protagonista.x-50;
                    tiroProtagonista.movy=protagonista.y-50;
                    if(cena.fase_3==1){
                      desenhaPortempo();
            
                    }
                          
                break;
            }
        }
        
    }
    
}
void redimensionada(int width, int height) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, LARGURA_MUNDO, 0, ALTURA_MUNDO, -1, 1);

    float razaoAspectoJanela = ((float)width)/height;
    float razaoAspectoMundo = ((float) LARGURA_MUNDO)/ ALTURA_MUNDO;
    // se a janela está menos larga do que o mundo (16:9)...
    if (razaoAspectoJanela < razaoAspectoMundo) {
        // vamos colocar barras verticais (acima e abaixo)
        float hViewport = width / razaoAspectoMundo;
        float yViewport = (height - hViewport)/2;
        glViewport(0, yViewport, width, hViewport);
    }
    // se a janela está mais larga (achatada) do que o mundo (16:9)...
    else if (razaoAspectoJanela > razaoAspectoMundo) {
        // vamos colocar barras horizontais (esquerda e direita)
        float wViewport = ((float)height) * razaoAspectoMundo;
        float xViewport = (width - wViewport)/2;
        glViewport(xViewport, 0, wViewport, height);
    } else {
        glViewport(0, 0, width, height);
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
void inicializa(){
    glClearColor(1.0,1.0,1.0,1.0);
    glClearColor(0, 0, 0, 1);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    boss.altura = 150;
    boss.largura = 150;
    boss.textura = carregaTextura("texturas/boss/abelha.png");
    boss.estrelinha = carregaTextura("texturas/boss/textura.png");
    boss.Enemy = carregaTextura("texturas/boss/texturascup.png");
    boss.rotacao = 0.0f;
    boss.TranslateX=200;
    boss.TranslateY=540;
    boss.vida=0;
    tiroBoss.TexturaTiro=carregaTextura("texturas/boss/textura.png");

    torpedo.torpedo = carregaTextura("texturas/torpedo.png");
    torpedo.x = 0.0;
    torpedo.y = 0.0;
    torpedo.largura = 100.0;
    torpedo.altura = 50.0;
    torpedo.spriteX = 0.0;
    torpedo.spriteY = 0.0;
    torpedo.libera = 0;
    torpedo.vida=1;

    protagonista.protagonista = carregaTextura("texturas/protagonista.png");
    protagonista.x = platParada[0][0].x;
    protagonista.y = yPlatParada.y6;
    protagonista.largura = 50;
    protagonista.altura = 50;
    protagonista.spriteX = 1.0/11.0;
    protagonista.spriteY = 1.0/3.0;
    protagonista.velocityY = 0.0f;
    protagonista.isJumping = false;
    protagonista.isFalling = false;
    protagonista.isFreeFall=false;
    protagonista.velocityX = 0.0f;
    
    tiroProtagonista.cima=0;
    tiroProtagonista.baixo=0;
    tiroProtagonista.direita=0;
    tiroProtagonista.esquerda=0;
    tiroProtagonista.libera=0;

    vida.x = 10;
    vida.y = 520;
    vida.largura = 350;
    vida.altura = 50;
    vida.spriteX = 0.0;
    vida.spriteY = 0.0;
    vida.invulneravel=0;

    passaro1.passaro = carregaTextura("texturas/passaro.png");
    passaro2.passaro = carregaTextura("texturas/passaro.png");
    passaro3.passaro = carregaTextura("texturas/passaro.png");

    passaro1.x = 60;
    passaro1.y = yPlatParada.y5;
    passaro1.largura = 50;
    passaro1.altura = 50;
    passaro1.spriteX = 0.0;
    passaro1.spriteY = 0.91;
    passaro1.vida=1;

    passaro2.x = 0;
    passaro2.y = yPlatParada.y4;
    passaro2.largura = 50;
    passaro2.altura = 50;
    passaro2.spriteX = 0.13;
    passaro2.spriteY = 0.13;
    passaro2.libera=0;
    passaro2.vida=1;

    passaro3.x = 410;
    passaro3.y = yPlatParada.y3;
    passaro3.largura = 50;
    passaro3.altura = 50;
    passaro3.spriteX = 0.0;
    passaro3.spriteY = 0.4;
    passaro3.vida=1;

    abelha.inimigo = carregaTextura("texturas/inimigo.png");
    abelha.x = 0.0;
    abelha.y = 0.0;
    abelha.altura = 100.0;
    abelha.largura = 50.0;
    abelha.spriteX = 0.0;
    abelha.spriteY = 0.0;
    abelha.libera = 0;
    abelha.xper = (float)LARGURA_MUNDO;
    abelha.ataque = 0;
    abelha.vida=1;

    ataqueAbelha.projetil = carregaTextura("texturas/projetil.png");
    ataqueAbelha.x = 0.0;
    ataqueAbelha.y = 0.0;
    ataqueAbelha.altura = 20.0;
    ataqueAbelha.largura = 20.0;
    ataqueAbelha.spriteX = 0.0;
    ataqueAbelha.spriteY = 0.0;

    passaroQuePersegue.projetil = carregaTextura("texturas/projetil.png");
    passaroQuePersegue.x = 0.0;
    passaroQuePersegue.y = 0.0;
    passaroQuePersegue.altura = 50.0;
    passaroQuePersegue.largura = 50.0;
    passaroQuePersegue.spriteX = 0.0;
    passaroQuePersegue.spriteY = 0.0;
    passaroQuePersegue.libera = 0;
    passaroQuePersegue.libera =0;
    passaroQuePersegue.vida=1;

    points=0;

    texturaCeu = SOIL_load_OGL_texture("texturas/ceu.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
    texturaNuvem1= carregaTextura("texturas/cenafase1.png");
    texturaNuvem2= carregaTextura("texturas/cenafase2.png"); 
    texturaNuvem3= carregaTextura("texturas/cenafase3.png"); 
    texturaNuvem4= carregaTextura("texturas/cenagameover.png"); 
    texturaFinal= carregaTextura("texturas/final.png"); 

    cena.menu=1;
    cena.gameOver=0;
    cena.instrucoes=0;

    acoesMenu.emCimaNovoJogo=0;
    acoesMenu.emCimaInstrucao=0;
    acoesMenu.cliqueNovoJogo=0;
    acoesMenu.cliqueInstrucao=0;
    teclado.w=0;
    teclado.a=0;
    teclado.s=0;
    teclado.d=0;
    controlNevoa.texNevoa=0;
    controlNevoa.auxNevoa=0;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    idTexturaNovoJogoMenu = carregaTextura("texturas/novoJogo.png");
    idTexturaNovoJogoCompleta = carregaTextura("texturas/novoJogo2.png");
    idTexturaInstrucoes = carregaTextura("texturas/Instrucoes.png");
    idTexturaMostrarInstrucoes = carregaTextura("texturas/instrucao.png");
    idTexturaNevoa[0] = carregaTextura("texturas/nevoa/nevoa.png");
    idTexturaNevoa[1] = carregaTextura("texturas/nevoa/nevoa2.png");
    idTexturaNevoa[2] = carregaTextura("texturas/nevoa/nevoa3.png");
    idTexturaNevoa[3] = carregaTextura("texturas/nevoa/nevoa4.png");
    idTexturaNevoa[4] = carregaTextura("texturas/nevoa/nevoa5.png");
    carregaListaNovoJogoNormal();
    carregaListaNovoJogoAlterado();
    carregaListaInstrucoesNormal();
    carregaListaInstrucoesAlterada();
}

int main(int argc, char** argv){
    glutInit(&argc, argv); // acordando o GLUT

    //definindo a versão do OpenGL que vamos usar
    glutInitContextVersion(1,1);
    glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

    //configuração inicial da janela GLUT
    glutInitDisplayMode(GLUT_DOUBLE); //QUANTIDADE DE BUFFERS -> GLUT_DOUBLE É DOIS
    glutInitWindowPosition(500,100); // posição inicial
    glutInitWindowSize(600,600);     // tamanho inicial

    glutCreateWindow("Menu Jogo");


    glutDisplayFunc(desenhaMinhaCena);
    glutReshapeFunc(redimensionada);

    glutPassiveMotionFunc(MouseAndandoNaoPressionado);
    glutMouseFunc(mouse);
    glutTimerFunc(frameInterval, trocarSprite, 0);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    glutIdleFunc(update);
    // glutTimerFunc(0, updatePuloEnemy, 0);
    
    setup();
    inicializa();


    glutMainLoop();
    return 0;
}