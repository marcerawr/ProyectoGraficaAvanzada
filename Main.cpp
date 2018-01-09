#include "Main.h"
#include "3ds.h"
#include "iostream"

HDC			hDC=NULL;		// Dispositivo de contexto GDI
HGLRC		hRC=NULL;		// Contexto de renderizado
HWND		hWnd=NULL;		// Manejador de ventana
HINSTANCE	hInstance;		// Instancia de la aplicacion

bool	keys[256];			// Arreglo para el manejo de teclado
bool	active=TRUE;		// Bandera de ventana activa
BOOL	done=FALSE;			// Variable booleana para salir del ciclo

//int glWidth;
//int glHeight;
infoGame infGame;

CVector posCamV;
CVector dirCamV;
CVector vecCamVPl;
//int detectado=0;

//Apuntador para primitivas de cuadricas
GLUquadricObj	*e;

void manejaEventosMain();
void cargaConfiguracion();
void actualizaCajaColPersonaje();

particles particle[MAX_PARTICULAS];	// Arreglo de partículas

//Nombre y ubicación del modelo
#define FILE_NAME1c  "Modelos/stewie_torso.3ds"
#define FILE_NAME2c  "Modelos/stewie_cabeza.3ds"
#define FILE_NAME3c  "Modelos/stewie_piernader.3ds"
#define FILE_NAME4c  "Modelos/stewie_piernaizq.3ds"
#define FILE_NAME5c  "Modelos/stewie_brazoder_a.3ds"
#define FILE_NAME6c  "Modelos/stewie_brazoder_b.3ds"
#define FILE_NAME7c  "Modelos/stewie_brazoizq_a.3ds"
#define FILE_NAME8c  "Modelos/stewie_brazoizq_b.3ds"
#define FILE_NAME9c  "Modelos/stewie_bota.3ds"
#define FILE_NAME1e  "Modelos/bomb.3DS"

//Contenedor de texturas del modelo
CTga textureModel1c[20];
CTga textureModel2c[20];
CTga textureModel3c[20];
CTga textureModel4c[20];
CTga textureModel5c[20];
CTga textureModel6c[20];
CTga textureModel7c[20];
CTga textureModel8c[20];
CTga textureModel9c[20];

CTga textureModel1e[5];

CLoad3DS g_Load3ds;

//Acceso a la estructura que almacena los datos del modelo
t3DModel g_3DModel1c;
t3DModel g_3DModel2c;
t3DModel g_3DModel3c;
t3DModel g_3DModel4c;
t3DModel g_3DModel5c;
t3DModel g_3DModel6c;
t3DModel g_3DModel7c;
t3DModel g_3DModel8c;
t3DModel g_3DModel9c;

t3DModel g_3DModel1e;

//Objeto para acceder a las variables de control del personaje
paramObj player1;

//Objeto para manejo de la cámara
paramCam camara1;

//Objeto para acceder a la selección de materiales
CMateriales Material;

//- SPLINES -//
CVector puntosdeControl[20]; //Aquí se almacenará la posición de cada punto de control
int resolucion=60; //Entre cada par de puntos de control se calcularan puntos intermedios para dibujar 5 segmentos
int pSpAct; //Punto calculado actual (indice)
int totalP; //Total de puntos a calcular
int pctAct;  //Punto de control actual
CVector pAct;  //Punto actual calculado en la curva
CVector pSig;  //Punto siguiente a calcular en la curva
CVector DirSp; //Dirección del vector sobre la curva

CVector camSpline;
CVector objSpline;

cajaCol cajaPersonaje;
cajaCol cajaEscenario[200];
cajaCol cajaBomba;
cajaCol cajaParticula;
int numCajas=200;
float altPiso=0.0f;
float altMin=-30.0f;
esferaCol esferaPersonaje;
esferaCol esferaEnemigo;

//Variables del personaje
float Angt1;
float Angt2;
float Angc1;
float Angc2;
float Angbi1;
float Angbi2;
float Angbib;
float Angbd1;
float Angbd2;
float Angbdb;
float Angpizq;
float Angpder;
float Angpi;
float Angpd;

float Xtor;
float Ytor;
float Ztor;

//Contenedor de texturas para el escenario
CTga textura[36];

//Variables para animaciones
const int maxKF1=3; //Num. total de KeyFrames para la secuencia 1 (caminar)
const int maxKF2=4; //Num. total de KeyFrames para la secuencia 2 (patada)
const int maxKF3=3; //Num. total de KeyFrames para la secuencia 3 (saltar)

FRAME KeyFrame1[maxKF1]; //Contenedor para almacenar cada keyframe de la secuencia 1
FRAME KeyFrame2[maxKF2]; //Contenedor para almacenar cada keyframe de la secuencia 2
FRAME KeyFrame3[maxKF3]; //Contenedor para almacenar cada keyframe de la secuencia 3

//Variables auxiliares para animacion
bool play=false; //Bandera para iniciar la animación
int  playIndex=0; //Auxiliar para leer la información del contenedor de keyframes
int  tipoAnim=1; //Indicador del tipo de animación: 1-caminando

//Variables para iluminacion
//GLfloat LightPos[] = {-250.0f, 80.0f, 5.0f, 1.0f};		// Posición de la luz
GLfloat LightPos[] = {-50.0f, 80.0f, 50.0f, 1.0f};		// Posición de la luz
GLfloat LightAmb[] = { 0.8f,  0.8f, 0.8f, 1.0f};		// Valores de la componente ambiente
GLfloat LightDif[] = { 0.9f,  0.9f, 0.9f, 1.0f};		// Valores de la componente difusa
GLfloat LightSpc[] = { 0.5f,  0.5f, 0.5f, 1.0f};		// Valores de la componente especular
CVector lightPosition;

GLfloat LightAmb2[] = { 0.3f,  0.3f, 0.3f, 1.0f};		// Valores de la componente ambiente
GLfloat LightDif2[] = { 0.4f,  0.4f, 0.4f, 1.0f};		// Valores de la componente difusa
GLfloat LightSpc2[] = { 0.2f,  0.2f, 0.2f, 1.0f};		// Valores de la componente especular

//Cambios para Fuentes
//Buscar el comentario anterior para ver todos los cambios necesarios
//Acceso a la clase CFont
CFont Font;
CControl controlFunc;
CAudio audioFunc;
CSonido sonidoF[3];
CTimer timerEstados;

//Para la bomba
CTimer timerBomba;		//Tiempo que dura la bomba antes de estallar
CVector PosicionBomba;	//Posicion bomba
bool existeBomba = false;

bool colisionInicialBomba = false;

modelo datosModelo;

//Para las colisiones entre particulas y cofres
int posPartX;
int posPartY;
int posPartZ;

//Cambios para FPS
//Buscar el comentario anterior para ver todos los cambios necesarios
//Variables para el control de fps
static int FPS = 0;
//Libreria que usamos para el timer
#pragma comment( lib, "winmm.lib" )

LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);	// Declaracion de WndProc (Procedimiento de ventana)

//Cambios para FPS
void CalculateFrameRate()
{
	static float framesPerSecond    = 0.0f;
    static float lastTime			= 0.0f;
	static char strFrameRate[50] = {0};

	float currentTime = GetTickCount() * 0.001f;				

	++framesPerSecond;

	if( currentTime - lastTime > 1.0f )
    {
	    lastTime = currentTime;
		
		FPS=int(framesPerSecond);
		
	    framesPerSecond = 0;
    }
}


GLvoid ReDimensionaEscenaGL(GLsizei width, GLsizei height)	// Redimensiona e inicializa la ventana
{
	if (height==0)							// Para que no se presente una division por cero
	{
		height=1;							// la altura se iguala a 1
	}

	glViewport(0,0,width,height);					// Resetea el puerto de vista

	glMatrixMode(GL_PROJECTION);					// Selecciona la Matriz de Proyeccion
	glLoadIdentity();								// Resetea la Matriz de Proyeccion

	// Calcula el radio de aspecto o proporcion de medidas de la ventana
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,1.0f,2000.0f);
	
	glMatrixMode(GL_MODELVIEW);							// Selecciona la Matriz de Vista de Modelo
	glLoadIdentity();									// Resetea la Matriz de Vista de Modelo

	infGame.glWidth=width;
	infGame.glHeight=height;
}

int CargaModelos()
{
	if(!g_Load3ds.Load3DSFile(FILE_NAME1c, &g_3DModel1c, textureModel1c))
		return 0;
	if(!g_Load3ds.Load3DSFile(FILE_NAME2c, &g_3DModel2c, textureModel2c))
		return 0;
	if(!g_Load3ds.Load3DSFile(FILE_NAME3c, &g_3DModel3c, textureModel3c))
		return 0;
	if(!g_Load3ds.Load3DSFile(FILE_NAME4c, &g_3DModel4c, textureModel4c))
		return 0;
	if(!g_Load3ds.Load3DSFile(FILE_NAME5c, &g_3DModel5c, textureModel5c))
		return 0;
	if(!g_Load3ds.Load3DSFile(FILE_NAME6c, &g_3DModel6c, textureModel6c))
		return 0;
	if(!g_Load3ds.Load3DSFile(FILE_NAME7c, &g_3DModel7c, textureModel7c))
		return 0;
	if(!g_Load3ds.Load3DSFile(FILE_NAME8c, &g_3DModel8c, textureModel8c))
		return 0;
	if(!g_Load3ds.Load3DSFile(FILE_NAME9c, &g_3DModel9c, textureModel9c))
		return 0;
	if (!g_Load3ds.Load3DSFile(FILE_NAME1e, &g_3DModel1e, textureModel1e))
		return 0;
	
	return TRUE;
}

void DescargaModelos()
{
	g_Load3ds.UnLoad3DSFile(&g_3DModel1c, textureModel1c);
	g_Load3ds.UnLoad3DSFile(&g_3DModel2c, textureModel2c);
	g_Load3ds.UnLoad3DSFile(&g_3DModel3c, textureModel3c);
	g_Load3ds.UnLoad3DSFile(&g_3DModel4c, textureModel4c);
	g_Load3ds.UnLoad3DSFile(&g_3DModel5c, textureModel5c);
	g_Load3ds.UnLoad3DSFile(&g_3DModel6c, textureModel6c);
	g_Load3ds.UnLoad3DSFile(&g_3DModel7c, textureModel7c);
	g_Load3ds.UnLoad3DSFile(&g_3DModel8c, textureModel8c);
	g_Load3ds.UnLoad3DSFile(&g_3DModel9c, textureModel9c);
	g_Load3ds.UnLoad3DSFile(&g_3DModel1e, textureModel1e);
	
}

void InicializaParticulas()
{
	//Se inicializan los valores de la estructura de partículas
	for (int loop = 0; loop<MAX_PARTICULAS; loop++)
	{
		particle[loop].life = 2.0f;
		particle[loop].fade = 0.02;

		//Posicion inicial
		particle[loop].x = 0;
		particle[loop].y = 0;
		particle[loop].z = 0;

		//"Gravedad" inicial
		particle[loop].xg = 0;
		particle[loop].yg = 0;
		particle[loop].zg = 0;

		particle[loop].xi = float((rand() % 50 + 1) - 25.0f) / 100.0f;  // Direccion y velocidad en X
		particle[loop].yi = 0;  // Direccion y velocidad en Y
		particle[loop].zi = 0;  // Direccion y velocidad en Z
	}
}

void InicializaParametrosdeControl()
{
	//Esta función establece los parámetros como velocidad del objeto y distancia de la cámara así como la posición y dirección iniciales

	player1.visible=true;
	player1.cayendo=true;
	player1.caminando=false;
	player1.dirX=0;
	player1.dirZ=0;

	player1.VelocidadObj=0.1f;
	player1.DistanciaCam=50.0f;
	
	player1.CamaraPosAlt=25.0f;	//Posición en y de la cámara (altura a la que se situa la cámara)
	player1.CamaraObjAlt=8.0f;	//Posición en y del objetivo de la cámara (altura a la que ve la cámara)
	player1.AngDir=0.0f;		//Este ángulo inicial hace que la dirección inicial sea paralela al eje Z y con sentido negativo
	player1.AngDirCam=0.0f;		//Este es el ángulo inicial que define la posición de la camara respecto al personaje
	player1.AngObj=90.0f;		//Este valor se elige dependiendo de la orientación con la que aparece el modelo en la escena al dibujarlo
								//sin aplicarle ninguna transformación (hacia adonde está volteando). Se elige un ángulo tal que al aplicarle
								//una rotación inicial con respecto al eje Y esté viendo hacia la misma dirección que la definida por AngDir
		
	player1.PosicionObj=CVector(-36.0f, 0.0f, 6.0f); //Esta es la posición inicial del objeto en la escena
	player1.Direccion.x=(float)cos(player1.AngDir*PI/180.0f);
	player1.Direccion.y=0.0f;
	player1.Direccion.z=(float)sin(player1.AngDir*PI/180.0f);

	player1.DireccionCam.x=(float)cos(player1.AngDirCam*PI/180.0f);
	player1.DireccionCam.y=0.0f;
	player1.DireccionCam.z=(float)sin(player1.AngDirCam*PI/180.0f);
	player1.PosicionCam=player1.PosicionObj-player1.DireccionCam*player1.DistanciaCam;
	player1.PosicionCam.y=player1.CamaraPosAlt;

	player1.ObjetivoCam=player1.PosicionObj;		//La cámara ve siempre al objeto
	player1.ObjetivoCam.y=player1.CamaraObjAlt;		//Para que no vea a los "pies" del objeto (personaje)

	player1.escalaX=0.8f;
	player1.escalaY=0.8f;
	player1.escalaZ=0.8f;

	player1.kick=false;
	player1.contAuxAnim=0;
		
}

void InicializaCamara()
{
	camara1.VariacionCam=2.0f;		//simetrico de -1.0 a 1.0)
	camara1.VariacionDist=300.0f;	//simetrico de -150.0 a 150.0

	//como no es simetrico porque va de -200.0 a 100.0 se usa un desplazamiento para la distancia
	camara1.VariacionDespDist=-50.0f;
	camara1.VariacionDespCam=0.0f;	//la variaciond de la camara se mantiene simétrica

	camara1.FactorCam=camara1.VariacionCam/camara1.VariacionDist;

	float aux=(player1.PosicionObj.x-camara1.VariacionDespDist)*camara1.FactorCam+camara1.VariacionDespCam;

	camara1.PosicionCam.x=player1.PosicionObj.x-30.0f*aux;
	camara1.PosicionCam.y=12.0f;
	camara1.PosicionCam.z=40.0f;

	camara1.ObjetivoCam=player1.PosicionObj;
	camara1.ObjetivoCam.y=player1.PosicionObj.y+5.0f;
}

void IniVarsPlayer()
{
	Angt1=25.0f;   //Torso: rotación en X
	Angt2=0.0f;   //Torso: rotación en Z
	Angc1=-25.0f;   //Cabeza: rotación en X
	Angc2=0.0f;   //Cabeza: rotación en Y
	Angbi1=-100.0f;  //Brazo Izq: rotación en X
	Angbi2=0.0f;  //Brazo Izq: rotación en Z
	Angbib=-30.0f;  //Antebrazo Izq: rotación en X
	Angbd1=120.0f;  //Brazo Der: rotación en X
	Angbd2=0.0f;  //Brazo Der: rotación en Z
	Angbdb=-80.0f;  //Antebrazo Der: rotación en X
	Angpizq=40.0f; //Pierna Izq: rotación en X
	Angpder=-60.0f; //Pierna Der: rotación en X
	Angpi=0.0f;   //Pie Izq: rotación en X
	Angpd=0.0f;   //Pie Der: rotación en X

	Xtor=0.0f;    //Torso: posición en X
	Ytor=0.0f;    //Torso: posición en Y
	Ztor=0.0f;    //Torso: posición en Z
}

void CargaTexturas()
{
	textura[0].LoadTGA("Texturas/t1.tga");
	textura[1].LoadTGA("Texturas/t2.tga");
	textura[2].LoadTGA("Texturas/t3.tga");
	textura[3].LoadTGA("Texturas/t4.tga");
	textura[4].LoadTGA("Texturas/t5.tga");
	textura[5].LoadTGA("Texturas/t6.tga");
	textura[6].LoadTGA("Texturas/t7.tga");
	textura[7].LoadTGA("Texturas/t8.tga");
	textura[8].LoadTGA("Texturas/t9.tga");
	textura[9].LoadTGA("Texturas/t10.tga");
	textura[10].LoadTGA("Texturas/t11.tga");
	textura[11].LoadTGA("Texturas/t12.tga");
	textura[12].LoadTGA("Texturas/t13.tga");
	textura[13].LoadTGA("Texturas/t14.tga");
	textura[14].LoadTGA("Texturas/t15.tga");
	textura[15].LoadTGA("Texturas/t16.tga");	
	textura[16].LoadTGA("Texturas/t17.tga");
	textura[17].LoadTGA("Texturas/t18.tga");
	textura[18].LoadTGA("Texturas/t19.tga");
	textura[19].LoadTGA("Texturas/t20.tga");
	textura[20].LoadTGA("Texturas/t21.tga");
	textura[21].LoadTGA("Texturas/t22.tga");
	textura[22].LoadTGA("Texturas/t23.tga");
	textura[23].LoadTGA("Texturas/t24.tga");
	textura[24].LoadTGA("Texturas/t25.tga");
	textura[25].LoadTGA("Texturas/t26.tga");
	textura[26].LoadTGA("Texturas/t27.tga");
	textura[27].LoadTGA("Texturas/t28.tga");
	textura[28].LoadTGA("Texturas/t29.tga");
	textura[29].LoadTGA("Texturas/t30.tga");
	textura[30].LoadTGA("Texturas/img0.tga");
	textura[31].LoadTGA("Texturas/particula.tga");

	textureModel1c[0].LoadTGA("Texturas/cara ste.tga");
}

void DescargaTexturas()
{
	textura[0].Elimina();
	textura[1].Elimina();
	textura[2].Elimina();
	textura[3].Elimina();
	textura[4].Elimina();
	textura[5].Elimina();
	textura[6].Elimina();
	textura[6].Elimina();
	textura[7].Elimina();
	textura[8].Elimina();
	textura[10].Elimina();
	textura[11].Elimina();
	textura[12].Elimina();
	textura[13].Elimina();
	textura[14].Elimina();
	textura[15].Elimina();
	textura[16].Elimina();
	textura[17].Elimina();
	textura[18].Elimina();
	textura[19].Elimina();
	textura[20].Elimina();
	textura[21].Elimina();
	textura[22].Elimina();
	textura[23].Elimina();
	textura[24].Elimina();
	textura[25].Elimina();
	textura[26].Elimina();
	textura[27].Elimina();
	textura[28].Elimina();
	textura[29].Elimina();
	textura[30].Elimina();
	textura[31].Elimina();

	textureModel1c[0].Elimina();
}

void InicializaAnim()
{
	//Se inicializan las variables de la secuencia 1
	for(int i=0; i<maxKF1; i++)
	{
		KeyFrame1[i].Angt1=0.0f;
		KeyFrame1[i].Angt2=0.0f;
		KeyFrame1[i].Angc1=0.0f;
		KeyFrame1[i].Angc2=0.0f;
		KeyFrame1[i].Angbi1=0.0f;
		KeyFrame1[i].Angbi2=0.0f;
		KeyFrame1[i].Angbib=0.0f;
		KeyFrame1[i].Angbd1=0.0f;
		KeyFrame1[i].Angbd2=0.0f;
		KeyFrame1[i].Angbdb=0.0f;
		KeyFrame1[i].Angpizq=0.0f;
		KeyFrame1[i].Angpder=0.0f;
		KeyFrame1[i].Angpi=0.0f;
		KeyFrame1[i].Angpd=0.0f;
		KeyFrame1[i].Xtor=0.0f;
		KeyFrame1[i].Ytor=0.0f;
		KeyFrame1[i].Ztor=0.0f;

		KeyFrame1[i].incAngt1=0.0f;
		KeyFrame1[i].incAngt1=0.0f;
		KeyFrame1[i].incAngc1=0.0f;
		KeyFrame1[i].incAngc2=0.0f;
		KeyFrame1[i].incAngbi1=0.0f;
		KeyFrame1[i].incAngbi2=0.0f;
		KeyFrame1[i].incAngbib=0.0f;
		KeyFrame1[i].incAngbd1=0.0f;
		KeyFrame1[i].incAngbd2=0.0f;
		KeyFrame1[i].incAngbdb=0.0f;
		KeyFrame1[i].incAngpizq=0.0f;
		KeyFrame1[i].incAngpder=0.0f;
		KeyFrame1[i].incAngpi=0.0f;
		KeyFrame1[i].incAngpd=0.0f;
		KeyFrame1[i].incXtor=0.0f;
		KeyFrame1[i].incYtor=0.0f;
		KeyFrame1[i].incZtor=0.0f;
	}

	for(int i=0; i<maxKF2; i++)
	{
		KeyFrame2[i].Angt1=0.0f;
		KeyFrame2[i].Angt2=0.0f;
		KeyFrame2[i].Angc1=0.0f;
		KeyFrame2[i].Angc2=0.0f;
		KeyFrame2[i].Angbi1=0.0f;
		KeyFrame2[i].Angbi2=0.0f;
		KeyFrame2[i].Angbib=0.0f;
		KeyFrame2[i].Angbd1=0.0f;
		KeyFrame2[i].Angbd2=0.0f;
		KeyFrame2[i].Angbdb=0.0f;
		KeyFrame2[i].Angpizq=0.0f;
		KeyFrame2[i].Angpder=0.0f;
		KeyFrame2[i].Angpi=0.0f;
		KeyFrame2[i].Angpd=0.0f;
		KeyFrame2[i].Xtor=0.0f;
		KeyFrame2[i].Ytor=0.0f;
		KeyFrame2[i].Ztor=0.0f;

		KeyFrame2[i].incAngt1=0.0f;
		KeyFrame2[i].incAngt1=0.0f;
		KeyFrame2[i].incAngc1=0.0f;
		KeyFrame2[i].incAngc2=0.0f;
		KeyFrame2[i].incAngbi1=0.0f;
		KeyFrame2[i].incAngbi2=0.0f;
		KeyFrame2[i].incAngbib=0.0f;
		KeyFrame2[i].incAngbd1=0.0f;
		KeyFrame2[i].incAngbd2=0.0f;
		KeyFrame2[i].incAngbdb=0.0f;
		KeyFrame2[i].incAngpizq=0.0f;
		KeyFrame2[i].incAngpder=0.0f;
		KeyFrame2[i].incAngpi=0.0f;
		KeyFrame2[i].incAngpd=0.0f;
		KeyFrame2[i].incXtor=0.0f;
		KeyFrame2[i].incYtor=0.0f;
		KeyFrame2[i].incZtor=0.0f;
	}
	
	for(int i=0; i<maxKF3; i++)
	{
		KeyFrame3[i].Angt1=0.0f;
		KeyFrame3[i].Angt2=0.0f;
		KeyFrame3[i].Angc1=0.0f;
		KeyFrame3[i].Angc2=0.0f;
		KeyFrame3[i].Angbi1=0.0f;
		KeyFrame3[i].Angbi2=0.0f;
		KeyFrame3[i].Angbib=0.0f;
		KeyFrame3[i].Angbd1=0.0f;
		KeyFrame3[i].Angbd2=0.0f;
		KeyFrame3[i].Angbdb=0.0f;
		KeyFrame3[i].Angpizq=0.0f;
		KeyFrame3[i].Angpder=0.0f;
		KeyFrame3[i].Angpi=0.0f;
		KeyFrame3[i].Angpd=0.0f;
		KeyFrame3[i].Xtor=0.0f;
		KeyFrame3[i].Ytor=0.0f;
		KeyFrame3[i].Ztor=0.0f;

		KeyFrame3[i].incAngt1=0.0f;
		KeyFrame3[i].incAngt1=0.0f;
		KeyFrame3[i].incAngc1=0.0f;
		KeyFrame3[i].incAngc2=0.0f;
		KeyFrame3[i].incAngbi1=0.0f;
		KeyFrame3[i].incAngbi2=0.0f;
		KeyFrame3[i].incAngbib=0.0f;
		KeyFrame3[i].incAngbd1=0.0f;
		KeyFrame3[i].incAngbd2=0.0f;
		KeyFrame3[i].incAngbdb=0.0f;
		KeyFrame3[i].incAngpizq=0.0f;
		KeyFrame3[i].incAngpder=0.0f;
		KeyFrame3[i].incAngpi=0.0f;
		KeyFrame3[i].incAngpd=0.0f;
		KeyFrame3[i].incXtor=0.0f;
		KeyFrame3[i].incYtor=0.0f;
		KeyFrame3[i].incZtor=0.0f;
	}
}

void DatosAnimacion()
{
	//Secuencia 1 (caminar)
	KeyFrame1[0].Angt1=10.0f;
	KeyFrame1[0].Angt2=0.0f;
	KeyFrame1[0].Angc1=-10.0f;
	KeyFrame1[0].Angc2=0.0f;
	KeyFrame1[0].Angbi1=-50.0f;
	KeyFrame1[0].Angbi2=0.0f;
	KeyFrame1[0].Angbib=0.0f;
	KeyFrame1[0].Angbd1=60.0f;
	KeyFrame1[0].Angbd2=0.0f;
	KeyFrame1[0].Angbdb=0.0f;
	KeyFrame1[0].Angpizq=40.0f;
	KeyFrame1[0].Angpder=-60.0f;
	KeyFrame1[0].Angpi=30.0f;
	KeyFrame1[0].Angpd=40.0f;
	KeyFrame1[0].Xtor=0.0f;
	KeyFrame1[0].Ytor=0.0f;
	KeyFrame1[0].Ztor=0.0f;

	KeyFrame1[1].Angt1=10.0f;
	KeyFrame1[1].Angt2=0.0f;
	KeyFrame1[1].Angc1=-10.0f;
	KeyFrame1[1].Angc2=0.0f;
	KeyFrame1[1].Angbi1=60.0f;
	KeyFrame1[1].Angbi2=0.0f;
	KeyFrame1[1].Angbib=0.0f;
	KeyFrame1[1].Angbd1=-50.0f;
	KeyFrame1[1].Angbd2=0.0f;
	KeyFrame1[1].Angbdb=0.0f;
	KeyFrame1[1].Angpizq=-60.0f;
	KeyFrame1[1].Angpder=40.0f;
	KeyFrame1[1].Angpi=40.0f;
	KeyFrame1[1].Angpd=30.0f;
	KeyFrame1[1].Xtor=0.0f;
	KeyFrame1[1].Ytor=0.0f;
	KeyFrame1[1].Ztor=0.0f;

	KeyFrame1[2].Angt1=10.0f;
	KeyFrame1[2].Angt2=0.0f;
	KeyFrame1[2].Angc1=-10.0f;
	KeyFrame1[2].Angc2=0.0f;
	KeyFrame1[2].Angbi1=-50.0f;
	KeyFrame1[2].Angbi2=0.0f;
	KeyFrame1[2].Angbib=0.0f;
	KeyFrame1[2].Angbd1=60.0f;
	KeyFrame1[2].Angbd2=0.0f;
	KeyFrame1[2].Angbdb=0.0f;
	KeyFrame1[2].Angpizq=40.0f;
	KeyFrame1[2].Angpder=-40.0f;
	KeyFrame1[2].Angpi=30.0f;
	KeyFrame1[2].Angpd=20.0f;
	KeyFrame1[2].Xtor=0.0f;
	KeyFrame1[2].Ytor=0.0f;
	KeyFrame1[2].Ztor=0.0f;

	//Secuencia 2 (Hurricane kick)
	KeyFrame2[0].Angt1=0.0f;
	KeyFrame2[0].Angt2=0.0f;
	KeyFrame2[0].Angc1=-20.0f;
	KeyFrame2[0].Angc2=0.0f;
	KeyFrame2[0].Angbi1=90.0f;
	KeyFrame2[0].Angbi2=30.0f;
	KeyFrame2[0].Angbib=0.0f;
	KeyFrame2[0].Angbd1=-100.0f;
	KeyFrame2[0].Angbd2=-50.0f;
	KeyFrame2[0].Angbdb=0.0f;
	KeyFrame2[0].Angpizq=0.0f;
	KeyFrame2[0].Angpder=-90.0f;
	KeyFrame2[0].Angpi=0.0f;
	KeyFrame2[0].Angpd=0.0f;
	KeyFrame2[0].Xtor=0.0f;
	KeyFrame2[0].Ytor=8.0f;
	KeyFrame2[0].Ztor=0.0f;

	KeyFrame2[1].Angt1=0.0f;
	KeyFrame2[1].Angt2=90.0f;
	KeyFrame2[1].Angc1=0.0f;
	KeyFrame2[1].Angc2=0.0f;
	KeyFrame2[1].Angbi1=90.0f;
	KeyFrame2[1].Angbi2=30.0f;
	KeyFrame2[1].Angbib=0.0f;
	KeyFrame2[1].Angbd1=-100.0f;
	KeyFrame2[1].Angbd2=-50.0f;
	KeyFrame2[1].Angbdb=0.0f;
	KeyFrame2[1].Angpizq=0.0f;
	KeyFrame2[1].Angpder=-90.0f;
	KeyFrame2[1].Angpi=0.0f;
	KeyFrame2[1].Angpd=0.0f;
	KeyFrame2[1].Xtor=0.0f;
	KeyFrame2[1].Ytor=8.0f;
	KeyFrame2[1].Ztor=0.0f;

	KeyFrame2[2].Angt1=0.0f;
	KeyFrame2[2].Angt2=180.0f;
	KeyFrame2[2].Angc1=-20.0f;
	KeyFrame2[2].Angc2=0.0f;
	KeyFrame2[2].Angbi1=90.0f;
	KeyFrame2[2].Angbi2=30.0f;
	KeyFrame2[2].Angbib=0.0f;
	KeyFrame2[2].Angbd1=-100.0f;
	KeyFrame2[2].Angbd2=-50.0f;
	KeyFrame2[2].Angbdb=0.0f;
	KeyFrame2[2].Angpizq=0.0f;
	KeyFrame2[2].Angpder=-90.0f;
	KeyFrame2[2].Angpi=0.0f;
	KeyFrame2[2].Angpd=0.0f;
	KeyFrame2[2].Xtor=0.0f;
	KeyFrame2[2].Ytor=8.0f;
	KeyFrame2[2].Ztor=0.0f;

	KeyFrame2[3].Angt1=0.0f;
	KeyFrame2[3].Angt2=270.0f;
	KeyFrame2[3].Angc1=0.0f;
	KeyFrame2[3].Angc2=0.0f;
	KeyFrame2[3].Angbi1=0.0f;
	KeyFrame2[3].Angbi2=30.0f;
	KeyFrame2[3].Angbib=0.0f;
	KeyFrame2[3].Angbd1=-100.0f;
	KeyFrame2[3].Angbd2=-50.0f;
	KeyFrame2[3].Angbdb=0.0f;
	KeyFrame2[3].Angpizq=0.0f;
	KeyFrame2[3].Angpder=-90.0f;
	KeyFrame2[3].Angpi=0.0f;
	KeyFrame2[3].Angpd=0.0f;
	KeyFrame2[3].Xtor=0.0f;
	KeyFrame2[3].Ytor=8.0f;
	KeyFrame2[3].Ztor=0.0f;

	//Secuencia 3 (salto)
	KeyFrame3[0].Angt1=6.0f;
	KeyFrame3[0].Angt2=0.0f;
	KeyFrame3[0].Angc1=-6.0f;
	KeyFrame3[0].Angc2=0.0f;
	KeyFrame3[0].Angbi1=10.0f;
	KeyFrame3[0].Angbi2=0.0f;
	KeyFrame3[0].Angbib=0.0f;
	KeyFrame3[0].Angbd1=10.0f;
	KeyFrame3[0].Angbd2=0.0f;
	KeyFrame3[0].Angbdb=0.0f;
	KeyFrame3[0].Angpizq=-26.0f;
	KeyFrame3[0].Angpder=0.0f;
	KeyFrame3[0].Angpi=11.0f;
	KeyFrame3[0].Angpd=0.0f;
	KeyFrame3[0].Xtor=0.0f;
	KeyFrame3[0].Ytor=0.0f;
	KeyFrame3[0].Ztor=0.0f;

	KeyFrame3[1].Angt1=18.0f;
	KeyFrame3[1].Angt2=0.0f;
	KeyFrame3[1].Angc1=-18.0f;
	KeyFrame3[1].Angc2=0.0f;
	KeyFrame3[1].Angbi1=30.0f;
	KeyFrame3[1].Angbi2=5.0f;
	KeyFrame3[1].Angbib=0.0f;
	KeyFrame3[1].Angbd1=30.0f;
	KeyFrame3[1].Angbd2=-5.0f;
	KeyFrame3[1].Angbdb=0.0f;
	KeyFrame3[1].Angpizq=-75.0f;
	KeyFrame3[1].Angpder=-10.0f;
	KeyFrame3[1].Angpi=55.0f;
	KeyFrame3[1].Angpd=0.0f;
	KeyFrame3[1].Xtor=0.0f;
	KeyFrame3[1].Ytor=0.0f;
	KeyFrame3[1].Ztor=0.0f;

	KeyFrame3[2].Angt1=20.0f;
	KeyFrame3[2].Angt2=0.0f;
	KeyFrame3[2].Angc1=-20.0f;
	KeyFrame3[2].Angc2=0.0f;
	KeyFrame3[2].Angbi1=60.0f;
	KeyFrame3[2].Angbi2=15.0f;
	KeyFrame3[2].Angbib=0.0f;
	KeyFrame3[2].Angbd1=60.0f;
	KeyFrame3[2].Angbd2=-15.0f;
	KeyFrame3[2].Angbdb=5.0f;
	KeyFrame3[2].Angpizq=-105.0f;
	KeyFrame3[2].Angpder=-20.0f;
	KeyFrame3[2].Angpi=100.0f;
	KeyFrame3[2].Angpd=15.0f;
	KeyFrame3[2].Xtor=0.0f;
	KeyFrame3[2].Ytor=0.0f;
	KeyFrame3[2].Ztor=0.0f;

}

void iniciaCajasdeColision()
{
	cajaPersonaje.pos = CVector(player1.PosicionObj.x, player1.PosicionObj.y + 3.0f, player1.PosicionObj.z);
	cajaPersonaje.tamaño = CVector(4.0f, 6.0f, 4.0f);
	cajaPersonaje.xMin = cajaPersonaje.pos.x - cajaPersonaje.tamaño.x*0.5f;
	cajaPersonaje.xMax = cajaPersonaje.pos.x + cajaPersonaje.tamaño.x*0.5f;
	cajaPersonaje.yMin = cajaPersonaje.pos.y - cajaPersonaje.tamaño.y*0.5f;
	cajaPersonaje.yMax = cajaPersonaje.pos.y + cajaPersonaje.tamaño.y*0.5f;
	cajaPersonaje.zMin = cajaPersonaje.pos.z - cajaPersonaje.tamaño.z*0.5f;
	cajaPersonaje.zMax = cajaPersonaje.pos.z + cajaPersonaje.tamaño.z*0.5f;


	cajaEscenario[0].pos = CVector(-45.0f, 5.0f, 40.0f); //Pared izquierda límite
	cajaEscenario[0].tamaño = CVector(7.0f, 8.0f, 80.0f);

	cajaEscenario[1].pos = CVector(45.0f, 5.0f, 40.0f); //Pared derecha límite
	cajaEscenario[1].tamaño = CVector(7.0f, 8.0f, 80.0f);

	cajaEscenario[2].pos = CVector(0.0f, 5.0f, 80.0f); //Pared inferior límite
	cajaEscenario[2].tamaño = CVector(80.0f, 8.0f, 7.0f);

	cajaEscenario[3].pos = CVector(0.0f, 5.0f, -2.0f); //Pared superior límite
	cajaEscenario[3].tamaño = CVector(80.0f, 8.0f, 7.0f);

	//CAJAS AMARILLAS DEL ESCENARIO

	cajaEscenario[4].pos = CVector(-32.0f, 5.0f, 13.0f); //Caja [1,1]
	cajaEscenario[4].tamaño = CVector(4.0f, 8.0f, 4.0f);

	cajaEscenario[5].pos = CVector(-32.0f, 5.0f, 26.0f); //Caja [1,2]
	cajaEscenario[5].tamaño = CVector(4.0f, 8.0f, 4.0f);

	cajaEscenario[6].pos = CVector(-32.0f, 5.0f, 39.0f); //Caja [1,3]
	cajaEscenario[6].tamaño = CVector(4.0f, 8.0f, 4.0f);

	cajaEscenario[7].pos = CVector(-32.0f, 5.0f, 52.0f); //Caja [1,4]
	cajaEscenario[7].tamaño = CVector(4.0f, 8.0f, 4.0f);

	cajaEscenario[8].pos = CVector(-18.0f, 5.0f, 13.0f); //Caja [2,1]
	cajaEscenario[8].tamaño = CVector(4.0f, 8.0f, 4.0f);

	cajaEscenario[9].pos = CVector(-18.0f, 5.0f, 26.0f); //Caja [2,2]
	cajaEscenario[9].tamaño = CVector(4.0f, 8.0f, 4.0f);

	cajaEscenario[10].pos = CVector(-18.0f, 5.0f, 39.0f); //Caja [2,3]
	cajaEscenario[10].tamaño = CVector(4.0f, 8.0f, 4.0f);

	cajaEscenario[11].pos = CVector(-18.0f, 5.0f, 52.0f); //Caja [2,4]
	cajaEscenario[11].tamaño = CVector(4.0f, 8.0f, 4.0f);

	cajaEscenario[12].pos = CVector(-5.0f, 5.0f, 13.0f); //Caja [3,1]
	cajaEscenario[12].tamaño = CVector(4.0f, 8.0f, 4.0f);

	cajaEscenario[13].pos = CVector(-5.0f, 5.0f, 26.0f); //Caja [3,2]
	cajaEscenario[13].tamaño = CVector(4.0f, 8.0f, 4.0f);

	cajaEscenario[14].pos = CVector(-5.0f, 5.0f, 39.0f); //Caja [3,3]
	cajaEscenario[14].tamaño = CVector(4.0f, 8.0f, 4.0f);

	cajaEscenario[15].pos = CVector(-5.0f, 5.0f, 52.0f); //Caja [3,4]
	cajaEscenario[15].tamaño = CVector(4.0f, 8.0f, 4.0f);

	cajaEscenario[16].pos = CVector(7.0f, 5.0f, 13.0f); //Caja [4,1]
	cajaEscenario[16].tamaño = CVector(4.0f, 8.0f, 4.0f);

	cajaEscenario[17].pos = CVector(7.0f, 5.0f, 26.0f); //Caja [4,2]
	cajaEscenario[17].tamaño = CVector(4.0f, 8.0f, 4.0f);

	cajaEscenario[18].pos = CVector(7.0f, 5.0f, 39.0f); //Caja [4,3]
	cajaEscenario[18].tamaño = CVector(4.0f, 8.0f, 4.0f);

	cajaEscenario[19].pos = CVector(7.0f, 5.0f, 52.0f); //Caja [4,4]
	cajaEscenario[19].tamaño = CVector(4.0f, 8.0f, 4.0f);

	cajaEscenario[20].pos = CVector(20.0f, 5.0f, 13.0f); //Caja [5,1]
	cajaEscenario[20].tamaño = CVector(4.0f, 8.0f, 4.0f);

	cajaEscenario[21].pos = CVector(20.0f, 5.0f, 26.0f); //Caja [5,2]
	cajaEscenario[21].tamaño = CVector(4.0f, 8.0f, 4.0f);

	cajaEscenario[22].pos = CVector(20.0f, 5.0f, 39.0f); //Caja [5,3]
	cajaEscenario[22].tamaño = CVector(4.0f, 8.0f, 4.0f);

	cajaEscenario[23].pos = CVector(20.0f, 5.0f, 52.0f); //Caja [5,4]
	cajaEscenario[23].tamaño = CVector(4.0f, 8.0f, 4.0f);

	cajaEscenario[24].pos = CVector(33.0f, 5.0f, 13.0f); //Caja [6,1]
	cajaEscenario[24].tamaño = CVector(4.0f, 8.0f, 4.0f);

	cajaEscenario[25].pos = CVector(33.0f, 5.0f, 26.0f); //Caja [6,2]
	cajaEscenario[25].tamaño = CVector(4.0f, 8.0f, 4.0f);

	cajaEscenario[26].pos = CVector(33.0f, 5.0f, 39.0f); //Caja [6,3]
	cajaEscenario[26].tamaño = CVector(4.0f, 8.0f, 4.0f);

	cajaEscenario[27].pos = CVector(33.0f, 5.0f, 52.0f); //Caja [6,4]
	cajaEscenario[27].tamaño = CVector(4.0f, 8.0f, 4.0f);

	cajaEscenario[28].pos = CVector(-32.0f, 5.0f, 65.0f); //Caja [1,5]
	cajaEscenario[28].tamaño = CVector(4.0f, 8.0f, 4.0f);

	cajaEscenario[29].pos = CVector(-18.0f, 5.0f, 65.0f); //Caja [2,5]
	cajaEscenario[29].tamaño = CVector(4.0f, 8.0f, 4.0f);

	cajaEscenario[30].pos = CVector(-5.0f, 5.0f, 65.0f); //Caja [3,5]
	cajaEscenario[30].tamaño = CVector(4.0f, 8.0f, 4.0f);

	cajaEscenario[31].pos = CVector(7.0f, 5.0f, 65.0f); //Caja [4,5]
	cajaEscenario[31].tamaño = CVector(4.0f, 8.0f, 4.0f);

	cajaEscenario[32].pos = CVector(20.0f, 5.0f, 65.0f); //Caja [5,5]
	cajaEscenario[32].tamaño = CVector(4.0f, 8.0f, 4.0f);

	cajaEscenario[33].pos = CVector(33.0f, 5.0f, 65.0f); //Caja [6,5]
	cajaEscenario[33].tamaño = CVector(4.0f, 8.0f, 4.0f);

	//CAJAS EXTRAS

	cajaEscenario[34].pos = CVector(33.0f, 5.0f, 32.5f); //Caja [6 , 2.5]
	cajaEscenario[34].tamaño = CVector(4.0f, 8.0f, 4.0f);

	cajaEscenario[35].pos = CVector(26.5f, 5.0f, 39.0f); //Caja [5.5 , 3]
	cajaEscenario[35].tamaño = CVector(4.0f, 8.0f, 4.0f);

	cajaEscenario[36].pos = CVector(20.0f, 5.0f, 45.5f); //Caja [5 , 3.5]
	cajaEscenario[36].tamaño = CVector(4.0f, 8.0f, 4.0f);

	cajaEscenario[37].pos = CVector(26.5f, 5.0f, 52.0f); //Caja [5.5 , 3]
	cajaEscenario[37].tamaño = CVector(4.0f, 8.0f, 4.0f);

	cajaEscenario[38].pos = CVector(-5.0f, 5.0f, 58.5f); //Caja [3, 4.5]
	cajaEscenario[38].tamaño = CVector(4.0f, 8.0f, 4.0f);

	cajaEscenario[39].pos = CVector(33.0f, 5.0f, 58.5f); //Caja [6 , 4.5]
	cajaEscenario[39].tamaño = CVector(4.0f, 8.0f, 4.0f);

	// CAJAS ROJAS ESCENARIO

	cajaEscenario[40].pos = CVector(-26.0f, 5.0f, 7.0f); //1
	cajaEscenario[40].tamaño = CVector(4.0f, 8.0f, 4.0f);

	cajaEscenario[41].pos = CVector(1.0f, 5.0f, 7.0f); //2
	cajaEscenario[41].tamaño = CVector(4.0f, 8.0f, 4.0f);

	cajaEscenario[42].pos = CVector(-26.0f, 5.0f, 13.5f); //3
	cajaEscenario[42].tamaño = CVector(4.0f, 8.0f, 4.0f);

	cajaEscenario[43].pos = CVector(-11.5f, 5.0f, 13.5f); // 4
	cajaEscenario[43].tamaño = CVector(4.0f, 8.0f, 4.0f);

	cajaEscenario[44].pos = CVector(13.5f, 5.0f, 13.5f); // 5
	cajaEscenario[44].tamaño = CVector(4.0f, 8.0f, 4.0f);

	cajaEscenario[45].pos = CVector(26.5f, 5.0f, 20.0f); //6
	cajaEscenario[45].tamaño = CVector(4.0f, 8.0f, 4.0f);

	cajaEscenario[46].pos = CVector(33.0f, 5.0f, 20.0f); //7
	cajaEscenario[46].tamaño = CVector(4.0f, 8.0f, 4.0f);

	cajaEscenario[47].pos = CVector(1.0f, 5.0f, 26.0f); //8
	cajaEscenario[47].tamaño = CVector(4.0f, 8.0f, 4.0f);

	cajaEscenario[48].pos = CVector(-38.5f, 5.0f, 32.5f); //9
	cajaEscenario[48].tamaño = CVector(4.0f, 8.0f, 4.0f);

	cajaEscenario[49].pos = CVector(-24.5f, 5.0f, 32.5f); //10
	cajaEscenario[49].tamaño = CVector(4.0f, 8.0f, 4.0f);

	cajaEscenario[50].pos = CVector(-18.0f, 5.0f, 32.5f); //11
	cajaEscenario[50].tamaño = CVector(4.0f, 8.0f, 4.0f);

	cajaEscenario[51].pos = CVector(7.0f, 5.0f, 32.5f); //12
	cajaEscenario[51].tamaño = CVector(4.0f, 8.0f, 4.0f);

	cajaEscenario[52].pos = CVector(13.5f, 5.0f, 32.5f); //13
	cajaEscenario[52].tamaño = CVector(4.0f, 8.0f, 4.0f);

	cajaEscenario[53].pos = CVector(20.0f, 5.0f, 32.5f); //14
	cajaEscenario[53].tamaño = CVector(4.0f, 8.0f, 4.0f);

	cajaEscenario[54].pos = CVector(26.5f, 5.0f, 32.5f); //15
	cajaEscenario[54].tamaño = CVector(4.0f, 8.0f, 4.0f);

	cajaEscenario[55].pos = CVector(-11.5f, 5.0f, 39.0f); //16
	cajaEscenario[55].tamaño = CVector(4.0f, 8.0f, 4.0f);

	cajaEscenario[56].pos = CVector(13.5f, 5.0f, 39.0f); //17
	cajaEscenario[56].tamaño = CVector(4.0f, 8.0f, 4.0f);

	cajaEscenario[57].pos = CVector(39.0f, 5.0f, 39.0f); //18
	cajaEscenario[57].tamaño = CVector(4.0f, 8.0f, 4.0f);

	cajaEscenario[58].pos = CVector(-11.5f, 5.0f, 45.5f); //19
	cajaEscenario[58].tamaño = CVector(4.0f, 8.0f, 4.0f);

	cajaEscenario[59].pos = CVector(0.5f, 5.0f, 45.5f); //20
	cajaEscenario[59].tamaño = CVector(4.0f, 8.0f, 4.0f);

	cajaEscenario[60].pos = CVector(-11.5f, 5.0f, 52.0f); //21
	cajaEscenario[60].tamaño = CVector(4.0f, 8.0f, 4.0f);

	cajaEscenario[61].pos = CVector(-32.0f, 5.0f, 58.5f); //22
	cajaEscenario[61].tamaño = CVector(4.0f, 8.0f, 4.0f);

	cajaEscenario[62].pos = CVector(-11.5f, 5.0f, 58.5f); //23
	cajaEscenario[62].tamaño = CVector(4.0f, 8.0f, 4.0f);

	cajaEscenario[63].pos = CVector(7.0f, 5.0f, 58.5f); //24
	cajaEscenario[63].tamaño = CVector(4.0f, 8.0f, 4.0f);

	cajaEscenario[64].pos = CVector(13.5f, 5.0f, 58.5f); //25
	cajaEscenario[64].tamaño = CVector(4.0f, 8.0f, 4.0f);

	cajaEscenario[65].pos = CVector(26.5f, 5.0f, 58.5f); //26
	cajaEscenario[65].tamaño = CVector(4.0f, 8.0f, 4.0f);

	cajaEscenario[66].pos = CVector(-38.5f, 5.0f, 65.0f); //27
	cajaEscenario[66].tamaño = CVector(4.0f, 8.0f, 4.0f);

	cajaEscenario[67].pos = CVector(-38.5f, 5.0f, 71.5f); //28
	cajaEscenario[67].tamaño = CVector(4.0f, 8.0f, 4.0f);

	cajaEscenario[68].pos = CVector(-24.5f, 5.0f, 71.5f); //29
	cajaEscenario[68].tamaño = CVector(4.0f, 8.0f, 4.0f);

	cajaEscenario[69].pos = CVector(-18.0f, 5.0f, 71.5f); //30
	cajaEscenario[69].tamaño = CVector(4.0f, 8.0f, 4.0f);

	cajaEscenario[70].pos = CVector(0.5f, 5.0f, 71.5f); //31
	cajaEscenario[70].tamaño = CVector(4.0f, 8.0f, 4.0f);

	cajaEscenario[71].pos = CVector(13.5f, 5.0f, 71.5f); //32
	cajaEscenario[71].tamaño = CVector(4.0f, 8.0f, 4.0f);

	cajaEscenario[72].pos = CVector(33.0f, 5.0f, 71.5f); //33
	cajaEscenario[72].tamaño = CVector(4.0f, 8.0f, 4.0f);

	cajaEscenario[73].pos = CVector(39.0f, 5.0f, 65.0f); //34
	cajaEscenario[73].tamaño = CVector(4.0f, 8.0f, 4.0f);

	for (int i = 0; i<200; i++)
	{
		cajaEscenario[i].xMin = cajaEscenario[i].pos.x - cajaEscenario[i].tamaño.x*0.5f;
		cajaEscenario[i].xMax = cajaEscenario[i].pos.x + cajaEscenario[i].tamaño.x*0.5f;
		cajaEscenario[i].yMin = cajaEscenario[i].pos.y - cajaEscenario[i].tamaño.y*0.5f;
		cajaEscenario[i].yMax = cajaEscenario[i].pos.y + cajaEscenario[i].tamaño.y*0.5f;
		cajaEscenario[i].zMin = cajaEscenario[i].pos.z - cajaEscenario[i].tamaño.z*0.5f;
		cajaEscenario[i].zMax = cajaEscenario[i].pos.z + cajaEscenario[i].tamaño.z*0.5f;
	}
}

void dibujaCajaColision(cajaCol *caja)
{
	glDisable(GL_LIGHTING);
	glColor3f(1.0f, 1.0f, 1.0f);
	glLineWidth(3.0f);

	glBegin(GL_LINE_LOOP);
	glVertex3f(caja->xMin, caja->yMin, caja->zMax);
	glVertex3f(caja->xMax, caja->yMin, caja->zMax);
	glVertex3f(caja->xMax, caja->yMax, caja->zMax);
	glVertex3f(caja->xMin, caja->yMax, caja->zMax);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex3f(caja->xMin, caja->yMin, caja->zMin);
	glVertex3f(caja->xMax, caja->yMin, caja->zMin);
	glVertex3f(caja->xMax, caja->yMax, caja->zMin);
	glVertex3f(caja->xMin, caja->yMax, caja->zMin);
	glEnd();

	glBegin(GL_LINES);
	glVertex3f(caja->xMin, caja->yMax, caja->zMin);
	glVertex3f(caja->xMin, caja->yMax, caja->zMax);

	glVertex3f(caja->xMax, caja->yMax, caja->zMin);
	glVertex3f(caja->xMax, caja->yMax, caja->zMax);

	glVertex3f(caja->xMin, caja->yMin, caja->zMin);
	glVertex3f(caja->xMin, caja->yMin, caja->zMax);

	glVertex3f(caja->xMax, caja->yMin, caja->zMin);
	glVertex3f(caja->xMax, caja->yMin, caja->zMax);
	glEnd();

	glColor3f(1.0f, 1.0f, 1.0f);
	glLineWidth(1.0f);
	glEnable(GL_LIGHTING);
}

void iniCajaBomba()
{
	cajaBomba.pos = CVector(-100.0f,-100.0f,-100.0f);
	cajaBomba.tamaño = CVector(6.0f, 8.0f, 6.0f);
	cajaBomba.xMin = cajaBomba.pos.x - cajaBomba.tamaño.x*0.5f;
	cajaBomba.xMax = cajaBomba.pos.x + cajaBomba.tamaño.x*0.5f;
	cajaBomba.yMin = cajaBomba.pos.y - cajaBomba.tamaño.y*0.5f;
	cajaBomba.yMax = cajaBomba.pos.y + cajaBomba.tamaño.y*0.5f;
	cajaBomba.zMin = cajaBomba.pos.z - cajaBomba.tamaño.z*0.5f;
	cajaBomba.zMax = cajaBomba.pos.z + cajaBomba.tamaño.z*0.5f;
}

void actualizaCajaBomba()
{
	cajaBomba.pos = CVector(PosicionBomba.x, PosicionBomba.y, PosicionBomba.z);
	cajaBomba.xMin = cajaBomba.pos.x - cajaBomba.tamaño.x*0.5f;
	cajaBomba.xMax = cajaBomba.pos.x + cajaBomba.tamaño.x*0.5f;
	cajaBomba.yMin = cajaBomba.pos.y - cajaBomba.tamaño.y*0.5f;
	cajaBomba.yMax = cajaBomba.pos.y + cajaBomba.tamaño.y*0.5f;
	cajaBomba.zMin = cajaBomba.pos.z - cajaBomba.tamaño.z*0.5f;
	cajaBomba.zMax = cajaBomba.pos.z + cajaBomba.tamaño.z*0.5f;
}

void iniCajaParticula()
{
	cajaParticula.pos = CVector(-100.0f, -100.0f, -100.0f);
	cajaParticula.tamaño = CVector(24.0f, 8.0f, 24.0f);
	cajaParticula.xMin = cajaParticula.pos.x - cajaParticula.tamaño.x*0.5f;
	cajaParticula.xMax = cajaParticula.pos.x + cajaParticula.tamaño.x*0.5f;
	cajaParticula.yMin = cajaParticula.pos.y - cajaParticula.tamaño.y*0.5f;
	cajaParticula.yMax = cajaParticula.pos.y + cajaParticula.tamaño.y*0.5f;
	cajaParticula.zMin = cajaParticula.pos.z - cajaParticula.tamaño.z*0.5f;
	cajaParticula.zMax = cajaParticula.pos.z + cajaParticula.tamaño.z*0.5f;
}

void actualizaCajaParticula()

{	cajaParticula.pos = CVector(PosicionBomba.x, PosicionBomba.y, PosicionBomba.z);
	cajaParticula.xMin = cajaParticula.pos.x - cajaParticula.tamaño.x*0.5f;
	cajaParticula.xMax = cajaParticula.pos.x + cajaParticula.tamaño.x*0.5f;
	cajaParticula.yMin = cajaParticula.pos.y - cajaParticula.tamaño.y*0.5f;
	cajaParticula.yMax = cajaParticula.pos.y + cajaParticula.tamaño.y*0.5f;
	cajaParticula.zMin = cajaParticula.pos.z - cajaParticula.tamaño.z*0.5f;
	cajaParticula.zMax = cajaParticula.pos.z + cajaParticula.tamaño.z*0.5f;
}

bool colisionBomba(int dir)
{
	bool col = false;
	CVector pSig;
	CVector A, B, C, D, E, F, G, H;

	if (dir == 1) //x+
	{
		pSig.x = player1.PosicionObj.x + player1.VelocidadObj;
		pSig.y = player1.PosicionObj.y;
		pSig.z = player1.PosicionObj.z;
	}
	else if (dir == 2) //x-
	{
		pSig.x = player1.PosicionObj.x - player1.VelocidadObj;
		pSig.y = player1.PosicionObj.y;
		pSig.z = player1.PosicionObj.z;
	}
	else if (dir == 3) //z+
	{
		pSig.x = player1.PosicionObj.x;
		pSig.y = player1.PosicionObj.y;
		pSig.z = player1.PosicionObj.z + player1.VelocidadObj;
	}
	else if (dir == 4) //z-
	{
		pSig.x = player1.PosicionObj.x;
		pSig.y = player1.PosicionObj.y;
		pSig.z = player1.PosicionObj.z - player1.VelocidadObj;
	}
	else if (dir == 5) //y+
	{
		pSig.x = player1.PosicionObj.x;
		pSig.y = player1.PosicionObj.y + player1.VelocidadObj;
		pSig.z = player1.PosicionObj.z;
	}
	else if (dir == 6) //y-
	{
		pSig.x = player1.PosicionObj.x;
		pSig.y = player1.PosicionObj.y - player1.VelocidadObj;
		pSig.z = player1.PosicionObj.z;
	}

	cajaPersonaje.pos = CVector(pSig.x, pSig.y + 3.0f, pSig.z);
	cajaPersonaje.xMin = cajaPersonaje.pos.x - cajaPersonaje.tamaño.x*0.5f;
	cajaPersonaje.xMax = cajaPersonaje.pos.x + cajaPersonaje.tamaño.x*0.5f;
	cajaPersonaje.yMin = cajaPersonaje.pos.y - cajaPersonaje.tamaño.y*0.5f;
	cajaPersonaje.yMax = cajaPersonaje.pos.y + cajaPersonaje.tamaño.y*0.5f;
	cajaPersonaje.zMin = cajaPersonaje.pos.z - cajaPersonaje.tamaño.z*0.5f;
	cajaPersonaje.zMax = cajaPersonaje.pos.z + cajaPersonaje.tamaño.z*0.5f;

	//Inf-izq-front
	A.x = cajaPersonaje.xMin;
	A.y = cajaPersonaje.yMin;
	A.z = cajaPersonaje.zMax;

	//Inf-der-front
	B.x = cajaPersonaje.xMax;
	B.y = cajaPersonaje.yMin;
	B.z = cajaPersonaje.zMax;

	//Sup-der-front
	C.x = cajaPersonaje.xMax;
	C.y = cajaPersonaje.yMax;
	C.z = cajaPersonaje.zMax;

	//Sup-izq-front
	D.x = cajaPersonaje.xMin;
	D.y = cajaPersonaje.yMax;
	D.z = cajaPersonaje.zMax;

	//Inf-izq-tras
	E.x = cajaPersonaje.xMin;
	E.y = cajaPersonaje.yMin;
	E.z = cajaPersonaje.zMin;

	//Inf-der-tras
	F.x = cajaPersonaje.xMax;
	F.y = cajaPersonaje.yMin;
	F.z = cajaPersonaje.zMin;

	//Sup-der-tras
	G.x = cajaPersonaje.xMax;
	G.y = cajaPersonaje.yMax;
	G.z = cajaPersonaje.zMin;

	//Sup-izq-tras
	H.x = cajaPersonaje.xMin;
	H.y = cajaPersonaje.yMax;
	H.z = cajaPersonaje.zMin;

	if ((A.x > cajaBomba.xMin && A.x < cajaBomba.xMax && A.y > cajaBomba.yMin && A.y < cajaBomba.yMax && A.z > cajaBomba.zMin && A.z < cajaBomba.zMax)
		|| (B.x > cajaBomba.xMin && B.x < cajaBomba.xMax && B.y > cajaBomba.yMin && B.y < cajaBomba.yMax && B.z > cajaBomba.zMin && B.z < cajaBomba.zMax)
		|| (C.x > cajaBomba.xMin && C.x < cajaBomba.xMax && C.y > cajaBomba.yMin && C.y < cajaBomba.yMax && C.z > cajaBomba.zMin && C.z < cajaBomba.zMax)
		|| (D.x > cajaBomba.xMin && D.x < cajaBomba.xMax && D.y > cajaBomba.yMin && D.y < cajaBomba.yMax && D.z > cajaBomba.zMin && D.z < cajaBomba.zMax)
		|| (E.x > cajaBomba.xMin && E.x < cajaBomba.xMax && E.y > cajaBomba.yMin && E.y < cajaBomba.yMax && E.z > cajaBomba.zMin && E.z < cajaBomba.zMax)
		|| (F.x > cajaBomba.xMin && F.x < cajaBomba.xMax && F.y > cajaBomba.yMin && F.y < cajaBomba.yMax && F.z > cajaBomba.zMin && F.z < cajaBomba.zMax)
		|| (G.x > cajaBomba.xMin && G.x < cajaBomba.xMax && G.y > cajaBomba.yMin && G.y < cajaBomba.yMax && G.z > cajaBomba.zMin && G.z < cajaBomba.zMax)
		|| (H.x > cajaBomba.xMin && H.x < cajaBomba.xMax && H.y > cajaBomba.yMin && H.y < cajaBomba.yMax && H.z > cajaBomba.zMin && H.z < cajaBomba.zMax))
	{
		col = true;

		if (dir == 1) //x+
			player1.PosicionObj.x = cajaBomba.xMin - cajaBomba.tamaño.x*0.5f;
		else if (dir == 2) //x-
			player1.PosicionObj.x = cajaBomba.xMax + cajaPersonaje.tamaño.x*0.5f;
		else if (dir == 3) //z+
			player1.PosicionObj.z = cajaBomba.zMin - cajaPersonaje.tamaño.z*0.5f;
		else if (dir == 4) //z-
			player1.PosicionObj.z = cajaBomba.zMax + cajaPersonaje.tamaño.z*0.5f;
		else if (dir == 5) //y+
			player1.PosicionObj.y = cajaBomba.yMin - cajaPersonaje.tamaño.y*0.5f - 3.0f;
		else if (dir == 6) //y-
			player1.PosicionObj.y = cajaBomba.yMax;

	}

	return col;
}

bool ColParticulas(int dir)
{
	bool col = false;
	CVector pSig;
	CVector A, B, C, D, E, F, G, H;

	for (int i = 40; i<numCajas; i++)
	{
		if (dir == 1) //x+
		{
			pSig.x = player1.PosicionObj.x + player1.VelocidadObj;
			pSig.y = player1.PosicionObj.y;
			pSig.z = player1.PosicionObj.z;
		}
		else if (dir == 2) //x-
		{
			pSig.x = player1.PosicionObj.x - player1.VelocidadObj;
			pSig.y = player1.PosicionObj.y;
			pSig.z = player1.PosicionObj.z;
		}
		else if (dir == 3) //z+
		{
			pSig.x = player1.PosicionObj.x;
			pSig.y = player1.PosicionObj.y;
			pSig.z = player1.PosicionObj.z + player1.VelocidadObj;
		}
		else if (dir == 4) //z-
		{
			pSig.x = player1.PosicionObj.x;
			pSig.y = player1.PosicionObj.y;
			pSig.z = player1.PosicionObj.z - player1.VelocidadObj;
		}


		cajaEscenario[i].pos = CVector(pSig.x, pSig.y + 3.0f, pSig.z);
		cajaEscenario[i].xMin = cajaEscenario[i].pos.x - cajaEscenario[i].tamaño.x*0.5f;
		cajaEscenario[i].xMax = cajaEscenario[i].pos.x + cajaEscenario[i].tamaño.x*0.5f;
		cajaEscenario[i].zMin = cajaEscenario[i].pos.z - cajaEscenario[i].tamaño.z*0.5f;
		cajaEscenario[i].zMax = cajaEscenario[i].pos.z + cajaEscenario[i].tamaño.z*0.5f;

		//Inf-izq-front
		A.x = cajaEscenario[i].xMin;
		A.y = cajaEscenario[i].yMin;
		A.z = cajaEscenario[i].zMax;

		//Inf-der-front
		B.x = cajaEscenario[i].xMax;
		B.y = cajaEscenario[i].yMin;
		B.z = cajaEscenario[i].zMax;

		//Sup-der-front
		C.x = cajaEscenario[i].xMax;
		C.y = cajaEscenario[i].yMax;
		C.z = cajaEscenario[i].zMax;

		//Sup-izq-front
		D.x = cajaEscenario[i].xMin;
		D.y = cajaEscenario[i].yMax;
		D.z = cajaEscenario[i].zMax;

		//Inf-izq-tras
		E.x = cajaEscenario[i].xMin;
		E.y = cajaEscenario[i].yMin;
		E.z = cajaEscenario[i].zMin;

		//Inf-der-tras
		F.x = cajaEscenario[i].xMax;
		F.y = cajaEscenario[i].yMin;
		F.z = cajaEscenario[i].zMin;

		//Sup-der-tras
		G.x = cajaEscenario[i].xMax;
		G.y = cajaEscenario[i].yMax;
		G.z = cajaEscenario[i].zMin;

		//Sup-izq-tras
		H.x = cajaEscenario[i].xMin;
		H.y = cajaEscenario[i].yMax;
		H.z = cajaEscenario[i].zMin;

		if ((A.x > cajaParticula.xMin && A.x < cajaParticula.xMax && A.y > cajaParticula.yMin && A.y < cajaParticula.yMax && A.z > cajaParticula.zMin && A.z < cajaParticula.zMax)
			|| (B.x > cajaParticula.xMin && B.x < cajaParticula.xMax && B.y > cajaParticula.yMin && B.y < cajaParticula.yMax && B.z > cajaParticula.zMin && B.z < cajaParticula.zMax)
			|| (C.x > cajaParticula.xMin && C.x < cajaParticula.xMax && C.y > cajaParticula.yMin && C.y < cajaParticula.yMax && C.z > cajaParticula.zMin && C.z < cajaParticula.zMax)
			|| (D.x > cajaParticula.xMin && D.x < cajaParticula.xMax && D.y > cajaParticula.yMin && D.y < cajaParticula.yMax && D.z > cajaParticula.zMin && D.z < cajaParticula.zMax)
			|| (E.x > cajaParticula.xMin && E.x < cajaParticula.xMax && E.y > cajaParticula.yMin && E.y < cajaParticula.yMax && E.z > cajaParticula.zMin && E.z < cajaParticula.zMax)
			|| (F.x > cajaParticula.xMin && F.x < cajaParticula.xMax && F.y > cajaParticula.yMin && F.y < cajaParticula.yMax && F.z > cajaParticula.zMin && F.z < cajaParticula.zMax)
			|| (G.x > cajaParticula.xMin && G.x < cajaParticula.xMax && G.y > cajaParticula.yMin && G.y < cajaParticula.yMax && G.z > cajaParticula.zMin && G.z < cajaParticula.zMax)
			|| (H.x > cajaParticula.xMin && H.x < cajaParticula.xMax && H.y > cajaParticula.yMin && H.y < cajaParticula.yMax && H.z > cajaParticula.zMin && H.z < cajaParticula.zMax))
		{
			col = true;

			if (dir == 1) //x+
				cajaEscenario[i].pos = CVector(0.0f, -25.0f, 0.0f);
			else if (dir == 2) //x-
				cajaEscenario[i].pos = CVector(0.0f, -25.0f, 0.0f);
			else if (dir == 3) //z+
				cajaEscenario[i].pos = CVector(0.0f, -25.0f, 0.0f);
			else if (dir == 4) //z-
				cajaEscenario[i].pos = CVector(0.0f, -25.0f, 0.0f);


			break;
		}
	}

	return col;
}

bool colisionCajas(int dir)
{
	bool col = false;
	CVector pSig;
	CVector A, B, C, D, E, F, G, H;

	for (int i = 0; i<numCajas; i++)
	{
		if (dir == 1) //x+
		{
			pSig.x = player1.PosicionObj.x + player1.VelocidadObj;
			pSig.y = player1.PosicionObj.y;
			pSig.z = player1.PosicionObj.z;
		}
		else if (dir == 2) //x-
		{
			pSig.x = player1.PosicionObj.x - player1.VelocidadObj;
			pSig.y = player1.PosicionObj.y;
			pSig.z = player1.PosicionObj.z;
		}
		else if (dir == 3) //z+
		{
			pSig.x = player1.PosicionObj.x;
			pSig.y = player1.PosicionObj.y;
			pSig.z = player1.PosicionObj.z + player1.VelocidadObj;
		}
		else if (dir == 4) //z-
		{
			pSig.x = player1.PosicionObj.x;
			pSig.y = player1.PosicionObj.y;
			pSig.z = player1.PosicionObj.z - player1.VelocidadObj;
		}
		else if (dir == 5) //y+
		{
			pSig.x = player1.PosicionObj.x;
			pSig.y = player1.PosicionObj.y + player1.VelocidadObj;
			pSig.z = player1.PosicionObj.z;
		}
		else if (dir == 6) //y-
		{
			pSig.x = player1.PosicionObj.x;
			pSig.y = player1.PosicionObj.y - player1.VelocidadObj;
			pSig.z = player1.PosicionObj.z;
		}

		cajaPersonaje.pos = CVector(pSig.x, pSig.y + 3.0f, pSig.z);
		cajaPersonaje.xMin = cajaPersonaje.pos.x - cajaPersonaje.tamaño.x*0.5f;
		cajaPersonaje.xMax = cajaPersonaje.pos.x + cajaPersonaje.tamaño.x*0.5f;
		cajaPersonaje.yMin = cajaPersonaje.pos.y - cajaPersonaje.tamaño.y*0.5f;
		cajaPersonaje.yMax = cajaPersonaje.pos.y + cajaPersonaje.tamaño.y*0.5f;
		cajaPersonaje.zMin = cajaPersonaje.pos.z - cajaPersonaje.tamaño.z*0.5f;
		cajaPersonaje.zMax = cajaPersonaje.pos.z + cajaPersonaje.tamaño.z*0.5f;

		//Inf-izq-front
		A.x = cajaPersonaje.xMin;
		A.y = cajaPersonaje.yMin;
		A.z = cajaPersonaje.zMax;

		//Inf-der-front
		B.x = cajaPersonaje.xMax;
		B.y = cajaPersonaje.yMin;
		B.z = cajaPersonaje.zMax;

		//Sup-der-front
		C.x = cajaPersonaje.xMax;
		C.y = cajaPersonaje.yMax;
		C.z = cajaPersonaje.zMax;

		//Sup-izq-front
		D.x = cajaPersonaje.xMin;
		D.y = cajaPersonaje.yMax;
		D.z = cajaPersonaje.zMax;

		//Inf-izq-tras
		E.x = cajaPersonaje.xMin;
		E.y = cajaPersonaje.yMin;
		E.z = cajaPersonaje.zMin;

		//Inf-der-tras
		F.x = cajaPersonaje.xMax;
		F.y = cajaPersonaje.yMin;
		F.z = cajaPersonaje.zMin;

		//Sup-der-tras
		G.x = cajaPersonaje.xMax;
		G.y = cajaPersonaje.yMax;
		G.z = cajaPersonaje.zMin;

		//Sup-izq-tras
		H.x = cajaPersonaje.xMin;
		H.y = cajaPersonaje.yMax;
		H.z = cajaPersonaje.zMin;

		if ((A.x > cajaEscenario[i].xMin && A.x < cajaEscenario[i].xMax && A.y > cajaEscenario[i].yMin && A.y < cajaEscenario[i].yMax && A.z > cajaEscenario[i].zMin && A.z < cajaEscenario[i].zMax)
			|| (B.x > cajaEscenario[i].xMin && B.x < cajaEscenario[i].xMax && B.y > cajaEscenario[i].yMin && B.y < cajaEscenario[i].yMax && B.z > cajaEscenario[i].zMin && B.z < cajaEscenario[i].zMax)
			|| (C.x > cajaEscenario[i].xMin && C.x < cajaEscenario[i].xMax && C.y > cajaEscenario[i].yMin && C.y < cajaEscenario[i].yMax && C.z > cajaEscenario[i].zMin && C.z < cajaEscenario[i].zMax)
			|| (D.x > cajaEscenario[i].xMin && D.x < cajaEscenario[i].xMax && D.y > cajaEscenario[i].yMin && D.y < cajaEscenario[i].yMax && D.z > cajaEscenario[i].zMin && D.z < cajaEscenario[i].zMax)
			|| (E.x > cajaEscenario[i].xMin && E.x < cajaEscenario[i].xMax && E.y > cajaEscenario[i].yMin && E.y < cajaEscenario[i].yMax && E.z > cajaEscenario[i].zMin && E.z < cajaEscenario[i].zMax)
			|| (F.x > cajaEscenario[i].xMin && F.x < cajaEscenario[i].xMax && F.y > cajaEscenario[i].yMin && F.y < cajaEscenario[i].yMax && F.z > cajaEscenario[i].zMin && F.z < cajaEscenario[i].zMax)
			|| (G.x > cajaEscenario[i].xMin && G.x < cajaEscenario[i].xMax && G.y > cajaEscenario[i].yMin && G.y < cajaEscenario[i].yMax && G.z > cajaEscenario[i].zMin && G.z < cajaEscenario[i].zMax)
			|| (H.x > cajaEscenario[i].xMin && H.x < cajaEscenario[i].xMax && H.y > cajaEscenario[i].yMin && H.y < cajaEscenario[i].yMax && H.z > cajaEscenario[i].zMin && H.z < cajaEscenario[i].zMax))
		{
			col = true;

			if (dir == 1) //x+
				player1.PosicionObj.x = cajaEscenario[i].xMin - cajaPersonaje.tamaño.x*0.5f;
			else if (dir == 2) //x-
				player1.PosicionObj.x = cajaEscenario[i].xMax + cajaPersonaje.tamaño.x*0.5f;
			else if (dir == 3) //z+
				player1.PosicionObj.z = cajaEscenario[i].zMin - cajaPersonaje.tamaño.z*0.5f;
			else if (dir == 4) //z-
				player1.PosicionObj.z = cajaEscenario[i].zMax + cajaPersonaje.tamaño.z*0.5f;
			else if (dir == 5) //y+
				player1.PosicionObj.y = cajaEscenario[i].yMin - cajaPersonaje.tamaño.y*0.5f - 3.0f;
			else if (dir == 6) //y-
				player1.PosicionObj.y = cajaEscenario[i].yMax;

			break;
		}
	}

	return col;
}

void actualizaMovPersonaje()
{
	static int tiempoColPersonaje = 0;
	static int tiempoColEnemigo = 0;

	actualizaCajaColPersonaje();

	if (esferaPersonaje.estado == 1)
	{
		if (tiempoColPersonaje < 20)
			tiempoColPersonaje++;
		else
		{
			tiempoColPersonaje = 0;
			esferaPersonaje.estado = 0;
		}
	}

	if (esferaEnemigo.estado == 1)
	{
		if (tiempoColEnemigo < 20)
			tiempoColEnemigo++;
		else
		{
			tiempoColEnemigo = 0;
			esferaEnemigo.estado = 0;
		}
	}
	if (player1.caminando == true)
	{
		if (player1.dirX == 1)
		{
			if (!colisionCajas(1))
				player1.PosicionObj.x += player1.VelocidadObj;
			else
				player1.caminando = false;
		}
		else if (player1.dirX == 2 )
		{
			if (!colisionCajas(2))
				player1.PosicionObj.x -= player1.VelocidadObj;
			else
				player1.caminando = false;
		}

		if (player1.dirZ == 1)
		{
			if (!colisionCajas(3))
				player1.PosicionObj.z += player1.VelocidadObj;
			else
				player1.caminando = false;
		}
		else if (player1.dirZ == 2)
		{
			if (!colisionCajas(4))
				player1.PosicionObj.z -= player1.VelocidadObj;
			else
				player1.caminando = false;
		}

		//////
		
	}		
}

void actualizaMovPersonaje2()
{
	static int tiempoColPersonaje = 0;
	static int tiempoColEnemigo = 0;

	actualizaCajaColPersonaje();

	if (esferaPersonaje.estado == 1)
	{
		if (tiempoColPersonaje < 20)
			tiempoColPersonaje++;
		else
		{
			tiempoColPersonaje = 0;
			esferaPersonaje.estado = 0;
		}
	}

	if (esferaEnemigo.estado == 1)
	{
		if (tiempoColEnemigo < 20)
			tiempoColEnemigo++;
		else
		{
			tiempoColEnemigo = 0;
			esferaEnemigo.estado = 0;
		}
	}
	if (player1.caminando == true)
	{
		if (player1.dirX == 1)
		{
			if (!colisionBomba(1))
				player1.PosicionObj.x += player1.VelocidadObj;
			else
				player1.caminando = false;
		}
		else if (player1.dirX == 2)
		{
			if (!colisionBomba(2))
				player1.PosicionObj.x -= player1.VelocidadObj;
			else
				player1.caminando = false;
		}

		if (player1.dirZ == 1)
		{
			if (!colisionBomba(3))
				player1.PosicionObj.z += player1.VelocidadObj;
			else
				player1.caminando = false;
		}
		else if (player1.dirZ == 2)
		{
			if (!colisionBomba(4))
				player1.PosicionObj.z -= player1.VelocidadObj;
			else
				player1.caminando = false;
		}

		//////

	}
}


int IniGL(GLvoid)										// Aqui se configuran los parametros iniciales de OpenGL
{
	glShadeModel(GL_SMOOTH);							// Activa Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// Fondo negro
	glClearDepth(1.0f);									// Valor para el Depth Buffer
	glEnable(GL_DEPTH_TEST);							// Activa Depth Testing
	glDepthFunc(GL_LEQUAL);								// Tipo de Depth Testing a usar
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Correccion de cálculos de perspectiva

	glCullFace(GL_BACK);								// Configurado para eliminar caras traseras
	glEnable(GL_CULL_FACE);								// Activa eliminacion de caras ocultas

	glLightfv(GL_LIGHT0, GL_POSITION, LightPos);		// Posicion de la luz0
	glLightfv(GL_LIGHT0, GL_AMBIENT,  LightAmb);		// Componente ambiente
	glLightfv(GL_LIGHT0, GL_DIFFUSE,  LightDif);		// Componente difusa
	glLightfv(GL_LIGHT0, GL_SPECULAR, LightSpc);		// Componente especular
	
	glEnable(GL_LIGHT0);								// Activa luz0

	glLightfv(GL_LIGHT1, GL_POSITION, LightPos);		// Posicion de la luz0
	glLightfv(GL_LIGHT1, GL_AMBIENT,  LightAmb2);		// Componente ambiente
	glLightfv(GL_LIGHT1, GL_DIFFUSE,  LightDif2);		// Componente difusa
	glLightfv(GL_LIGHT1, GL_SPECULAR, LightSpc2);		// Componente especular

	glEnable(GL_LIGHTING);								// Habilita la iluminación

	e=gluNewQuadric();

	CargaModelos();
	iniciaCajasdeColision(); //Llamada a caja de colision
	iniCajaBomba();
	iniCajaParticula();
	IniVarsPlayer();
	InicializaParametrosdeControl();
	InicializaCamara();
	
	CargaTexturas();
	InicializaAnim();
	DatosAnimacion();

	//Cambios para Fuentes
	Font.BuildFont();
	controlFunc.inicializaControl();

	infGame.estadoJuego=1;

	posCamV=CVector(0.0f, 30.0f, 0.0f);
	dirCamV=CVector(0.0f, 0.0f, 1.0f);

	infGame.opcionMenuSelec=0;

	infGame.opcionMenuPausa=0;
	infGame.pausa=0;
	
	infGame.opMenuPrinc.dificultad=1;
	infGame.opMenuPrinc.numVidas=5;
	infGame.opMenuPrinc.sonido=0;

	cargaConfiguracion();

	sonidoF[0].cargaSonido("Audio/sbtitle.mp3", 1);
	sonidoF[1].cargaSonido("Audio/sbnivel.mp3", 1);


	timerEstados.iniciaTimer();
	InicializaParticulas();

	return TRUE;										
}

void controlEstados()
{
	if (infGame.estadoJuego == 1) //Logo estudio
	{
		
		if (timerEstados.TiempoActual() > 200)
		{
			infGame.tiempoEstado = 0;
			infGame.estadoJuego = 5;
			sonidoF[1].pausaSonido(true);
			sonidoF[0].pausaSonido(false);
			sonidoF[0].volumenSonido(0.5f);
		}
	}
	else if (infGame.estadoJuego == 2) //Pantalla de carga
	{
		if (timerEstados.TiempoActual() > 6000)
			infGame.cargaArchivosCompleta = 1;

		if (infGame.cargaArchivosCompleta == 1)
		{
			infGame.tiempoEstado = 0;
			infGame.estadoJuego = 3;
		}
	}
	else if (infGame.estadoJuego == 3) //introduccion del juego
	{
		if (timerEstados.TiempoActual() > 28000)
		{
			infGame.tiempoEstado = 0;
			infGame.estadoJuego = 4;
		}
	}
	else if (infGame.estadoJuego == 4) //Título del juego
	{
		if (timerEstados.TiempoActual() > 300)
		{
			infGame.tiempoEstado = 0;
			timerEstados.detieneTimer();
		}
	}
}

void animacion(FRAME *KeyFrame, int maxKF , int frames)
{
	if(play)
	{		
		if((abs(KeyFrame[playIndex+1].Angt1-Angt1))<0.1 &&
		   (abs(KeyFrame[playIndex+1].Angt2-Angt2))<0.1 &&
		   (abs(KeyFrame[playIndex+1].Angc1-Angc1))<0.1 &&
		   (abs(KeyFrame[playIndex+1].Angc2-Angc2))<0.1 &&
		   (abs(KeyFrame[playIndex+1].Angbi1-Angbi1))<0.1 &&
		   (abs(KeyFrame[playIndex+1].Angbi2-Angbi2))<0.1 &&
		   (abs(KeyFrame[playIndex+1].Angbib-Angbib))<0.1 &&
		   (abs(KeyFrame[playIndex+1].Angbd1-Angbd1))<0.1 &&
		   (abs(KeyFrame[playIndex+1].Angbd2-Angbd2))<0.1 &&
		   (abs(KeyFrame[playIndex+1].Angbdb-Angbdb))<0.1 &&
		   (abs(KeyFrame[playIndex+1].Angpizq-Angpizq))<0.1 &&
		   (abs(KeyFrame[playIndex+1].Angpder-Angpder))<0.1 &&
		   (abs(KeyFrame[playIndex+1].Angpi-Angpi))<0.1 &&
		   (abs(KeyFrame[playIndex+1].Angpd-Angpd))<0.1 &&
		   (abs(KeyFrame[playIndex+1].Xtor-Xtor))<0.1 &&
		   (abs(KeyFrame[playIndex+1].Ytor-Ytor))<0.1 &&
		   (abs(KeyFrame[playIndex+1].Ztor-Ztor))<0.1)
		{			
			playIndex++;			
			if(playIndex>maxKF-2)
			{
				playIndex=0;
				play=false;
								
			}
		}
		else
		{
			KeyFrame[playIndex].incAngt1   = (KeyFrame[playIndex+1].Angt1-KeyFrame[playIndex].Angt1)/frames;
			KeyFrame[playIndex].incAngt2   = (KeyFrame[playIndex+1].Angt2-KeyFrame[playIndex].Angt2)/frames;
			KeyFrame[playIndex].incAngc1   = (KeyFrame[playIndex+1].Angc1-KeyFrame[playIndex].Angc1)/frames;
			KeyFrame[playIndex].incAngc2   = (KeyFrame[playIndex+1].Angc2-KeyFrame[playIndex].Angc2)/frames;
			KeyFrame[playIndex].incAngbi1  = (KeyFrame[playIndex+1].Angbi1-KeyFrame[playIndex].Angbi1)/frames;
			KeyFrame[playIndex].incAngbi2  = (KeyFrame[playIndex+1].Angbi2-KeyFrame[playIndex].Angbi2)/frames;
			KeyFrame[playIndex].incAngbib  = (KeyFrame[playIndex+1].Angbib-KeyFrame[playIndex].Angbib)/frames;
			KeyFrame[playIndex].incAngbd1  = (KeyFrame[playIndex+1].Angbd1-KeyFrame[playIndex].Angbd1)/frames;
			KeyFrame[playIndex].incAngbd2  = (KeyFrame[playIndex+1].Angbd2-KeyFrame[playIndex].Angbd2)/frames;
			KeyFrame[playIndex].incAngbdb  = (KeyFrame[playIndex+1].Angbdb-KeyFrame[playIndex].Angbdb)/frames;
			KeyFrame[playIndex].incAngpizq = (KeyFrame[playIndex+1].Angpizq-KeyFrame[playIndex].Angpizq)/frames;
			KeyFrame[playIndex].incAngpder = (KeyFrame[playIndex+1].Angpder-KeyFrame[playIndex].Angpder)/frames;
			KeyFrame[playIndex].incAngpi   = (KeyFrame[playIndex+1].Angpi-KeyFrame[playIndex].Angpi)/frames;
			KeyFrame[playIndex].incAngpd   = (KeyFrame[playIndex+1].Angpd-KeyFrame[playIndex].Angpd)/frames;
			KeyFrame[playIndex].incXtor    = (KeyFrame[playIndex+1].Xtor-KeyFrame[playIndex].Xtor)/frames;
			KeyFrame[playIndex].incYtor    = (KeyFrame[playIndex+1].Ytor-KeyFrame[playIndex].Ytor)/frames;
			KeyFrame[playIndex].incZtor    = (KeyFrame[playIndex+1].Ztor-KeyFrame[playIndex].Ztor)/frames;
			
			Angt1   += KeyFrame[playIndex].incAngt1;
			Angt2   += KeyFrame[playIndex].incAngt2;
			Angc1   += KeyFrame[playIndex].incAngc1;
			Angc2   += KeyFrame[playIndex].incAngc2;
			Angbi1  += KeyFrame[playIndex].incAngbi1;
			Angbi2  += KeyFrame[playIndex].incAngbi2;
			Angbib  += KeyFrame[playIndex].incAngbib;
			Angbd1  += KeyFrame[playIndex].incAngbd1;
			Angbd2  += KeyFrame[playIndex].incAngbd2;
			Angbdb  += KeyFrame[playIndex].incAngbdb;
			Angpizq += KeyFrame[playIndex].incAngpizq;
			Angpder += KeyFrame[playIndex].incAngpder;
			Angpi   += KeyFrame[playIndex].incAngpi;
			Angpd   += KeyFrame[playIndex].incAngpd;
			Xtor    += KeyFrame[playIndex].incXtor;
			Ytor    += KeyFrame[playIndex].incYtor;
			Ztor    += KeyFrame[playIndex].incZtor;
			
		}
	}
}

void ActualizaCamara()
{
	float objAlt=camara1.ObjetivoCam.y;
	float aux=(player1.PosicionObj.x-camara1.VariacionDespDist)*camara1.FactorCam+camara1.VariacionDespCam;

	//camara1.PosicionCam.x=150.0f*aux;
	camara1.PosicionCam.x=player1.PosicionObj.x-30.0f*aux;
	camara1.PosicionCam.y=12.0f;
	camara1.PosicionCam.z=40.0f;

	camara1.ObjetivoCam.x=player1.PosicionObj.x;
	camara1.ObjetivoCam.z=player1.PosicionObj.z;
	camara1.ObjetivoCam.y=objAlt;
}

void ControlPersonaje(int funcion)
{
	if(funcion == 1) //Avanza a la derecha (x+)
	{
		player1.PosicionObj.x+=player1.VelocidadObj;
		player1.AngObj=90.0f;
	}
	else if(funcion == 2) //Avanza a la izquierda (x-)
	{
		player1.PosicionObj.x-=player1.VelocidadObj;
		player1.AngObj=-90.0f;
	}
	else if(funcion == 3) //Avanza hacia adentro (z-)
	{
		player1.PosicionObj.z-=player1.VelocidadObj;
		player1.AngObj=180.0f;
	}
	else if(funcion == 4) //Avanza hacia afuera (z+)
	{
		player1.PosicionObj.z+=player1.VelocidadObj;
		player1.AngObj=0.0f;
	}
	actualizaCajaColPersonaje();
}

void actualizaCajaColPersonaje()
{
	cajaPersonaje.pos = CVector(player1.PosicionObj.x, player1.PosicionObj.y + 3.5, player1.PosicionObj.z);
	cajaPersonaje.xMin = cajaPersonaje.pos.x - cajaPersonaje.tamaño.x*0.5f;
	cajaPersonaje.xMax = cajaPersonaje.pos.x + cajaPersonaje.tamaño.x*0.5f;
	cajaPersonaje.yMin = cajaPersonaje.pos.y - cajaPersonaje.tamaño.y*0.5f;
	cajaPersonaje.yMax = cajaPersonaje.pos.y + cajaPersonaje.tamaño.y*0.5f;
	cajaPersonaje.zMin = cajaPersonaje.pos.z - cajaPersonaje.tamaño.z*0.5f;
	cajaPersonaje.zMax = cajaPersonaje.pos.z + cajaPersonaje.tamaño.z*0.5f;
}


void DibujaCaja(int tipo)
{
	glEnable(GL_TEXTURE_2D);
	int num;
	//cara frontal
	if (tipo == 1)
		num = 18; //Selección de la textura bloques
	else if (tipo == 2)
		num = 19; //Selección de la textura bloques cofre
	else if (tipo == 3)
		num = 16; //Bloques del límite del juego lados
	else if (tipo == 4)
		num = 17; //Bloques limite de juego abajo

		glBindTexture(GL_TEXTURE_2D, textura[num].texID);
	glBegin(GL_QUADS);
		glNormal3f(0.0f,0.0f,1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-2.5f, -2.0f, 3.5f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f( 2.5f, -2.0f, 3.5f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f( 2.5f,  2.0f, 3.5f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-2.5f,  2.0f, 3.5f);
	glEnd();


	//cara sup
	glBindTexture(GL_TEXTURE_2D, textura[num].texID);

	glBegin(GL_QUADS);
		glNormal3f(0.0f,1.0f,0.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-2.5f, 2.0f, -3.5f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-2.5f, 2.0f,  3.5f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f( 2.5f, 2.0f,  3.5f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f( 2.5f, 2.0f, -3.5f);
	glEnd();



	glDisable(GL_TEXTURE_2D);
}


void DibujaEscenarioBase() {

		glEnable(GL_TEXTURE_2D);

		//Plano del piso
		glBindTexture(GL_TEXTURE_2D, textura[4].texID);
		glBegin(GL_QUADS);
		glNormal3f(0.0f, 1.0f, 0.0f);
		glTexCoord2f(0.1f, 0.0f); glVertex3f(-42.5f, 0.0f, 78.0f);
		glTexCoord2f(5.0f, 0.0f); glVertex3f(42.5f, 0.0f, 78.0f);
		glTexCoord2f(5.0f, 5.0f); glVertex3f(42.5f, 0.0f, 2.0f);
		glTexCoord2f(0.1f, 5.0f); glVertex3f(-42.5f, 0.0f, 2.0f);
		glEnd();

		//Cajas limite escenario izquierda
		glPushMatrix();
		glTranslatef(-45.5f, 2.5f, 5.0f);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(3);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(-45.5f, 2.5f, 10.0f);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(3);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(-45.5f, 2.5f, 15.0f);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(3);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(-45.5f, 2.5f, 20.0f);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(3);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(-45.5f, 2.5f, 25.0f);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(3);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(-45.5f, 2.5f, 25.0f);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(3);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(-45.5f, 2.5f, 30.0f);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(3);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(-45.5f, 2.5f, 35.0f);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(3);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(-45.5f, 2.5f, 40.0f);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(3);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(-45.5f, 2.5f, 45.0f);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(3);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(-45.5f, 2.5f, 50.0f);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(3);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(-45.5f, 2.5f, 55.0f);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(3);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(-45.5f, 2.5f, 60.0f);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(3);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(-45.5f, 2.5f, 65.0f);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(3);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(-45.5f, 2.5f, 70.0f);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(3);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(-45.5f, 2.5f, 75.0f);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(3);
		glPopMatrix();

		//Cajas limite escenario derecha
		glPushMatrix();
		glTranslatef(45.5f, 2.5f, 5.0f);
		glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(3);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(45.5f, 2.5f, 10.0f);
		glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(3);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(45.5f, 2.5f, 15.0f);
		glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(3);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(45.5f, 2.5f, 20.0f);
		glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(3);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(45.5f, 2.5f, 25.0f);
		glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(3);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(45.5f, 2.5f, 25.0f);
		glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(3);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(45.5f, 2.5f, 30.0f);
		glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(3);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(45.5f, 2.5f, 35.0f);
		glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(3);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(45.5f, 2.5f, 40.0f);
		glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(3);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(45.5f, 2.5f, 45.0f);
		glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(3);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(45.5f, 2.5f, 50.0f);
		glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(3);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(45.5f, 2.5f, 55.0f);
		glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(3);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(45.5f, 2.5f, 60.0f);
		glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(3);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(45.5f, 2.5f, 65.0f);
		glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(3);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(45.5f, 2.5f, 70.0f);
		glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(3);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(45.5f, 2.5f, 75.0f);
		glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(3);
		glPopMatrix();

		//Límite del escenario inferior

		glPushMatrix();
		glTranslatef(42.5f, 7.5f, 78.0f);
		glScalef(0.7, 0.0, 0.7);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(4);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(37.5f, 7.5f, 78.0f);
		glScalef(0.7, 0.0, 0.7);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(4);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(32.5f, 7.5f, 78.0f);
		glScalef(0.7, 0.0, 0.7);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(4);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(27.5f, 7.5f, 78.0f);
		glScalef(0.7, 0.0, 0.7);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(4);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(22.5f, 7.5f, 78.0f);
		glScalef(0.7, 0.0, 0.7);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(4);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(17.5f, 7.5f, 78.0f);
		glScalef(0.7, 0.0, 0.7);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(4);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(12.5f, 7.5f, 78.0f);
		glScalef(0.7, 0.0, 0.7);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(4);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(7.5f, 7.5f, 78.0f);
		glScalef(0.7, 0.0, 0.7);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(4);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(2.5f, 7.5f, 78.0f);
		glScalef(0.7, 0.0, 0.7);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(4);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(-2.5f, 7.5f, 78.0f);
		glScalef(0.7, 0.0, 0.7);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(4);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(-7.5f, 7.5f, 78.0f);
		glScalef(0.7, 0.0, 0.7);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(4);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(-12.5f, 7.5f, 78.0f);
		glScalef(0.7, 0.0, 0.7);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(4);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(-17.5f, 7.5f, 78.0f);
		glScalef(0.7, 0.0, 0.7);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(4);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(-22.5f, 7.5f, 78.0f);
		glScalef(0.7, 0.0, 0.7);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(4);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(-27.5f, 7.5f, 78.0f);
		glScalef(0.7, 0.0, 0.7);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(4);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(-32.5f, 7.5f, 78.0f);
		glScalef(0.7, 0.0, 0.7);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(4);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(-37.5f, 7.5f, 78.0f);
		glScalef(0.7, 0.0, 0.7);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(4);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(-42.5f, 7.5f, 78.0f);
		glScalef(0.7, 0.0, 0.7);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(4);
		glPopMatrix();

		// ------------------- CAJAS AMARILLAS -------------------//

		// CAJAS [1,X]
		glPushMatrix();
		glTranslatef(-32.0f, 2.5f, 13.0f);
		glScalef(0.7, 0.0, 1.0);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(1);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(-32.0f, 2.5f, 26.0f);
		glScalef(0.7, 0.0, 1.0);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(1);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(-32.0f, 2.5f, 39.0f);
		glScalef(0.7, 0.0, 1.0);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(1);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(-32.0f, 2.5f, 52.0f);
		glScalef(0.7, 0.0, 1.0);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(1);
		glPopMatrix();

		// CAJAS [2,X]
		glPushMatrix();
		glTranslatef(-18.0f, 2.5f, 13.0f);
		glScalef(0.7, 0.0, 1.0);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(1);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(-18.0f, 2.5f, 26.0f);
		glScalef(0.7, 0.0, 1.0);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(1);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(-18.0f, 2.5f, 39.0f);
		glScalef(0.7, 0.0, 1.0);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(1);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(-18.0f, 2.5f, 52.0f);
		glScalef(0.7, 0.0, 1.0);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(1);
		glPopMatrix();

		// CAJAS [3,X]
		glPushMatrix();
		glTranslatef(-5.0f, 2.5f, 13.0f);
		glScalef(0.7, 0.0, 1.0);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(1);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(-5.0f, 2.5f, 26.0f);
		glScalef(0.7, 0.0, 1.0);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(1);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(-5.0f, 2.5f, 39.0f);
		glScalef(0.7, 0.0, 1.0);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(1);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(-5.0f, 2.5f, 52.0f);
		glScalef(0.7, 0.0, 1.0);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(1);
		glPopMatrix();

		// CAJAS [4,X]
		glPushMatrix();
		glTranslatef(7.0f, 2.5f, 13.0f);
		glScalef(0.7, 0.0, 1.0);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(1);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(7.0f, 2.5f, 26.0f);
		glScalef(0.7, 0.0, 1.0);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(1);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(7.0f, 2.5f, 39.0f);
		glScalef(0.7, 0.0, 1.0);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(1);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(7.0f, 2.5f, 52.0f);
		glScalef(0.7, 0.0, 1.0);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(1);
		glPopMatrix();

		// CAJAS [5,X]
		glPushMatrix();
		glTranslatef(20.0f, 2.5f, 13.0f);
		glScalef(0.7, 0.0, 1.0);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(1);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(20.0f, 2.5f, 26.0f);
		glScalef(0.7, 0.0, 1.0);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(1);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(20.0f, 2.5f, 39.0f);
		glScalef(0.7, 0.0, 1.0);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(1);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(20.0f, 2.5f, 52.0f);
		glScalef(0.7, 0.0, 1.0);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(1);
		glPopMatrix();

		// CAJAS [6,X]
		glPushMatrix();
		glTranslatef(33.0f, 2.5f, 13.0f);
		glScalef(0.7, 0.0, 1.0);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(1);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(33.0f, 2.5f, 26.0f);
		glScalef(0.7, 0.0, 1.0);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(1);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(33.0f, 2.5f, 39.0f);
		glScalef(0.7, 0.0, 1.0);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(1);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(33.0f, 2.5f, 52.0f);
		glScalef(0.7, 0.0, 1.0);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(1);
		glPopMatrix();

		// CAJAS [X , 5]

		glPushMatrix();
		glTranslatef(-32.0f, 2.5f, 65.0f);
		glScalef(0.7, 0.0, 1.0);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(1);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(-18.0f, 2.5f, 65.0f);
		glScalef(0.7, 0.0, 1.0);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(1);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(-5.0f, 2.5f, 65.0f);
		glScalef(0.7, 0.0, 1.0);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(1);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(7.0f, 2.5f, 65.0f);
		glScalef(0.7, 0.0, 1.0);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(1);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(20.0f, 2.5f, 65.0f);
		glScalef(0.7, 0.0, 1.0);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(1);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(33.0f, 2.5f, 65.0f);
		glScalef(0.7, 0.0, 1.0);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(1);
		glPopMatrix();

		// CAJAS EXTRAS

		glPushMatrix();
		glTranslatef(33.0f, 2.5f, 32.5f);
		glScalef(0.7, 0.0, 1.0);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(1);
		glPopMatrix();

		//Caja [5.5 , 3]

		glPushMatrix();
		glTranslatef(26.5f, 2.5f, 39.0f);
		glScalef(0.7, 0.0, 1.0);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(1);
		glPopMatrix();

		//Caja [5 , 3.5]

		glPushMatrix();
		glTranslatef(20.0f, 2.5f, 45.5f);
		glScalef(0.7, 0.0, 1.0);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(1);
		glPopMatrix();

		//Caja [5.5 , 3]

		glPushMatrix();
		glTranslatef(26.5f, 2.5f, 52.5f);
		glScalef(0.7, 0.0, 1.0);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(1);
		glPopMatrix();

		//Caja [3, 4.5]

		glPushMatrix();
		glTranslatef(-5.0f, 2.5f, 58.5f);
		glScalef(0.7, 0.0, 1.0);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(1);
		glPopMatrix();

		//Caja [6 , 4.5]

		glPushMatrix();
		glTranslatef(33.0f, 2.5f, 58.5f);
		glScalef(0.7, 0.0, 1.0);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(1);
		glPopMatrix();


		// CAJAS ROJAS

		glPushMatrix();		//1
		glTranslatef(-26.0f, 2.0f, 7.0f);
		glScalef(1.0, 1.0, 1.0);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(2);
		glPopMatrix();

		glPushMatrix();		//2
		glTranslatef(1.0f, 2.0f, 7.0f);
		glScalef(1.0, 1.0, 1.0);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(2);
		glPopMatrix();

		glPushMatrix();		//3
		glTranslatef(-26.0f, 2.0f, 13.5f);
		glScalef(1.0, 1.0, 1.0);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(2);
		glPopMatrix();

		glPushMatrix();		//4
		glTranslatef(-11.5f, 2.0f, 13.5f);
		glScalef(1.0, 1.0, 1.0);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(2);
		glPopMatrix();

		glPushMatrix();		//5
		glTranslatef(13.5f, 2.0f, 13.5f);
		glScalef(1.0, 1.0, 1.0);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(2);
		glPopMatrix();

		glPushMatrix();		//6
		glTranslatef(26.5f, 2.0f, 20.0f);
		glScalef(1.0, 1.0, 1.0);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(2);
		glPopMatrix();

		glPushMatrix();		//7
		glTranslatef(33.0f, 2.0f, 20.0f);
		glScalef(1.0, 1.0, 1.0);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(2);
		glPopMatrix();

		glPushMatrix();		//8
		glTranslatef(1.0f, 2.0f, 26.0f);
		glScalef(1.0, 1.0, 1.0);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(2);
		glPopMatrix();

		glPushMatrix();		//9
		glTranslatef(-38.5f, 2.0f, 32.5f);
		glScalef(1.0, 1.0, 1.0);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(2);
		glPopMatrix();

		glPushMatrix();		//10
		glTranslatef(-24.5f, 2.0f, 32.5f);
		glScalef(1.0, 1.0, 1.0);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(2);
		glPopMatrix();

		glPushMatrix();		//11
		glTranslatef(-18.0f, 2.0f, 32.5f);
		glScalef(1.0, 1.0, 1.0);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(2);
		glPopMatrix();

		glPushMatrix();		//12
		glTranslatef(7.0f, 2.0f, 32.5f);
		glScalef(1.0, 1.0, 1.0);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(2);
		glPopMatrix();

		glPushMatrix();		//13
		glTranslatef(13.5f, 2.0f, 32.5f);
		glScalef(1.0, 1.0, 1.0);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(2);
		glPopMatrix();

		glPushMatrix();		//14
		glTranslatef(20.0f, 2.0f, 32.5f);
		glScalef(1.0, 1.0, 1.0);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(2);
		glPopMatrix();

		glPushMatrix();		//15
		glTranslatef(26.5f, 2.0f, 32.5f);
		glScalef(1.0, 1.0, 1.0);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(2);
		glPopMatrix();

		glPushMatrix();		//16
		glTranslatef(-11.5f, 2.0f, 39.0f);
		glScalef(1.0, 1.0, 1.0);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(2);
		glPopMatrix();

		glPushMatrix();		//17
		glTranslatef(13.5f, 2.0f, 39.0f);
		glScalef(1.0, 1.0, 1.0);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(2);
		glPopMatrix();

		glPushMatrix();		//18
		glTranslatef(39.0f, 2.0f, 39.0f);
		glScalef(1.0, 1.0, 1.0);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(2);
		glPopMatrix();

		glPushMatrix();		//19
		glTranslatef(-11.5f, 2.0f, 45.5f);
		glScalef(1.0, 1.0, 1.0);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(2);
		glPopMatrix();

		glPushMatrix();		//20
		glTranslatef(0.5f, 2.0f, 45.5f);
		glScalef(1.0, 1.0, 1.0);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(2);
		glPopMatrix();

		glPushMatrix();		//21
		glTranslatef(-11.5f, 2.0f, 52.0f);
		glScalef(1.0, 1.0, 1.0);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(2);
		glPopMatrix();

		glPushMatrix();		//22
		glTranslatef(-32.0f, 2.0f, 58.5f);
		glScalef(1.0, 1.0, 1.0);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(2);
		glPopMatrix();

		glPushMatrix();		//23
		glTranslatef(-11.5f, 2.0f, 58.5f);
		glScalef(1.0, 1.0, 1.0);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(2);
		glPopMatrix();

		glPushMatrix();		//24
		glTranslatef(7.0f, 2.0f, 58.5f);
		glScalef(1.0, 1.0, 1.0);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(2);
		glPopMatrix();

		glPushMatrix();		//25
		glTranslatef(13.5f, 2.0f, 58.5f);
		glScalef(1.0, 1.0, 1.0);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(2);
		glPopMatrix();

		glPushMatrix();		//26
		glTranslatef(26.5f, 2.0f, 58.5f);
		glScalef(1.0, 1.0, 1.0);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(2);
		glPopMatrix();

		glPushMatrix();		//27
		glTranslatef(-38.5f, 2.0f, 65.0f);
		glScalef(1.0, 1.0, 1.0);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(2);
		glPopMatrix();

		glPushMatrix();		//28
		glTranslatef(-38.5f, 2.0f, 71.5f);
		glScalef(1.0, 1.0, 1.0);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(2);
		glPopMatrix();

		glPushMatrix();		//29
		glTranslatef(-24.5f, 2.0f, 71.5f);
		glScalef(1.0, 1.0, 1.0);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(2);
		glPopMatrix();

		glPushMatrix();		//30
		glTranslatef(-18.0f, 2.0f, 71.5f);
		glScalef(1.0, 1.0, 1.0);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(2);
		glPopMatrix();

		glPushMatrix();		//31
		glTranslatef(0.5f, 2.0f, 71.5f);
		glScalef(1.0, 1.0, 1.0);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(2);
		glPopMatrix();

		glPushMatrix();		//32
		glTranslatef(13.5f, 2.0f, 71.5f);
		glScalef(1.0, 1.0, 1.0);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(2);
		glPopMatrix();

		glPushMatrix();		//33
		glTranslatef(33.0f, 2.0f, 71.5f);
		glScalef(1.0, 1.0, 1.0);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(2);
		glPopMatrix();

		glPushMatrix();		//34
		glTranslatef(39.0f, 2.0f, 65.0f);
		glScalef(1.0, 1.0, 1.0);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		DibujaCaja(2);
		glPopMatrix();

}

void DibujaPersonaje()
{
	glPushMatrix();

	//glTranslatef(-5.0f, Ytor + 1.3f, -57.0);

	glPushMatrix();

		glTranslatef(Xtor, Ytor+1.3f, Ztor);
		glRotatef(Angt2, 0.0f, 1.0f, 0.0f);
		glRotatef(Angt1, 1.0f, 0.0f, 0.0f);
		
		//Torso
		g_Load3ds.Render3DSFile(&g_3DModel1c, textureModel1c, 1);

		//Cabeza
		glPushMatrix();
			glTranslatef(0.0f, 1.9f ,0.0f);
			glRotatef(Angc2, 0.0f, 1.0f, 0.0f);
			glRotatef(Angc1, 1.0f, 0.0f, 0.0f);
			g_Load3ds.Render3DSFile(&g_3DModel2c, textureModel1c, 1);
		glPopMatrix();

		//Pierna derecha
		glPushMatrix();
			glTranslatef(-0.6f, 0.1f ,0.1f);
			glRotatef(Angpder, 1.0f, 0.0f, 0.0f);
			g_Load3ds.Render3DSFile(&g_3DModel3c, textureModel3c, 1);

			//Pie derecho
			glPushMatrix();
				glTranslatef(0.07f, -0.85f ,0.0f);
				glRotatef(Angpd, 1.0f, 0.0f, 0.0f);
				g_Load3ds.Render3DSFile(&g_3DModel9c, textureModel9c, 1);
			glPopMatrix();

		glPopMatrix();

		//Pierna izquierda
		glPushMatrix();
			glTranslatef(0.6f, 0.1f ,0.1f);
			glRotatef(Angpizq, 1.0f, 0.0f, 0.0f);
			g_Load3ds.Render3DSFile(&g_3DModel4c, textureModel4c, 1);

			//Pie izquierdo
			glPushMatrix();
				glTranslatef(-0.07f, -0.85f ,0.0f);
				glRotatef(Angpi, 1.0f, 0.0f, 0.0f);
				g_Load3ds.Render3DSFile(&g_3DModel9c, textureModel9c, 1);
			glPopMatrix();

		glPopMatrix();

		//Brazo derecho
		glPushMatrix();
			glTranslatef(-0.9f, 1.45f, -0.1f);
			glRotatef(Angbd2, 0.0f, 0.0f, 1.0f);
			glRotatef(Angbd1, 1.0f, 0.0f, 0.0f);
			g_Load3ds.Render3DSFile(&g_3DModel5c, textureModel5c, 1);

		glPopMatrix();

		//Brazo izquierdo
		glPushMatrix();
			glTranslatef(0.9f, 1.45f, -0.1f);
			glRotatef(Angbi2, 0.0f, 0.0f, 1.0f);
			glRotatef(Angbi1, 1.0f, 0.0f, 0.0f);
			g_Load3ds.Render3DSFile(&g_3DModel7c, textureModel7c, 1);

		glPopMatrix();

	glPopMatrix();

	glPopMatrix();
}

void ActualizaLuzParam()
{
	glLightfv(GL_LIGHT0, GL_POSITION, LightPos);		// Posicion de la luz0
	glLightfv(GL_LIGHT0, GL_AMBIENT,  LightAmb);		// Componente ambiente
	glLightfv(GL_LIGHT0, GL_DIFFUSE,  LightDif);		// Componente difusa
	glLightfv(GL_LIGHT0, GL_SPECULAR, LightSpc);		// Componente especular
}

void AlturaPiso()
{
	if(player1.PosicionObj.x >= -90.0f && player1.PosicionObj.x < -60.0f &&
	   player1.PosicionObj.z >=   0.0f && player1.PosicionObj.z <  20.0f)
		altPiso=0.0f;
	else if(player1.PosicionObj.x >= -60.0f && player1.PosicionObj.x < -45.0f &&
	   player1.PosicionObj.z >=   0.0f && player1.PosicionObj.z <  10.0f)
		altPiso=5.0f;
	else if(player1.PosicionObj.x >= -45.0f && player1.PosicionObj.x < 0.0f &&
	   player1.PosicionObj.z >=   0.0f && player1.PosicionObj.z <  22.0f)
		altPiso=8.0f;
	else if(player1.PosicionObj.x >= 0.0f && player1.PosicionObj.x < 80.0f &&
	   player1.PosicionObj.z >=   0.0f && player1.PosicionObj.z <  30.0f)
		altPiso=17.0f;
	else
		altPiso=-50.0f;
}

//Cambios para Fuentes
void DibujaTextos()
{
	glDisable(GL_DEPTH_TEST);							// Desactiva la prueba de profundidad
	glMatrixMode(GL_PROJECTION);						// Selecciona la matriz de proyección
	glPushMatrix();										// Guarda la matriz de proyección
	glLoadIdentity();									// Limpia la matriz de proyección
	glOrtho(0,infGame.glWidth,0,infGame.glHeight,-1,1);					// Crea una proyección ortogonal
	glMatrixMode(GL_MODELVIEW);							// Selecciona la matriz de modelo de vista
	glPushMatrix();										// Guarda la matriz de modelo de vista
	
	glDisable(GL_LIGHTING);
	glLoadIdentity();

	glColor3f(1.0f,1.0f,1.0f);
	
	glEnable(GL_LIGHTING);
	glMatrixMode(GL_PROJECTION);						// Selecciona la matriz de proyección
	glPopMatrix();										// Recupera la anterior matriz de proyección
	glMatrixMode(GL_MODELVIEW);							// Selecciona la matriz de modelo de vista
	glPopMatrix();										// Recupera la anterior matriz de modelo de vista
	glEnable(GL_DEPTH_TEST);							// Activa la prueba de profundidad
}

void dibujaTituloJuego()
{
	static float color=0.0f;

	glDisable(GL_DEPTH_TEST);							// Desactiva prueba de profundidad
	glMatrixMode(GL_PROJECTION);						// Selecciona la matriz de proyeccion
	glPushMatrix();										// Guarda la matriz de proyeccion
	glLoadIdentity();									// Limpia la matriz de proyeccion
	glOrtho(0,infGame.glWidth,0,infGame.glHeight,-1,1);					// Crea una proyección paralela
	glMatrixMode(GL_MODELVIEW);							// Selecciona la matriz de modelo de vista
	glPushMatrix();										// Guarda matriz de modelo de vista
	glLoadIdentity();

	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);

	glColor3f(color, color, color);

	glBindTexture(GL_TEXTURE_2D, textura[30].texID);

	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex2f(infGame.glWidth*0.0f, infGame.glHeight*0.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex2f(infGame.glWidth*1.0f, infGame.glHeight*0.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex2f(infGame.glWidth*1.0f, infGame.glHeight*1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex2f(infGame.glWidth*0.0f, infGame.glHeight*1.0f);
	glEnd();

	glColor3f(1.0f, 1.0f, 1.0f);

	if(color < 1.0f)
		color+=0.01f;
	
	if(infGame.estadoJuego == 5 && color >= 0.9f)
	{
		glAlphaFunc(GL_GREATER, 0.6f);
		glEnable(GL_ALPHA_TEST);

		if(infGame.opcionMenuSelec == 0)
			glColor3f(1.0f, 0.11f, 0.0f);
		else
			glColor3f(0.0f, 0.0f, 0.0f);
		Font.glPrint((2.2f/640.0f)*infGame.glWidth, infGame.glWidth*0.40f, infGame.glHeight*0.17f,"Start");

		if(infGame.opcionMenuSelec == 1)
			glColor3f(1.0f, 0.11f, 0.0f);
		else
			glColor3f(0.0f, 0.0f, 0.0f);
		Font.glPrint((2.2f/640.0f)*infGame.glWidth, infGame.glWidth*0.33f, infGame.glHeight*0.09f,"Load game");

		if(infGame.opcionMenuSelec == 2)
			glColor3f(1.0f, 0.11f, 0.0f);
		else
			glColor3f(0.0f, 0.0f, 0.0f);
		Font.glPrint((2.2f/640.0f)*infGame.glWidth, infGame.glWidth*0.33f, infGame.glHeight*0.02f," Options");
		
		glColor3f(0.76f, 0.86f, 0.93f);
				
		glDisable(GL_ALPHA_TEST);
	}

	glDisable(GL_TEXTURE_2D);

	glEnable(GL_LIGHTING);
	
	glMatrixMode(GL_PROJECTION);						// Selecciona la matriz de proyeccion
	glPopMatrix();										// Restaura la matriz de proyeccion anterior
	glMatrixMode(GL_MODELVIEW);							// Selecciona la matriz de modelo de vista
	glPopMatrix();										// Restaura la matriz de modelo de vista anterior
	glEnable(GL_DEPTH_TEST);							// Activa prueba de profundidad
}

void dibujaMenuOpciones()
{
	glDisable(GL_DEPTH_TEST);							// Desactiva prueba de profundidad
	glMatrixMode(GL_PROJECTION);						// Selecciona la matriz de proyeccion
	glPushMatrix();										// Guarda la matriz de proyeccion
	glLoadIdentity();									// Limpia la matriz de proyeccion
	glOrtho(0,infGame.glWidth,0,infGame.glHeight,-1,1);					// Crea una proyección paralela
	glMatrixMode(GL_MODELVIEW);							// Selecciona la matriz de modelo de vista
	glPushMatrix();										// Guarda matriz de modelo de vista
	glLoadIdentity();

	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, textura[30].texID);

	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex2f(infGame.glWidth*0.0f, infGame.glHeight*0.0f);
		glTexCoord2f(0.5f, 0.0f); glVertex2f(infGame.glWidth*1.0f, infGame.glHeight*0.0f);
		glTexCoord2f(0.5f, 0.3f); glVertex2f(infGame.glWidth*1.0f, infGame.glHeight*1.0f);
		glTexCoord2f(0.0f, 0.3f); glVertex2f(infGame.glWidth*0.0f, infGame.glHeight*1.0f);
	glEnd();
	
	glAlphaFunc(GL_GREATER, 0.6f);
	glEnable(GL_ALPHA_TEST);

	if(infGame.opMenuPrinc.opcionSelec == 0)
		glColor3f(0.76f, 0.86f, 0.93f);
	else
		glColor3f(0.0f, 0.0f, 0.0f);
	Font.glPrint((2.2f/640.0f)*infGame.glWidth, infGame.glWidth*0.15f, infGame.glHeight*0.8f,"Lifes:");

	if(infGame.opMenuPrinc.opcionSelec == 1)
		glColor3f(0.76f, 0.86f, 0.93f);
	else
		glColor3f(0.0f, 0.0f, 0.0f);
	Font.glPrint((2.2f/640.0f)*infGame.glWidth, infGame.glWidth*0.15f, infGame.glHeight*0.6f,"Music:");

	if(infGame.opMenuPrinc.opcionSelec == 2)
		glColor3f(0.76f, 0.86f, 0.93f);
	else
		glColor3f(0.0f, 0.0f, 0.0f);
	Font.glPrint((2.2f/640.0f)*infGame.glWidth, infGame.glWidth*0.15f, infGame.glHeight*0.4f,"Main Menu");

	glColor3f(0.76f, 0.86f, 0.93f);
	
	//Número de vidas
	Font.glPrint((2.2f/640.0f)*infGame.glWidth, infGame.glWidth*0.6f, infGame.glHeight*0.8f,"%d", infGame.opMenuPrinc.numVidas);

	//Sonido
	if(infGame.opMenuPrinc.sonido == 0)
		Font.glPrint((2.2f/640.0f)*infGame.glWidth, infGame.glWidth*0.6f, infGame.glHeight*0.6f,"Off");
	else
		Font.glPrint((2.2f/640.0f)*infGame.glWidth, infGame.glWidth*0.6f, infGame.glHeight*0.6f,"On");
	
				
	glDisable(GL_ALPHA_TEST);

	glDisable(GL_TEXTURE_2D);

	glEnable(GL_LIGHTING);
	
	glMatrixMode(GL_PROJECTION);						// Selecciona la matriz de proyeccion
	glPopMatrix();										// Restaura la matriz de proyeccion anterior
	glMatrixMode(GL_MODELVIEW);							// Selecciona la matriz de modelo de vista
	glPopMatrix();										// Restaura la matriz de modelo de vista anterior
	glEnable(GL_DEPTH_TEST);							// Activa prueba de profundidad
}

void dibujaMenuPausa()
{
	glDisable(GL_DEPTH_TEST);							// Desactiva prueba de profundidad
	glMatrixMode(GL_PROJECTION);						// Selecciona la matriz de proyeccion
	glPushMatrix();										// Guarda la matriz de proyeccion
	glLoadIdentity();									// Limpia la matriz de proyeccion
	glOrtho(0,infGame.glWidth,0,infGame.glHeight,-1,1);					// Crea una proyección paralela
	glMatrixMode(GL_MODELVIEW);							// Selecciona la matriz de modelo de vista
	glPushMatrix();										// Guarda matriz de modelo de vista
	glLoadIdentity();

	glDisable(GL_LIGHTING);
	
	glColor3f(0.0f, 0.0f, 0.0f);

	glBegin(GL_QUADS);
		glVertex2f(infGame.glWidth*0.25f, infGame.glHeight*0.27f);
		glVertex2f(infGame.glWidth*0.75f, infGame.glHeight*0.27f);
		glVertex2f(infGame.glWidth*0.75f, infGame.glHeight*0.73f);
		glVertex2f(infGame.glWidth*0.25f, infGame.glHeight*0.73f);
	glEnd();
	
	glColor3f(1.0f, 1.0f, 1.0f);

	glEnable(GL_TEXTURE_2D);
	glAlphaFunc(GL_GREATER, 0.6f);
	glEnable(GL_ALPHA_TEST);

	if(infGame.opcionMenuPausa == 0)
		glColor3f(0.00f, 0.00f, 0.93f);
	else
		glColor3f(1.0f, 1.0f, 1.0f);
	Font.glPrint((2.2f/640.0f)*infGame.glWidth, infGame.glWidth*0.38f, infGame.glHeight*0.58f,"Options");

	if(infGame.opcionMenuPausa == 1)
		glColor3f(0.00f, 0.00f, 0.93f);
	else
		glColor3f(1.0f, 1.0f, 1.0f);
	Font.glPrint((2.2f/640.0f)*infGame.glWidth, infGame.glWidth*0.35f, infGame.glHeight*0.50f,"Main Menu");

	if(infGame.opcionMenuPausa == 2)
		glColor3f(0.00f, 0.00f, 0.93f);
	else
		glColor3f(1.0f, 1.0f, 1.0f);
	Font.glPrint((2.2f/640.0f)*infGame.glWidth, infGame.glWidth*0.35f, infGame.glHeight*0.42f,"Exit Game");
	glColor3f(0.76f, 0.86f, 0.93f);
		
	Font.glPrint((2.2f / 640.0f)*infGame.glWidth*0.5f, infGame.glWidth*0.36f, infGame.glHeight*0.31f, "Exit Menu -> Esc");

	glDisable(GL_ALPHA_TEST);
	glDisable(GL_TEXTURE_2D);

	glEnable(GL_LIGHTING);
	
	glMatrixMode(GL_PROJECTION);						// Selecciona la matriz de proyeccion
	glPopMatrix();										// Restaura la matriz de proyeccion anterior
	glMatrixMode(GL_MODELVIEW);							// Selecciona la matriz de modelo de vista
	glPopMatrix();										// Restaura la matriz de modelo de vista anterior
	glEnable(GL_DEPTH_TEST);							// Activa prueba de profundidad
}

void dibujaVolumendeSombra()
{
	CVector vert[2];
	CVector vertP[2];
	CVector vertD[2];
	CVector centro, centroP, centroD;

	CVector posLuz=CVector(player1.PosicionObj.x-23.0f, player1.PosicionObj.y+35.0f, player1.PosicionObj.z-4.0f);

	float ang, deltaAng;

	deltaAng=360.0f/16;

	for(int i=0; i<16; i++)
	{
		ang=i*deltaAng;
		vert[0].x=player1.PosicionObj.x+1.5f*cos(ang*PI/180.0f);
		vert[0].y=player1.PosicionObj.y+2.5f;
		vert[0].z=player1.PosicionObj.z+1.5f*sin(ang*PI/180.0f);

		ang=(i+1)*deltaAng;
		vert[1].x=player1.PosicionObj.x+1.5f*cos(ang*PI/180.0f);
		vert[1].y=player1.PosicionObj.y+2.5f;
		vert[1].z=player1.PosicionObj.z+1.5f*sin(ang*PI/180.0f);

		centro.x=player1.PosicionObj.x;
		centro.y=player1.PosicionObj.y+2.5f;
		centro.z=player1.PosicionObj.z;

		vertD[0]=Normaliza(vert[0]-posLuz);
		vertD[1]=Normaliza(vert[1]-posLuz);
		centroD=Normaliza(centro-posLuz);

		vertP[0]=vert[0]+vertD[0]*200.0f;
		vertP[1]=vert[1]+vertD[1]*200.0f;
		centroP=centro+centroD*200.0f;

		glBegin(GL_TRIANGLES);
			glVertex3f(vert[1].x, vert[1].y, vert[1].z);
			glVertex3f(vert[0].x, vert[0].y, vert[0].z);
			glVertex3f(player1.PosicionObj.x, player1.PosicionObj.y+4.0f, player1.PosicionObj.z);
		glEnd();

		glBegin(GL_QUADS);
			glVertex3f(vertP[1].x, vertP[1].y, vertP[1].z);
			glVertex3f(vertP[0].x, vertP[0].y, vertP[0].z);
			glVertex3f(vert[0].x, vert[0].y, vert[0].z);
			glVertex3f(vert[1].x, vert[1].y, vert[1].z);
		glEnd();

		glBegin(GL_TRIANGLES);
			glVertex3f(vertP[0].x, vertP[0].y, vertP[0].z);
			glVertex3f(vertP[1].x, vertP[1].y, vertP[1].z);
			glVertex3f(centroP.x, centroP.y, centroP.z);
		glEnd();

	}
}

//PARTÍCULAS -- cambio 4

//Aqui se dibuja cada partícula que consiste en un pequeño plano con alguna textura que representa algun
//efecto como humo, fuego, vapor, agua, etc. Debido a que se dibujan muchos planos y una forma de quitarle
//trabajo a la máquina sería dibujar directamente los dos triangulos que forman cada plano en vez de usar
//GL_QUADS se usa GL_TRIANGLE_STRIP para dibujar 2 triangulos en un solo paso, solo indicando 4 vertices.

//A cada particula se le aplica transparencia y la combinación de parámetros de glBlendFunc depende del efecto
//que se quiera representar. Antes de dibujar el plano se desactiva iluminación y se aplica un color de 4 componentes
//usando la vida de la partícula como valor de transparencia alpha

//Se actualiza la posición de cada partícula usando los valores definidos para velocidad y gravedad y se disminuye su vida
//en una cantidad definida por la variable fade.
//Cuando una partícula llega a cierto valor que se define segun el efecto a representar, se reinicia su posición y la vida
//de la partícula tambien. En ese paso se pueden hacer calculos con alguna función (por ejemplo senoidal) para definir un
//patrón de movimiento de las partículas. asignando el resultado a la nueva velocidad de la partícula
void DibujaParticulas()
{
	glDisable(GL_LIGHTING);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glEnable(GL_TEXTURE_2D);

	glDepthMask(GL_FALSE);

	glBindTexture(GL_TEXTURE_2D, textura[31].texID);

	for (int loop = 0; loop<MAX_PARTICULAS; loop++)
	{
		float x = particle[loop].x;
		float y = particle[loop].y;
		float z = particle[loop].z;

		// Se dibuja la particula usando los valores RGB, El desvanecimiento depende de su vida
		glColor4f(0.9f, 0.5f, 0.2f, particle[loop].life);

		glBegin(GL_TRIANGLE_STRIP); // Se construye un Quad a partir de un Triangle Strip
		glTexCoord2f(1, 1); glVertex3f(x + 2.0f, y + 2.0f, z + 2.0f); // Superior derecha
		glTexCoord2f(1, 0); glVertex3f(x + 2.0f, y + 2.0f, z - 2.0f); // Inferior derecha
		glTexCoord2f(0, 1); glVertex3f(x - 2.0f, y + 2.0f, z + 2.0f); // Superior izquierda
		glTexCoord2f(0, 0); glVertex3f(x - 2.0f, y + 2.0f, z - 2.0f); // Inferior izquierda
		glEnd();
		particle[loop].x += particle[loop].xi;	// Movimiento en el eje X a velocidad X
		particle[loop].y += particle[loop].yi;	// Movimiento en el eje Y a velocidad Y
		particle[loop].z += particle[loop].zi;	// Movimiento en el eje Z a velocidad Z

												//Efecto de gravedad
		particle[loop].xi += particle[loop].xg;
		particle[loop].yi += particle[loop].yg;
		particle[loop].zi += particle[loop].zg;

		particle[loop].life -= particle[loop].fade; // Se reduce la vida de las partículas una cantidad "Fade"

		if (particle[loop].life < 0.2f) // Si la vida de la partícula esta por agotarse
		{
			particle[loop].life = 2.0f; // Se le da nueva vida
			particle[loop].fade = 0.02; // Un valor aleatorio de desvanecimiento

										//Posición inicial de cada particula
			particle[loop].x = 0;
			particle[loop].y = 0;
			particle[loop].z = 0;

			//Gravedad
			particle[loop].xg = 0;
			particle[loop].yg = 0;
			particle[loop].zg = 0;
			
			//Velocidad
			particle[loop].xi = float((rand() % 50 + 1) - 25.0f)/100.0f;
			particle[loop].yi = 0;
			particle[loop].zi = 0;
		}

		posPartX = particle[loop].x + 2.0f;
		posPartY = particle[loop].y + 2.0f;
		posPartZ = particle[loop].z + 2.0f;

	}

	glDepthMask(GL_TRUE);

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);

	glEnable(GL_LIGHTING);
}

void lanzaBomba()
{
		glPushMatrix();
			glTranslatef(PosicionBomba.x, PosicionBomba.y, PosicionBomba.z);
			glRotatef(-165, 0, 1, 0);
			glScalef(0.02, 0.02, 0.02);
			g_Load3ds.Render3DSFile(&g_3DModel1e, textureModel1e, 1);
		glPopMatrix();
}

int RenderizaEscena(GLvoid)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glLoadIdentity();
		
	if (infGame.estadoJuego == 6)
	{
		sonidoF[0].pausaSonido(true);
		sonidoF[1].volumenSonido(0.5f);
		sonidoF[1].pausaSonido(false);
		gluLookAt(0.0f, 115.0f, 44.0f, 0.0f, -10.0f, 39.0f, 0.0f, 1.0f, 0.0f);
	/*	gluLookAt(player1.PosicionObj.x - 10.0f, player1.PosicionObj.y + 30.0f, player1.PosicionObj.z + 40.0f,
			player1.PosicionObj.x, player1.PosicionObj.y + 18.0f, player1.PosicionObj.z,
			0.0f, 1.0f, 0.0f);*/
	}
					
	ActualizaLuzParam();
		
	glDisable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	
	DibujaEscenarioBase();
	
	
//	dibujaCajaColision(&cajaPersonaje);
	dibujaCajaColision(&cajaBomba);

	dibujaCajaColision(&cajaParticula);

	for (int i = 0; i < 100; i++)
	{
		dibujaCajaColision(&cajaEscenario[i]);

	}
	
	if(player1.visible == true)
	{
		glPushMatrix();
			glTranslatef(player1.PosicionObj.x, player1.PosicionObj.y, player1.PosicionObj.z);
			glRotatef(player1.AngObj, 0.0f, 1.0f, 0.0f);
			glScalef(player1.escalaX,player1.escalaY,player1.escalaZ);
			DibujaPersonaje();
		glPopMatrix();
	}
	
	
	/*glPushMatrix();
	glTranslatef(0, 2.0f, 30.0f);
	DibujaParticulas();
	glRotatef(90, 0, 1.0f, 0);
	DibujaParticulas();
	glPopMatrix();*/

	glColorMask(0,0,0,0);
	glDepthMask(0);

	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 0, 0);
	glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);

	//1er paso de la prueba de pase de profundidad
	glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
	glCullFace(GL_BACK);
	dibujaVolumendeSombra();

	//2do paso de la prueba de pase de profundidad
	glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
	glCullFace(GL_FRONT);
	dibujaVolumendeSombra();

	glCullFace(GL_BACK);

	glColorMask(1,1,1,1);
	glDepthMask(1);

	glStencilFunc(GL_NOTEQUAL, 1, 1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	glEnable(GL_LIGHT0);
	glDisable(GL_LIGHT1);

	DibujaEscenarioBase();


	//Al presionar b y dejar una bomba
	if (timerBomba.TiempoActual() <= 1800)
	{
		actualizaCajaBomba();
		actualizaCajaParticula();
		lanzaBomba();
	}

	if (timerBomba.TiempoActual() > 1800 && timerBomba.TiempoActual() <= 2000)
	{
		glPushMatrix();
		glTranslatef(PosicionBomba.x, PosicionBomba.y + 2.0f, PosicionBomba.z);
		iniCajaBomba();
		iniCajaParticula();
		DibujaParticulas();
		glRotatef(90.0f, 0, 1.0f, 0);
		DibujaParticulas();
		glPopMatrix();
	}


	if (player1.visible == true)
	{
		glPushMatrix();
		glTranslatef(player1.PosicionObj.x, player1.PosicionObj.y, player1.PosicionObj.z);
		glRotatef(player1.AngObj, 0.0f, 1.0f, 0.0f);
		glScalef(player1.escalaX, player1.escalaY, player1.escalaZ);
		DibujaPersonaje();
		glPopMatrix();
	}

	glDisable(GL_STENCIL_TEST);

	DibujaTextos();
	actualizaMovPersonaje();
	if (colisionInicialBomba)
	{
		actualizaMovPersonaje();
		if (sqrt(pow((player1.PosicionObj.x - PosicionBomba.x),2.0f) + pow((player1.PosicionObj.z - PosicionBomba.z), 2.0f)) >= 5.0f)
			colisionInicialBomba = false;
	}
	if (existeBomba && !colisionInicialBomba)
		actualizaMovPersonaje2();
	CalculateFrameRate();

	return TRUE;
}

void dibujaSegunEstado()
{

	 if (infGame.estadoJuego == 1 || infGame.estadoJuego == 5)
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();
		dibujaTituloJuego();
	}
	else if (infGame.estadoJuego == 6)
	{
		RenderizaEscena();
		if (infGame.pausa == 1)
			dibujaMenuPausa();

		if (infGame.pausa == 0)
		{
			if (play)
			{
				if (tipoAnim == 1) //caminar
					animacion(KeyFrame1, maxKF1, 15);
			}
		}
	}
	else if (infGame.estadoJuego == 8)
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();

		dibujaMenuOpciones();
	}
}
GLvoid DestruyeVentanaOGL(GLvoid)						// Elimina la ventana apropiadamente
{
	if (hRC)											// Si existe un contexto de renderizado...
	{
		if (!wglMakeCurrent(NULL,NULL))					// Si no se pueden liberar los contextos DC y RC...
		{
			MessageBox(NULL,"Falla al liberar DC y RC.","Error de finalización",MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(hRC))						// Si no se puede eliminar el RC?
		{
			MessageBox(NULL,"Falla al liberar el contexto de renderizado.","Error de finalización",MB_OK | MB_ICONINFORMATION);
		}
		hRC=NULL;										// Se pone RC en NULL
	}

	if (hDC && !ReleaseDC(hWnd,hDC))					// Si no se puede eliminar el DC
	{
		MessageBox(NULL,"Falla al liberar el contexto de renderizado.","Error de finalización",MB_OK | MB_ICONINFORMATION);
		hDC=NULL;										// Se pone DC en NULL
	}

	if (hWnd && !DestroyWindow(hWnd))					// Si no se puede destruir la ventana
	{
		MessageBox(NULL,"No se pudo liberar hWnd.","Error de finalización",MB_OK | MB_ICONINFORMATION);
		hWnd=NULL;										// Se pone hWnd en NULL
	}

	if (!UnregisterClass("OpenGL",hInstance))			// Si no se puede eliminar el registro de la clase
	{
		MessageBox(NULL,"No se pudo eliminar el registro de la clase.","Error de finalización",MB_OK | MB_ICONINFORMATION);
		hInstance=NULL;									// Se pone hInstance en NULL
	}
}

//	Este código crea la ventana de OpenGL.  Parámetros:					
//	title			- Titulo en la parte superior de la ventana			
//	width			- Ancho de la ventana								
//	height			- Alto de la ventana								
//	bits			- Número de bits a usar para el color (8/16/24/32)	
  
BOOL CreaVentanaOGL(char* title, int width, int height, int bits)
{
	GLuint	PixelFormat;				// Guarda el resultado despues de determinar el formato a usar
	WNDCLASS	wc;						// Estructura de la clase ventana
	DWORD		dwExStyle;				// Estilo extendido de ventana
	DWORD		dwStyle;				// Estilo de ventana
	RECT		WindowRect;				// Guarda los valores Superior Izquierdo / Inferior Derecho del rectángulo
	WindowRect.left=(long)0;			// Inicia el valor Izquierdo a 0
	WindowRect.right=(long)width;		// Inicia el valor Derecho al ancho especificado
	WindowRect.top=(long)0;				// Inicia el valor Superior a 0
	WindowRect.bottom=(long)height;		// Inicia el valor Inferior al alto especificado

	hInstance			= GetModuleHandle(NULL);				// Guarda una instancia de la ventana
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redibuja el contenido de la ventana al redimensionarla
	wc.lpfnWndProc		= (WNDPROC) WndProc;					// Maneja los mensajes para WndProc
	wc.cbClsExtra		= 0;									// Ningun dato extra para la clase
	wc.cbWndExtra		= 0;									// Ningun dato extra para la ventana
	wc.hInstance		= hInstance;							// Inicia la instancia
	wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);			// Carga el ícono por defecto
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);			// Carga el puntero de flecha
	wc.hbrBackground	= NULL;									// No se requiere ningun fondo
	wc.lpszMenuName		= NULL;									// No hay menú en la ventana
	wc.lpszClassName	= "OpenGL";								// Fija el nombre de la clase.

	if (!RegisterClass(&wc))									// Intenta registrar la clase de ventana
	{
		MessageBox(NULL,"Failed To Register The Window Class.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;											
	}
		
	dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;					// Estilo extendido de ventana
	dwStyle=WS_OVERLAPPEDWINDOW;									// Estilo de ventana

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// Ajusta la ventana al tamaño especificado

	// Crea la ventana
	if (!(hWnd=CreateWindowEx(	dwExStyle,							// Estilo extendido para la ventana
								"OpenGL",							// Nombre de la clase
								title,								// Título de la ventana
								dwStyle |							// Definición del estilo de la ventana
								WS_CLIPSIBLINGS |					// Estilo requerido de la ventana
								WS_CLIPCHILDREN,					// Estilo requerido de la ventana
								0, 0,								// Posición de la ventana
								WindowRect.right-WindowRect.left,	// Calcula el ancho de la ventana
								WindowRect.bottom-WindowRect.top,	// Calcula el alto de la ventana
								NULL,								// No hay ventana superior
								NULL,								// No hay menú
								hInstance,							// Instancia
								NULL)))								// No se pasa nada a WM_CREATE
	{
		DestruyeVentanaOGL();										// Resetea el despliegue
		MessageBox(NULL,"Error al crear la ventana.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								
	}

	static	PIXELFORMATDESCRIPTOR pfd=				// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,							// Must Support Double Buffering
		PFD_TYPE_RGBA,								// Request An RGBA Format
		bits,										// Select Our Color Depth
		0, 0, 0, 0, 0, 0,							// Color Bits Ignored
		0,											// No Alpha Buffer
		0,											// Shift Bit Ignored
		0,											// No Accumulation Buffer
		0, 0, 0, 0,									// Accumulation Bits Ignored
		16,											// 16Bit Z-Buffer (Depth Buffer)  
		1,											// No Stencil Buffer
		0,											// No Auxiliary Buffer
		PFD_MAIN_PLANE,								// Main Drawing Layer
		0,											// Reserved
		0, 0, 0										// Layer Masks Ignored
	};
	
	if (!(hDC=GetDC(hWnd)))							// Si no se creo el contexto de dispositivo...
	{
		DestruyeVentanaOGL();						// Resetea el despliegue
		MessageBox(NULL,"No se puede crear un contexto de dispositivo GL.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								
	}

	if (!(PixelFormat=ChoosePixelFormat(hDC,&pfd)))	// Si Windows no encontró un formato de pixel compatible
	{
		DestruyeVentanaOGL();						// Resetea el despliegue
		MessageBox(NULL,"No se puede encontrar un formato de pixel compatible.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								
	}

	if(!SetPixelFormat(hDC,PixelFormat,&pfd))		// Si no se pudo habilitar el formato de pixel
	{
		DestruyeVentanaOGL();						// Resetea el despliegue
		MessageBox(NULL,"No se puede usar el formato de pixel.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								
	}

	if (!(hRC=wglCreateContext(hDC)))				// Si no se creo el contexto de renderizado
	{
		DestruyeVentanaOGL();						// Resetea el despliegue
		MessageBox(NULL,"No se puede crear un contexto de renderizado GL.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								
	}

	if(!wglMakeCurrent(hDC,hRC))					// Si no se puede activar el contexto de renderizado
	{
		DestruyeVentanaOGL();						// Resetea el despliegue
		MessageBox(NULL,"No se puede usar el contexto de renderizado GL.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								
	}

	ShowWindow(hWnd,SW_SHOW);				// Muestra la ventana
	SetForegroundWindow(hWnd);				// Le da la prioridad mas alta
	SetFocus(hWnd);							// Pasa el foco del teclado a la ventana
	ReDimensionaEscenaGL(width, height);	// Inicia la perspectiva para la ventana OGL

	if (!IniGL())							// Si no se inicializa la ventana creada
	{
		DestruyeVentanaOGL();				// Resetea el despliegue
		MessageBox(NULL,"Falla en la inicialización.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								
	}

	return TRUE;							// Todo correcto
}

LRESULT CALLBACK WndProc(	HWND	hWnd,	// Manejador para esta ventana
							UINT	uMsg,	// Mensaje para esta ventana
							WPARAM	wParam,	// Información adicional del mensaje
							LPARAM	lParam)	// Información adicional del mensaje
{
	switch (uMsg)							// Revisa los mensajes de la ventana
	{
		case WM_ACTIVATE:					// Revisa el mensaje de activación de ventana
		{
			if (!HIWORD(wParam))			// Revisa el estado de minimización
			{
				active=TRUE;				// El programa está activo
			}
			else
			{
				active=FALSE;				// El programa no está activo
			}

			return 0;						// Regresa al ciclo de mensajes
		}

		case WM_SYSCOMMAND:					// Intercepta comandos del sistema
		{
			switch (wParam)					// Revisa llamadas del sistema
			{
				case SC_SCREENSAVE:			// ¿Screensaver tratando de iniciar?
				case SC_MONITORPOWER:		// ¿Monitor tratando de entrar a modo de ahorro de energía?
				return 0;					// Evita que suceda
			}
			break;							// Sale del caso
		}

		case WM_CLOSE:						// Si se recibe un mensaje de cerrar...
		{
			PostQuitMessage(0);				// Se manda el mensaje de salida
			return 0;						// y se regresa al ciclo
		}

		case WM_KEYDOWN:					// Si se está presionando una tecla...
		{
			keys[wParam] = TRUE;			// Si es así, se marca como TRUE
			return 0;						// y se regresa al ciclo
		}

		case WM_KEYUP:						// ¿Se ha soltado una tecla?
		{
			keys[wParam] = FALSE;			// Si es así, se marca como FALSE
			return 0;						// y se regresa al ciclo
		}

		case WM_SIZE:						// Si se redimensiona la ventana...
		{
			ReDimensionaEscenaGL(LOWORD(lParam),HIWORD(lParam));  	// LoWord=Width, HiWord=Height
			return 0;						// y se regresa al ciclo
		}
	}

	// Pasa todos los mensajes no considerados a DefWindowProc
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

// Este es el punto de entrada al programa; la función principal 
int WINAPI WinMain(	HINSTANCE	hInstance,			// Instancia
					HINSTANCE	hPrevInstance,		// Instancia previa
					LPSTR		lpCmdLine,			// Parametros de la linea de comandos
					int			nCmdShow)			// Muestra el estado de la ventana
{
	MSG		msg;									// Estructura de mensajes de la ventana
	
	infGame.estadoJuego=0;
	infGame.glHeight=0;
	infGame.glWidth=0;

	static int estadoTeclaEsc=0;

	audioFunc.iniciaAudio();

	// Crea la ventana OpenGL
	if (!CreaVentanaOGL("Computación Gráfica Avanzada",640,480,16))
	{
		return 0;									// Salir del programa si la ventana no fue creada
	}

	while(!done)									// Mientras done=FALSE
	{
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))	// Revisa si hay mensajes en espera
		{
			if (msg.message==WM_QUIT)				// Si se ha recibido el mensje de salir...
			{
				done=TRUE;							// Entonces done=TRUE
			}
			else									// Si no, Procesa los mensajes de la ventana
			{
				TranslateMessage(&msg);				// Traduce el mensaje
				DispatchMessage(&msg);				// Envia el mensaje
			}
		}
		else										// Si no hay mensajes...
		{
			// Dibuja la escena. 
			if (active)								// Si está activo el programa...
			{
				if (keys[VK_ESCAPE])				// Si se ha presionado ESC
				{
					if(estadoTeclaEsc == 0)
					{
						if(infGame.pausa == 0)
						{
							infGame.opcionMenuPausa=0;
							infGame.pausa=1;
						}
						else
							infGame.pausa=0;

						estadoTeclaEsc=1;
					}
				}
				else								// De lo contrario, actualiza la pantalla
				{
					estadoTeclaEsc=0;

					audioFunc.actualizaSistemaAudio();

					//RenderizaEscena();				// Dibuja la escena
					dibujaSegunEstado();
					
					SwapBuffers(hDC);				// Intercambia los Buffers (Double Buffering)
				}

				//if(!ManejaTeclado()) return 0;
				controlFunc.actualizaControl();
				manejaEventosMain();
				controlEstados();
			}
			
		}
	}

	// Finalización del programa
	sonidoF[0].liberaSonido();
	sonidoF[1].liberaSonido();
	audioFunc.finalizaAudio();
	DescargaTexturas();
	DescargaModelos();
	//Cambios para Fuentes
	Font.DestroyFont();
	DestruyeVentanaOGL();							// Destruye la ventana
	return (msg.wParam);							// Sale del programa
}

void guardaJuego()
{
	FILE *F;
	float varF[3];
	float varF1[1];
	int varI[1];

	F=fopen("save.cga", "wb");

	varI[0]=infGame.estadoJuego;
	fwrite(varI, sizeof(int), 1, F);

	//pos player
	varF[0]=player1.PosicionObj.x;
	varF[1]=player1.PosicionObj.y;
	varF[2]=player1.PosicionObj.z;
	fwrite(varF, sizeof(float), 3, F);

	//Ang obj player
	varF1[0]=player1.AngObj;
	fwrite(varF1, sizeof(float), 1, F);

	//pos player
	varF[0]=player1.escalaX;
	varF[1]=player1.escalaY;
	varF[2]=player1.escalaZ;
	fwrite(varF, sizeof(float), 3, F);

	fclose(F);
}

void cargaJuego()
{
	FILE *F;
	float varF[3];
	float varF1[1];
	int varI[1];

	F=fopen("save.cga", "rb");

	fread(varI, sizeof(int), 1, F);
	infGame.estadoJuego=varI[0];

	//pos player
	fread(varF, sizeof(float), 3, F);
	player1.PosicionObj.x=varF[0];
	player1.PosicionObj.y=varF[1];
	player1.PosicionObj.z=varF[2];

	//angObj player
	fread(varF1, sizeof(float), 1, F);
	player1.AngObj=varF1[0];

	//escala player
	fread(varF, sizeof(float), 3, F);
	player1.escalaX=varF[0];
	player1.escalaY=varF[1];
	player1.escalaZ=varF[2];

	fclose(F);

}


















void guardaConfiguracion()
{
	FILE *F;
	int valI[1];

	F=fopen("configuracion.jue", "wb");

	//Dificultad
	valI[0]=infGame.opMenuPrinc.dificultad;
	fwrite(valI, sizeof(int), 1, F);

	//Num. vidas
	valI[0]=infGame.opMenuPrinc.numVidas;
	fwrite(valI, sizeof(int), 1, F);

	//Sonido
	valI[0]=infGame.opMenuPrinc.sonido;
	fwrite(valI, sizeof(int), 1, F);

	fclose(F);
}

void cargaConfiguracion()
{
	FILE *F;
	int valI[1];

	F=fopen("configuracion.jue", "rb");

	//Dificultad
	fread(valI, sizeof(int), 1, F);
	infGame.opMenuPrinc.dificultad=valI[0];
	
	//Num. vidas
	fread(valI, sizeof(int), 1, F);
	infGame.opMenuPrinc.numVidas=valI[0];

	//Sonido
	fread(valI, sizeof(int), 1, F);
	infGame.opMenuPrinc.sonido=valI[0];

	fclose(F);
}

void manejaEventosMain()
{
	static int estadoTeclaUp=0;
	static int estadoTeclaDown=0;
	static int estadoTeclaLeft=0;
	static int estadoTeclaRight=0;
	static int estadoTeclaEnter=0;
	static int estadoTeclaB = 0;

	static int menuPrincipalActivo=1;
	static int menuOpcionesActivo=1;

	//*************************************************************
	if(infGame.estadoJuego == 5) //Menú principal
	{
		if(controlFunc.obtieneEstadoTecla(0) == 1) //Up
		{
			if(estadoTeclaUp == 0)
			{
				if(infGame.opcionMenuSelec > 0)
					infGame.opcionMenuSelec--;

				estadoTeclaUp=1;
			}
		}
		else if(controlFunc.obtieneEstadoTecla(0) == 0) //Up
			estadoTeclaUp=0;

		if(controlFunc.obtieneEstadoTecla(1) == 1) //Down
		{
			if(estadoTeclaDown == 0)
			{
				if(infGame.opcionMenuSelec < 2)
					infGame.opcionMenuSelec++;

				estadoTeclaDown=1;
			}
		}
		else if(controlFunc.obtieneEstadoTecla(1) == 0) //Down
			estadoTeclaDown=0;

		if(controlFunc.obtieneEstadoTecla(6) == 1) //Enter
		{
			if(menuPrincipalActivo == 1)
			{
				if(infGame.opcionMenuSelec == 0)
				{
					infGame.estadoJuego=6;
					if(infGame.opMenuPrinc.sonido == 1)
					{
						sonidoF[1].pausaSonido(true);
						sonidoF[0].volumenSonido(0.5f);
						sonidoF[0].pausaSonido(false);
					}
				}
				else if(infGame.opcionMenuSelec == 1)
				{
					cargaJuego();
				}
				else if(infGame.opcionMenuSelec == 2)
				{
					infGame.estadoJuego=8;
					menuOpcionesActivo=0;
					infGame.opMenuPrinc.opcionSelec=0;
				}
			}
		}
		else if(controlFunc.obtieneEstadoTecla(6) == 0) //Enter
		{
			if(menuPrincipalActivo == 0)
				menuPrincipalActivo=1;
		}
	}

	//**********************************************************************
	if(infGame.estadoJuego == 8) //Menú Opciones principales
	{
		if(controlFunc.obtieneEstadoTecla(0) == 1) //Up
		{
			if(estadoTeclaUp == 0)
			{
				if(infGame.opMenuPrinc.opcionSelec > 0)
					infGame.opMenuPrinc.opcionSelec--;

				estadoTeclaUp=1;
			}
		}
		else if(controlFunc.obtieneEstadoTecla(0) == 0) //Up
			estadoTeclaUp=0;

		if(controlFunc.obtieneEstadoTecla(1) == 1) //Down
		{
			if(estadoTeclaDown == 0)
			{
				if(infGame.opMenuPrinc.opcionSelec < 3)
					infGame.opMenuPrinc.opcionSelec++;

				estadoTeclaDown=1;
			}
		}
		else if(controlFunc.obtieneEstadoTecla(1) == 0) //Down
			estadoTeclaDown=0;

		if(controlFunc.obtieneEstadoTecla(2) == 1) //Left
		{
			if(estadoTeclaLeft == 0)
			{
				if(infGame.opMenuPrinc.opcionSelec == 0) //vidas
				{
					if(infGame.opMenuPrinc.numVidas == 7)
						infGame.opMenuPrinc.numVidas=5;
					else if(infGame.opMenuPrinc.numVidas == 5)
						infGame.opMenuPrinc.numVidas=3;
				}
				else if(infGame.opMenuPrinc.opcionSelec == 1) //sonido
					infGame.opMenuPrinc.sonido=0;
					
				estadoTeclaLeft=1;
			}
		}
		else if(controlFunc.obtieneEstadoTecla(2) == 0) //Up
			estadoTeclaLeft=0;

		if(controlFunc.obtieneEstadoTecla(3) == 1) //Right
		{
			if(estadoTeclaRight == 0)
			{
				if(infGame.opMenuPrinc.opcionSelec == 0) //vidas
				{
					if(infGame.opMenuPrinc.numVidas == 3)
						infGame.opMenuPrinc.numVidas=5;
					else if(infGame.opMenuPrinc.numVidas == 5)
						infGame.opMenuPrinc.numVidas=7;
				}
				else if(infGame.opMenuPrinc.opcionSelec == 1) //sonido
					infGame.opMenuPrinc.sonido=1;

				estadoTeclaRight=1;
			}
		}
		else if(controlFunc.obtieneEstadoTecla(3) == 0) //!Right
			estadoTeclaRight=0;

		if(controlFunc.obtieneEstadoTecla(6) == 1) //Enter
		{
			if(menuOpcionesActivo == 1)
			{
				if(infGame.opMenuPrinc.opcionSelec == 2)
				{
					infGame.estadoJuego=5;
					menuPrincipalActivo=0;
					guardaConfiguracion();

					if(infGame.opMenuPrinc.sonido == 0)
					{
						sonidoF[0].pausaSonido(true);
						sonidoF[1].pausaSonido(true);
					}
				}
			}
		}
		else if(controlFunc.obtieneEstadoTecla(6) == 0) //Enter
		{
			if(menuOpcionesActivo == 0)
				menuOpcionesActivo=1;
		}
	}

	//************************************************************************
	else if(infGame.estadoJuego == 6)	//Juego
	{
		if(infGame.pausa == 0)	//Sin pausa
		{
			if(controlFunc.obtieneEstadoTecla(0) == 1 && controlFunc.obtieneEstadoTecla(2) == 0 && controlFunc.obtieneEstadoTecla(3) == 0) //Up
			{
				if(player1.kick==false && player1.saltando == false)
				{
					ControlPersonaje(3);
					player1.caminando=true;
					player1.dirZ=2;
					player1.AngObj=180.0f;
					
					if(fabs(player1.PosicionObj.y - altPiso) < 0.01f)
					{
						if(play==false) //Para que la asignación de valores siguiente solo se haga una vez y empiece la animación
						{
							//Se le asignan a las variables del personaje los 
							//valores almacenados en el primer keyframe para que
							//inicie desde ahí la animación.
							Angt1   = KeyFrame1[0].Angt1;
							Angt2   = KeyFrame1[0].Angt2;
							Angc1   = KeyFrame1[0].Angc1;
							Angc2   = KeyFrame1[0].Angc2;
							Angbi1  = KeyFrame1[0].Angbi1;
							Angbi2  = KeyFrame1[0].Angbi2;
							Angbib  = KeyFrame1[0].Angbib;
							Angbd1  = KeyFrame1[0].Angbd1;
							Angbd2  = KeyFrame1[0].Angbd2;
							Angbdb  = KeyFrame1[0].Angbdb;
							Angpizq = KeyFrame1[0].Angpizq;
							Angpder = KeyFrame1[0].Angpder;
							Angpi   = KeyFrame1[0].Angpi;
							Angpd   = KeyFrame1[0].Angpd;
							Xtor    = KeyFrame1[0].Xtor;
							Ytor    = KeyFrame1[0].Ytor;
							Ztor    = KeyFrame1[0].Ztor;

							play=true;
							playIndex=0;
							tipoAnim=1;
						}
					}
				}
			}

			if(controlFunc.obtieneEstadoTecla(1) == 1 && controlFunc.obtieneEstadoTecla(2) == 0 && controlFunc.obtieneEstadoTecla(3) == 0) //Down
			{
				if(player1.kick==false && player1.saltando == false)
				{
					ControlPersonaje(4);
					player1.caminando=true;
					player1.dirZ=1;
					player1.AngObj=0.0f;
					
					if(fabs(player1.PosicionObj.y - altPiso) < 0.01f)
					{
						if(play==false) //Para que la asignación de valores siguiente solo se haga una vez y empiece la animación
						{
							//Se le asignan a las variables del personaje los 
							//valores almacenados en el primer keyframe para que
							//inicie desde ahí la animación.
							Angt1   = KeyFrame1[0].Angt1;
							Angt2   = KeyFrame1[0].Angt2;
							Angc1   = KeyFrame1[0].Angc1;
							Angc2   = KeyFrame1[0].Angc2;
							Angbi1  = KeyFrame1[0].Angbi1;
							Angbi2  = KeyFrame1[0].Angbi2;
							Angbib  = KeyFrame1[0].Angbib;
							Angbd1  = KeyFrame1[0].Angbd1;
							Angbd2  = KeyFrame1[0].Angbd2;
							Angbdb  = KeyFrame1[0].Angbdb;
							Angpizq = KeyFrame1[0].Angpizq;
							Angpder = KeyFrame1[0].Angpder;
							Angpi   = KeyFrame1[0].Angpi;
							Angpd   = KeyFrame1[0].Angpd;
							Xtor    = KeyFrame1[0].Xtor;
							Ytor    = KeyFrame1[0].Ytor;
							Ztor    = KeyFrame1[0].Ztor;

							play=true;
							playIndex=0;
							tipoAnim=1;
						}
					}
				}
			}

			if(controlFunc.obtieneEstadoTecla(2) == 1 && controlFunc.obtieneEstadoTecla(0) == 0 && controlFunc.obtieneEstadoTecla(1) == 0) //Left
			{
				if(player1.kick==false && player1.saltando == false)
				{
					ControlPersonaje(2);
					player1.caminando=true;
					player1.dirX=2;
					player1.AngObj=-90.0f;
				
					if(fabs(player1.PosicionObj.y - altPiso) < 0.01f)
					{
						if(play==false) //Para que la asignación de valores siguiente solo se haga una vez y empiece la animación
						{
							//Se le asignan a las variables del personaje los 
							//valores almacenados en el primer keyframe para que
							//inicie desde ahí la animación.
							Angt1   = KeyFrame1[0].Angt1;
							Angt2   = KeyFrame1[0].Angt2;
							Angc1   = KeyFrame1[0].Angc1;
							Angc2   = KeyFrame1[0].Angc2;
							Angbi1  = KeyFrame1[0].Angbi1;
							Angbi2  = KeyFrame1[0].Angbi2;
							Angbib  = KeyFrame1[0].Angbib;
							Angbd1  = KeyFrame1[0].Angbd1;
							Angbd2  = KeyFrame1[0].Angbd2;
							Angbdb  = KeyFrame1[0].Angbdb;
							Angpizq = KeyFrame1[0].Angpizq;
							Angpder = KeyFrame1[0].Angpder;
							Angpi   = KeyFrame1[0].Angpi;
							Angpd   = KeyFrame1[0].Angpd;
							Xtor    = KeyFrame1[0].Xtor;
							Ytor    = KeyFrame1[0].Ytor;
							Ztor    = KeyFrame1[0].Ztor;

							play=true;
							playIndex=0;
							tipoAnim=1;
						}
					}
				}
			}

			if(controlFunc.obtieneEstadoTecla(3) == 1 && controlFunc.obtieneEstadoTecla(0) == 0 && controlFunc.obtieneEstadoTecla(1) == 0) //Right
			{
				if(player1.kick==false && player1.saltando == false)
				{
					ControlPersonaje(1);
					player1.caminando=true;
					player1.dirX=1;
					player1.AngObj=90.0f;
					
					if(fabs(player1.PosicionObj.y - altPiso) < 0.01f)
					{
						if(play==false) //Para que la asignación de valores siguiente solo se haga una vez y empiece la animación
						{
							//Se le asignan a las variables del personaje los 
							//valores almacenados en el primer keyframe para que
							//inicie desde ahí la animación.
							Angt1   = KeyFrame1[0].Angt1;
							Angt2   = KeyFrame1[0].Angt2;
							Angc1   = KeyFrame1[0].Angc1;
							Angc2   = KeyFrame1[0].Angc2;
							Angbi1  = KeyFrame1[0].Angbi1;
							Angbi2  = KeyFrame1[0].Angbi2;
							Angbib  = KeyFrame1[0].Angbib;
							Angbd1  = KeyFrame1[0].Angbd1;
							Angbd2  = KeyFrame1[0].Angbd2;
							Angbdb  = KeyFrame1[0].Angbdb;
							Angpizq = KeyFrame1[0].Angpizq;
							Angpder = KeyFrame1[0].Angpder;
							Angpi   = KeyFrame1[0].Angpi;
							Angpd   = KeyFrame1[0].Angpd;
							Xtor    = KeyFrame1[0].Xtor;
							Ytor    = KeyFrame1[0].Ytor;
							Ztor    = KeyFrame1[0].Ztor;

							play=true;
							playIndex=0;
							tipoAnim=1;
						}
					}
				}
			}

			if (controlFunc.obtieneEstadoTecla(7) == 1) //B Bomba
			{
				if (!existeBomba) 
				{
					timerBomba.iniciaTimer();
					PosicionBomba = player1.PosicionObj;
					existeBomba = true;
					colisionInicialBomba = true;
				}
				//Cuando desaparece la bomba, ya se puede poner otra bomba
				if (timerBomba.TiempoActual() > 2300)
					existeBomba = false;
			}


			if(!(controlFunc.obtieneEstadoTecla(0) == 1 || controlFunc.obtieneEstadoTecla(1) == 1))
				player1.dirZ=0;

			if(!(controlFunc.obtieneEstadoTecla(2) == 1 || controlFunc.obtieneEstadoTecla(3) == 1))
				player1.dirX=0;

			//Si no se presiona ninguna tecla de direccion
			if(!(controlFunc.obtieneEstadoTecla(0) == 1 || controlFunc.obtieneEstadoTecla(1) == 1
				|| controlFunc.obtieneEstadoTecla(2) == 1 || controlFunc.obtieneEstadoTecla(3) == 1))
			{
				//if(tipoAnim == 1)
				if(!player1.kick && !player1.saltando && fabs(player1.PosicionObj.y - altPiso) < 0.01f)
				{
					play=false;
					Angt1   = 0.0f;
					Angt2   = 0.0f;
					Angc1   = 0.0f;
					Angc2   = 0.0f;
					Angbi1  = 0.0f;
					Angbi2  = 0.0f;
					Angbib  = 0.0f;
					Angbd1  = 0.0f;
					Angbd2  = 0.0f;
					Angbdb  = 0.0f;
					Angpizq = 0.0f;
					Angpder = 0.0f;
					Angpi   = 0.0f;
					Angpd   = 0.0f;
					Xtor    = 0.0f;
					Ytor    = 0.0f;
					Ztor    = 0.0f;
				}
			}

		}


		else if(infGame.pausa == 1)
		{
			if(controlFunc.obtieneEstadoTecla(0) == 1) //Up
			{
				if(estadoTeclaUp == 0)
				{
					if(infGame.opcionMenuPausa > 0)
						infGame.opcionMenuPausa--;

					estadoTeclaUp=1;
				}
			}
			else if(controlFunc.obtieneEstadoTecla(0) == 0) //Up
				estadoTeclaUp=0;

			if(controlFunc.obtieneEstadoTecla(1) == 1) //Down
			{
				if(estadoTeclaDown == 0)
				{
					if(infGame.opcionMenuPausa < 2)
						infGame.opcionMenuPausa++;

					estadoTeclaDown=1;
				}
			}
			else if(controlFunc.obtieneEstadoTecla(1) == 0) //Down
				estadoTeclaDown=0;

			if(controlFunc.obtieneEstadoTecla(6) == 1) //Enter
			{
				if(estadoTeclaEnter == 0)
				{
					if (infGame.opcionMenuPausa == 0)
					{
						infGame.estadoJuego = 8;
						menuOpcionesActivo = 0;
						infGame.opMenuPrinc.opcionSelec = 0;
					}
					else if(infGame.opcionMenuPausa == 1)
					{
						infGame.estadoJuego=5;
						menuPrincipalActivo=0;
						infGame.pausa = 0;
					}
					else if(infGame.opcionMenuPausa == 2)
						done=TRUE;						// ESC indica el termino del programa

					estadoTeclaEnter=1;
				}
			}
			else if(controlFunc.obtieneEstadoTecla(6) == 0) //Enter
				estadoTeclaEnter=0;
		}
	}
}