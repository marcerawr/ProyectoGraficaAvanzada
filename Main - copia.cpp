#include "Main.h"
#include "3ds.h"

HDC			hDC=NULL;		// Dispositivo de contexto GDI
HGLRC		hRC=NULL;		// Contexto de renderizado
HWND		hWnd=NULL;		// Manejador de ventana
HINSTANCE	hInstance;		// Instancia de la aplicacion

bool	keys[256];			// Arreglo para el manejo de teclado
bool	active=TRUE;		// Bandera de ventana activa

int glWidth;
int glHeight;

//Apuntador para primitivas de cuadricas
GLUquadricObj	*e;

void controlEventos();

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

float altPiso;

//Contenedor de texturas para el escenario
CTga textura[36];

//Variables para animaciones
const int maxKF1=3; //Num. total de KeyFrames para la secuencia 1 (caminar)
const int maxKF2=4; //Num. total de KeyFrames para la secuencia 1 (caminar)

FRAME KeyFrame1[maxKF1]; //Contenedor para almacenar cada keyframe de la secuencia 1
FRAME KeyFrame2[maxKF2]; //Contenedor para almacenar cada keyframe de la secuencia 1

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

//Cambios para Fuentes
//Buscar el comentario anterior para ver todos los cambios necesarios
//Acceso a la clase CFont
CFont Font;

CControl controlFunc;

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

	glWidth=width;
	glHeight=height;
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
	
}

void InicializaParametrosdeControl()
{
	//Esta función establece los parámetros como velocidad del objeto y distancia de la cámara así como la posición y dirección iniciales

	player1.visible=true;

	player1.VelocidadObj=0.3f;
	player1.DistanciaCam=50.0f;
	
	player1.CamaraPosAlt=15.0f;	//Posición en y de la cámara (altura a la que se situa la cámara)
	player1.CamaraObjAlt=8.0f;	//Posición en y del objetivo de la cámara (altura a la que ve la cámara)
	player1.AngDir=0.0f;		//Este ángulo inicial hace que la dirección inicial sea paralela al eje Z y con sentido negativo
	player1.AngDirCam=0.0f;		//Este es el ángulo inicial que define la posición de la camara respecto al personaje
	player1.AngObj=90.0f;		//Este valor se elige dependiendo de la orientación con la que aparece el modelo en la escena al dibujarlo
								//sin aplicarle ninguna transformación (hacia adonde está volteando). Se elige un ángulo tal que al aplicarle
								//una rotación inicial con respecto al eje Y esté viendo hacia la misma dirección que la definida por AngDir
		
	player1.PosicionObj=CVector(-80.0f, 0.0f, 10.0f); //Esta es la posición inicial del objeto en la escena
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

		KeyFrame1[i].incAngt1=false;
		KeyFrame1[i].incAngt1=false;
		KeyFrame1[i].incAngc1=false;
		KeyFrame1[i].incAngc2=false;
		KeyFrame1[i].incAngbi1=false;
		KeyFrame1[i].incAngbi2=false;
		KeyFrame1[i].incAngbib=false;
		KeyFrame1[i].incAngbd1=false;
		KeyFrame1[i].incAngbd2=false;
		KeyFrame1[i].incAngbdb=false;
		KeyFrame1[i].incAngpizq=false;
		KeyFrame1[i].incAngpder=false;
		KeyFrame1[i].incAngpi=false;
		KeyFrame1[i].incAngpd=false;
		KeyFrame1[i].incXtor=false;
		KeyFrame1[i].incYtor=false;
		KeyFrame1[i].incZtor=false;
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

		KeyFrame2[i].incAngt1=false;
		KeyFrame2[i].incAngt1=false;
		KeyFrame2[i].incAngc1=false;
		KeyFrame2[i].incAngc2=false;
		KeyFrame2[i].incAngbi1=false;
		KeyFrame2[i].incAngbi2=false;
		KeyFrame2[i].incAngbib=false;
		KeyFrame2[i].incAngbd1=false;
		KeyFrame2[i].incAngbd2=false;
		KeyFrame2[i].incAngbdb=false;
		KeyFrame2[i].incAngpizq=false;
		KeyFrame2[i].incAngpder=false;
		KeyFrame2[i].incAngpi=false;
		KeyFrame2[i].incAngpd=false;
		KeyFrame2[i].incXtor=false;
		KeyFrame2[i].incYtor=false;
		KeyFrame2[i].incZtor=false;
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
    glEnable(GL_LIGHTING);								// Habilita la iluminación

	e=gluNewQuadric();

	CargaModelos();
	IniVarsPlayer();
	InicializaParametrosdeControl();
	InicializaCamara();
	
	CargaTexturas();
	InicializaAnim();
	DatosAnimacion();

	//Cambios para Fuentes
	Font.BuildFont();
	controlFunc.inicializaControl();

	return TRUE;										
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
	if(funcion == 1) //Giro a la derecha
	{
		player1.AngDir+=1.0f;
		if(player1.AngDir > 360.0f)
			player1.AngDir-=360.0f;

		player1.AngObj-=1.0f;
		if(player1.AngObj < 0.0f)
			player1.AngObj+=360.0f;

		player1.Direccion.x=(float)cos(player1.AngDir*PI/180.0f);
		player1.Direccion.y=0.0f;
		player1.Direccion.z=(float)sin(player1.AngDir*PI/180.0f);
	}
	else if(funcion == 2) //Giro a la izquierda
	{
		player1.AngDir-=1.0f;
		if(player1.AngDir < 0.0f)
			player1.AngDir+=360.0f;

		player1.AngDirCam-=1.0f;
		if(player1.AngDirCam < 0.0f)
			player1.AngDirCam+=360.0f;
		
		player1.AngObj+=1.0f;
		if(player1.AngObj > 360.0f)
			player1.AngObj-=360.0f;

		player1.Direccion.x=(float)cos(player1.AngDir*PI/180.0f);
		player1.Direccion.y=0.0f;
		player1.Direccion.z=(float)sin(player1.AngDir*PI/180.0f);
	
	}
	else if(funcion == 3) //Avanza hacia adelante
	{
		player1.PosicionObj=player1.PosicionObj+player1.Direccion*player1.VelocidadObj;
		player1.PosicionCam=player1.PosicionObj-player1.DireccionCam*player1.DistanciaCam;
		player1.PosicionCam.y=player1.CamaraPosAlt;
		player1.ObjetivoCam=player1.PosicionObj;
		player1.ObjetivoCam.y=player1.CamaraObjAlt;

		player1.PosicionObjAnt=player1.PosicionObj;

		ActualizaCamara();

	}
	else if(funcion == 4) //Avanza hacia atrás
	{
		player1.PosicionObj=player1.PosicionObj-player1.Direccion*player1.VelocidadObj;
		player1.PosicionCam=player1.PosicionObj-player1.DireccionCam*player1.DistanciaCam;
		player1.PosicionCam.y=player1.CamaraPosAlt;
		player1.ObjetivoCam=player1.PosicionObj;
		player1.ObjetivoCam.y=player1.CamaraObjAlt;

		player1.PosicionObjAnt=player1.PosicionObj;

		ActualizaCamara();

	}
	else if(funcion == 5) //Sube objetivo de la cámara
	{
		camara1.ObjetivoCam.y+=1.0f;
		player1.CamaraObjAlt+=1.0f;
	}
	else if(funcion == 6) //Baja objetivo de la cámara
	{
		camara1.ObjetivoCam.y-=1.0f;
		player1.CamaraObjAlt-=1.0f;
	}
	
}

void ControlPersonaje2(int funcion)
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
}

void ControlPersonaje2D(int funcion)
{
	if(funcion == 1) //Avanza a la derecha
	{
		player1.AngObj=90.0f;

		player1.Direccion=CVector(1.0f,0.0f,0.0f);

		player1.PosicionObj=player1.PosicionObj+player1.Direccion*player1.VelocidadObj;
		player1.PosicionCam=player1.PosicionObj-player1.DireccionCam*player1.DistanciaCam;
		player1.PosicionCam.y=player1.CamaraPosAlt;
		player1.ObjetivoCam=player1.PosicionObj;
		player1.ObjetivoCam.y=player1.CamaraObjAlt;

		player1.PosicionObjAnt=player1.PosicionObj;

	}
	else if(funcion == 2) //Giro a la izquierda
	{
		player1.AngObj=270.0f;

		player1.Direccion=CVector(-1.0f,0.0f,0.0f);

		player1.PosicionObj=player1.PosicionObj+player1.Direccion*player1.VelocidadObj;
		player1.PosicionCam=player1.PosicionObj-player1.DireccionCam*player1.DistanciaCam;
		player1.PosicionCam.y=player1.CamaraPosAlt;
		player1.ObjetivoCam=player1.PosicionObj;
		player1.ObjetivoCam.y=player1.CamaraObjAlt;

		player1.PosicionObjAnt=player1.PosicionObj;
	}
	
}

void DibujaEjes()
{
	glBegin(GL_LINES);
		//Eje X
		glColor3f(1.0f,0.0f,0.0f);
		glVertex3f(-100.0f, 0.0f, 0.0f);
		glVertex3f( 100.0f, 0.0f, 0.0f);

		//Eje Y
		glColor3f(0.0f,1.0f,0.0f);
		glVertex3f(0.0f, -100.0f, 0.0f);
		glVertex3f(0.0f,  100.0f, 0.0f);

		//Eje Z
		glColor3f(0.0f,0.0f,1.0f);
		glVertex3f(0.0f, 0.0f, -100.0f);
		glVertex3f(0.0f, 0.0f,  100.0f);
	glEnd();

	glPointSize(10.0f);

	glBegin(GL_POINTS);
		//"Flecha" eje X
		glColor3f(1.0f,0.0f,0.0f);
		glVertex3f( 100.0f, 0.0f, 0.0f);

		//"Flecha" eje Y
		glColor3f(0.0f,1.0f,0.0f);
		glVertex3f(0.0f,  100.0f, 0.0f);

		//"Flecha" eje Z
		glColor3f(0.0f,0.0f,1.0f);
		glVertex3f(0.0f, 0.0f,  100.0f);
	glEnd();

	glPointSize(1.0f);

	glColor3f(1.0f,1.0f,1.0f);
}

void DibujaCaja(int tipo)
{
	glEnable(GL_TEXTURE_2D);

	//cara frontal
	if(tipo == 1)
		glBindTexture(GL_TEXTURE_2D, textura[17].texID);
	else if(tipo == 2)
		glBindTexture(GL_TEXTURE_2D, textura[18].texID);
	else if(tipo == 3)
		glBindTexture(GL_TEXTURE_2D, textura[19].texID);

	glBegin(GL_QUADS);
		glNormal3f(0.0f,0.0f,1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-2.5f, -2.0f, 3.5f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f( 2.5f, -2.0f, 3.5f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f( 2.5f,  2.0f, 3.5f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-2.5f,  2.0f, 3.5f);
	glEnd();

	//cara izq
	glBindTexture(GL_TEXTURE_2D, textura[20].texID);

	glBegin(GL_QUADS);
		glNormal3f(-1.0f,0.0f,0.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-2.5f, -2.0f, -3.5f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-2.5f, -2.0f,  3.5f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-2.5f,  2.0f,  3.5f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-2.5f,  2.0f, -3.5f);
	glEnd();

	//cara der
	glBindTexture(GL_TEXTURE_2D, textura[20].texID);

	glBegin(GL_QUADS);
		glNormal3f(1.0f,0.0f,0.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(2.5f, -2.0f,  3.5f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(2.5f, -2.0f, -3.5f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(2.5f,  2.0f, -3.5f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(2.5f,  2.0f,  3.5f);
	glEnd();

	//cara sup
	glBindTexture(GL_TEXTURE_2D, textura[21].texID);

	glBegin(GL_QUADS);
		glNormal3f(0.0f,1.0f,0.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-2.5f, 2.0f, -3.5f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-2.5f, 2.0f,  3.5f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f( 2.5f, 2.0f,  3.5f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f( 2.5f, 2.0f, -3.5f);
	glEnd();

	glDisable(GL_TEXTURE_2D);
}

void DibujaTransportadora()
{
	CVector V1, V2, N;
	static float animTs=0.0f;

	glEnable(GL_TEXTURE_2D);

	//Banda transportadora cara superior
	glBindTexture(GL_TEXTURE_2D, textura[24].texID);

	glBegin(GL_QUADS);
		glNormal3f(0.0f,1.0f,0.0f);
		glTexCoord2f(0.0f-animTs, 0.0f); glVertex3f(-8.0f, 1.5f,  3.0f);
		glTexCoord2f(4.0f-animTs, 0.0f); glVertex3f( 8.0f, 1.5f,  3.0f);
		glTexCoord2f(4.0f-animTs, 1.0f); glVertex3f( 8.0f, 1.5f, -3.0f);
		glTexCoord2f(0.0f-animTs, 1.0f); glVertex3f(-8.0f, 1.5f, -3.0f);
	glEnd();

	//Banda transportadora cara izquierda inclinada
	
	V1=CVector(-8.0f, 1.5f,  3.0f)-CVector(-9.0f, 1.3f,  3.0f);
	V2=CVector(-9.0f, 1.3f, -3.0f)-CVector(-9.0f, 1.3f,  3.0f);
	N=Normaliza(Cruz(V1, V2));

	glBegin(GL_QUADS);
		glNormal3f(N.x,N.y,N.z);
		glTexCoord2f(0.6f-animTs, 0.0f); glVertex3f(-9.0f, 1.0f,  3.0f);
		glTexCoord2f(1.0f-animTs, 0.0f); glVertex3f(-8.0f, 1.5f,  3.0f);
		glTexCoord2f(1.0f-animTs, 1.0f); glVertex3f(-8.0f, 1.5f, -3.0f);
		glTexCoord2f(0.6f-animTs, 1.0f); glVertex3f(-9.0f, 1.0f, -3.0f);
	glEnd();
	
	//Banda transportadora cara izquierda vertical
	
	glBegin(GL_QUADS);
		glNormal3f(-1.0f,0.0f,0.0f);
		glTexCoord2f(0.0f-animTs, 0.0f); glVertex3f(-9.0f, -0.5f,  3.0f);
		glTexCoord2f(0.6f-animTs, 0.0f); glVertex3f(-9.0f,  1.0f,  3.0f);
		glTexCoord2f(0.6f-animTs, 1.0f); glVertex3f(-9.0f,  1.0f, -3.0f);
		glTexCoord2f(0.0f-animTs, 1.0f); glVertex3f(-9.0f, -0.5f, -3.0f);
	glEnd();

	//Banda transportadora cara derecha inclinada
	
	V1=CVector(9.0f, 1.0f,  3.0f)-CVector(8.0f, 1.5f,  3.0f);
	V2=CVector(8.0f, 1.5f, -3.0f)-CVector(8.0f, 1.5f,  3.0f);
	N=Normaliza(Cruz(V1, V2));

	glBegin(GL_QUADS);
		glNormal3f(N.x,N.y,N.z);
		glTexCoord2f(0.0f-animTs, 0.0f); glVertex3f(8.0f, 1.5f,  3.0f);
		glTexCoord2f(0.4f-animTs, 0.0f); glVertex3f(9.0f, 1.0f,  3.0f);
		glTexCoord2f(0.4f-animTs, 1.0f); glVertex3f(9.0f, 1.0f, -3.0f);
		glTexCoord2f(0.0f-animTs, 1.0f); glVertex3f(8.0f, 1.5f, -3.0f);
	glEnd();

	//Banda transportadora cara frontal 1
	glBindTexture(GL_TEXTURE_2D, textura[23].texID);

	glBegin(GL_QUADS);
		glNormal3f(0.0f,1.0f,0.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-7.0f, -1.5f, 3.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-4.0f, -1.5f, 3.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-4.0f,  1.5f, 3.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-7.0f,  1.5f, 3.0f);
	glEnd();

	//Banda transportadora cara frontal 2
	glBindTexture(GL_TEXTURE_2D, textura[22].texID);

	glBegin(GL_QUADS);
		glNormal3f(0.0f,1.0f,0.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-4.0f, -1.5f, 3.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f( 7.0f, -1.5f, 3.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f( 7.0f,  1.5f, 3.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-4.0f,  1.5f, 3.0f);
	glEnd();

	glDisable(GL_TEXTURE_2D);

	glDisable(GL_LIGHTING);

	glColor3f(0.0f,0.0f,0.0f);

	glBegin(GL_POLYGON);
		glVertex3f(-7.0f, -1.5f, 3.0f);
		glVertex3f(-7.0f,  1.5f, 3.0f);
		glVertex3f(-8.0f,  1.5f, 3.0f);
		glVertex3f(-9.0f,  1.0f, 3.0f);
		glVertex3f(-9.0f, -0.5f, 3.0f);
	glEnd();

	glBegin(GL_POLYGON);
		glVertex3f(7.0f, -1.5f, 3.0f);
		glVertex3f(9.0f, -0.5f, 3.0f);
		glVertex3f(9.0f,  1.0f, 3.0f);
		glVertex3f(8.0f,  1.5f, 3.0f);
		glVertex3f(7.0f,  1.5f, 3.0f);
	glEnd();
/*
	glBegin(GL_QUADS);
		glVertex3f(-7.0f, -1.5f, 3.0f);
		glVertex3f(-3.0f, -1.5f, 3.0f);
		glVertex3f(-3.0f,  1.5f, 3.0f);
		glVertex3f(-7.0f,  1.5f, 3.0f);
	glEnd();
*/
	glColor3f(1.0f,1.0f,1.0f);

	glEnable(GL_LIGHTING);

	if(animTs < 1.0f)
		animTs+=0.01f;
	else
		animTs=0.0f;
}

void DibujaEscenario()
{
	//-- Aquí comienzan los planos con textura
	glEnable(GL_TEXTURE_2D);

	//Plano 1 (pared 1)
	glBindTexture(GL_TEXTURE_2D, textura[2].texID);

	glBegin(GL_QUADS);
		glNormal3f(0.0f,0.0f,1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-90.0f, 20.0f, -20.0f);
		glTexCoord2f(8.0f, 0.0f); glVertex3f( 80.0f, 20.0f, -20.0f);
		glTexCoord2f(8.0f, 5.0f); glVertex3f( 80.0f, 80.0f, -20.0f);
		glTexCoord2f(0.0f, 5.0f); glVertex3f(-90.0f, 80.0f, -20.0f);
	glEnd();

	//Plano 2 (piso 1)
	glBindTexture(GL_TEXTURE_2D, textura[4].texID);

	glBegin(GL_QUADS);
		glNormal3f(0.0f,1.0f,0.0f);
		glTexCoord2f(0.1f, 0.0f); glVertex3f(-90.0f, 0.0f, 20.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-60.0f, 0.0f, 20.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-60.0f, 0.0f,  0.0f);
		glTexCoord2f(0.1f, 1.0f); glVertex3f(-90.0f, 0.0f,  0.0f);
	glEnd();

	//Plano 3 (pared 2)
	glBindTexture(GL_TEXTURE_2D, textura[3].texID);

	glBegin(GL_QUADS);
		glNormal3f(0.0f,0.0f,1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-90.0f,  0.0f, 0.0f);
		glTexCoord2f(3.0f, 0.0f); glVertex3f(-60.0f,  0.0f, 0.0f);
		glTexCoord2f(3.0f, 1.0f); glVertex3f(-60.0f, 15.0f, 0.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-90.0f, 15.0f, 0.0f);
	glEnd();

	//Plano 4 (pared 3)
	glBindTexture(GL_TEXTURE_2D, textura[0].texID);

	glBegin(GL_QUADS);
		glNormal3f(0.0f,0.0f,1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-60.0f,  5.0f, 0.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-45.0f,  5.0f, 0.0f);
		glTexCoord2f(1.0f, 0.7f); glVertex3f(-45.0f, 15.0f, 0.0f);
		glTexCoord2f(0.0f, 0.7f); glVertex3f(-60.0f, 15.0f, 0.0f);
	glEnd();

	//Plano 5 (pared 4)
	glBindTexture(GL_TEXTURE_2D, textura[0].texID);

	glBegin(GL_QUADS);
		glNormal3f(0.0f,0.0f,1.0f);
		glTexCoord2f(-0.6f, 0.0f); glVertex3f(-90.0f, 15.0f, -10.0f);
		glTexCoord2f( 1.4f, 0.0f); glVertex3f(-45.0f, 15.0f, -10.0f);
		glTexCoord2f( 1.4f, 1.0f); glVertex3f(-45.0f, 30.0f, -10.0f);
		glTexCoord2f(-0.6f, 1.0f); glVertex3f(-90.0f, 30.0f, -10.0f);
	glEnd();

	//Plano 6 (piso 2)
	glBindTexture(GL_TEXTURE_2D, textura[1].texID);

	glBegin(GL_QUADS);
		glNormal3f(0.0f,1.0f,0.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-90.0f, 15.0f,   0.0f);
		glTexCoord2f(2.5f, 0.0f); glVertex3f(-45.0f, 15.0f,   0.0f);
		glTexCoord2f(2.5f, 1.0f); glVertex3f(-45.0f, 15.0f, -10.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-90.0f, 15.0f, -10.0f);
	glEnd();

	//Plano 7 (pared 5)
	glBindTexture(GL_TEXTURE_2D, textura[16].texID);

	glBegin(GL_QUADS);
		glNormal3f(0.0f,0.0f,1.0f);
		glTexCoord2f(0.0f, 0.5f); glVertex3f(-90.0f, -20.0f, 20.0f);
		glTexCoord2f(1.0f, 0.5f); glVertex3f(-60.0f, -20.0f, 20.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-60.0f,   0.0f, 20.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-90.0f,   0.0f, 20.0f);
	glEnd();

	//Plano 8 (bloque piso)
	glBindTexture(GL_TEXTURE_2D, textura[5].texID);

	glBegin(GL_QUADS);
		glNormal3f(0.0f,1.0f,0.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-60.0f, 5.0f, 10.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-45.0f, 5.0f, 10.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-45.0f, 5.0f,  0.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-60.0f, 5.0f,  0.0f);
	glEnd();

	//Plano 9 (bloque pared front)
	glBindTexture(GL_TEXTURE_2D, textura[6].texID);

	glBegin(GL_QUADS);
		glNormal3f(0.0f,0.0f,1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-60.0f, 0.0f, 10.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-45.0f, 0.0f, 10.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-45.0f, 5.0f, 10.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-60.0f, 5.0f, 10.0f);
	glEnd();

	//Plano 10 (bloque pared izq)
	glBindTexture(GL_TEXTURE_2D, textura[6].texID);

	glBegin(GL_QUADS);
		glNormal3f(-1.0f,0.0f,0.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-60.0f, 0.0f,  0.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-60.0f, 0.0f, 10.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-60.0f, 5.0f, 10.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-60.0f, 5.0f,  0.0f);
	glEnd();

	//Plano 11 (pared 6)
	glBindTexture(GL_TEXTURE_2D, textura[12].texID);

	glBegin(GL_QUADS);
		glNormal3f(0.0f,0.0f,1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-60.0f, -20.0f, 10.0f);
		glTexCoord2f(4.0f, 0.0f); glVertex3f(-45.0f, -20.0f, 10.0f);
		glTexCoord2f(4.0f, 8.0f); glVertex3f(-45.0f,   0.0f, 10.0f);
		glTexCoord2f(0.0f, 8.0f); glVertex3f(-60.0f,   0.0f, 10.0f);
	glEnd();

	//Plano 12 (pared 7)
	glBindTexture(GL_TEXTURE_2D, textura[12].texID);

	glBegin(GL_QUADS);
		glNormal3f(1.0f,0.0f,0.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-60.0f, -20.0f, 20.0f);
		glTexCoord2f(4.0f, 0.0f); glVertex3f(-60.0f, -20.0f, 10.0f);
		glTexCoord2f(4.0f, 8.0f); glVertex3f(-60.0f,   0.0f, 10.0f);
		glTexCoord2f(0.0f, 8.0f); glVertex3f(-60.0f,   0.0f, 20.0f);
	glEnd();

	//Plano 13 (pared 8)
	glBindTexture(GL_TEXTURE_2D, textura[7].texID);

	glBegin(GL_QUADS);
		glNormal3f(-1.0f,0.0f,0.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-45.0f, 15.0f, -10.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-45.0f, 15.0f,   0.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-45.0f, 30.0f,   0.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-45.0f, 30.0f, -10.0f);
	glEnd();

	//Plano 14 (pared 9)
	glBindTexture(GL_TEXTURE_2D, textura[8].texID);

	glBegin(GL_QUADS);
		glNormal3f(0.0f,0.0f,1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-45.0f,  5.0f, 0.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(  0.0f,  5.0f, 0.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(  0.0f, 30.0f, 0.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-45.0f, 30.0f, 0.0f);
	glEnd();

	//Plano 15 (piso 3)
	glBindTexture(GL_TEXTURE_2D, textura[1].texID);

	glBegin(GL_QUADS);
		glNormal3f(0.0f,1.0f,0.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-45.0f, 30.0f,   0.0f);
		glTexCoord2f(2.5f, 0.0f); glVertex3f(  0.0f, 30.0f,   0.0f);
		glTexCoord2f(2.5f, 1.0f); glVertex3f(  0.0f, 30.0f, -10.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-45.0f, 30.0f, -10.0f);
	glEnd();

	//Plano 16 (piso 4)
	glBindTexture(GL_TEXTURE_2D, textura[11].texID);

	glBegin(GL_QUADS);
		glNormal3f(0.0f,1.0f,0.0f);
		glTexCoord2f( 0.0f, 0.0f); glVertex3f(-45.0f, 8.0f, 22.0f);
		glTexCoord2f(14.0f, 0.0f); glVertex3f(  0.0f, 8.0f, 22.0f);
		glTexCoord2f(14.0f, 5.0f); glVertex3f(  0.0f, 8.0f,  0.0f);
		glTexCoord2f( 0.0f, 5.0f); glVertex3f(-45.0f, 8.0f,  0.0f);
	glEnd();

	//Plano 17 (pared 10)
	glBindTexture(GL_TEXTURE_2D, textura[10].texID);

	glBegin(GL_QUADS);
		glNormal3f(-1.0f,0.0f,0.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-45.0f, 4.0f,  0.0f);
		glTexCoord2f(5.0f, 0.0f); glVertex3f(-45.0f, 4.0f, 22.0f);
		glTexCoord2f(5.0f, 2.0f); glVertex3f(-45.0f, 8.0f, 22.0f);
		glTexCoord2f(0.0f, 2.0f); glVertex3f(-45.0f, 8.0f,  0.0f);
	glEnd();

	//Plano 18 (pared 11)
	glBindTexture(GL_TEXTURE_2D, textura[9].texID);

	glBegin(GL_QUADS);
		glNormal3f(0.0f,0.0f,1.0f);
		glTexCoord2f( 0.0f, 0.0f); glVertex3f(-45.0f, 4.0f, 22.0f);
		glTexCoord2f(14.0f, 0.0f); glVertex3f(  0.0f, 4.0f, 22.0f);
		glTexCoord2f(14.0f, 2.0f); glVertex3f(  0.0f, 8.0f, 22.0f);
		glTexCoord2f( 0.0f, 2.0f); glVertex3f(-45.0f, 8.0f, 22.0f);
	glEnd();

	//Plano 19 (piso 5)
	glBindTexture(GL_TEXTURE_2D, textura[11].texID);

	glBegin(GL_QUADS);
		glNormal3f(0.0f,1.0f,0.0f);
		glTexCoord2f( 0.0f, 0.0f); glVertex3f(-45.0f, 4.0f, 25.0f);
		glTexCoord2f(14.0f, 0.0f); glVertex3f(  0.0f, 4.0f, 25.0f);
		glTexCoord2f(14.0f, 1.0f); glVertex3f(  0.0f, 4.0f, 22.0f);
		glTexCoord2f( 0.0f, 1.0f); glVertex3f(-45.0f, 4.0f, 22.0f);
	glEnd();

	//Plano 20 (pared 12)
	glBindTexture(GL_TEXTURE_2D, textura[10].texID);

	glBegin(GL_QUADS);
		glNormal3f(-1.0f,0.0f,0.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-45.0f, -6.0f,  0.0f);
		glTexCoord2f(6.0f, 0.0f); glVertex3f(-45.0f, -6.0f, 25.0f);
		glTexCoord2f(6.0f, 5.0f); glVertex3f(-45.0f,  4.0f, 25.0f);
		glTexCoord2f(0.0f, 5.0f); glVertex3f(-45.0f,  4.0f,  0.0f);
	glEnd();

	//Plano 21 (pared 13)
	glBindTexture(GL_TEXTURE_2D, textura[9].texID);

	glBegin(GL_QUADS);
		glNormal3f(0.0f,0.0f,1.0f);
		glTexCoord2f( 0.0f, 0.0f); glVertex3f(-45.0f, -6.0f, 25.0f);
		glTexCoord2f(14.0f, 0.0f); glVertex3f(  0.0f, -6.0f, 25.0f);
		glTexCoord2f(14.0f, 5.0f); glVertex3f(  0.0f,  4.0f, 25.0f);
		glTexCoord2f( 0.0f, 5.0f); glVertex3f(-45.0f,  4.0f, 25.0f);
	glEnd();

	//Plano 22 (piso 6)
	glBindTexture(GL_TEXTURE_2D, textura[11].texID);

	glBegin(GL_QUADS);
		glNormal3f(0.0f,1.0f,0.0f);
		glTexCoord2f(0.0f, 3.0f); glVertex3f(-25.0f, 10.0f, 8.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(  0.0f, 10.0f, 8.0f);
		glTexCoord2f(2.0f, 0.0f); glVertex3f(  0.0f, 10.0f, 0.0f);
		glTexCoord2f(2.0f, 3.0f); glVertex3f(-25.0f, 10.0f, 0.0f);
	glEnd();

	//Plano 23 (piso 7)
	glBindTexture(GL_TEXTURE_2D, textura[11].texID);

	glBegin(GL_QUADS);
		glNormal3f(0.0f,1.0f,0.0f);
		glTexCoord2f(0.0f, 3.0f); glVertex3f(-15.0f, 10.0f, 12.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(  0.0f, 10.0f, 12.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(  0.0f, 10.0f,  8.0f);
		glTexCoord2f(1.0f, 3.0f); glVertex3f(-15.0f, 10.0f,  8.0f);
	glEnd();

	//Plano 24 (piso 8)
	glBindTexture(GL_TEXTURE_2D, textura[11].texID);

	glBegin(GL_QUADS);
		glNormal3f(0.0f,1.0f,0.0f);
		glTexCoord2f(0.0f, 3.0f); glVertex3f(-7.0f, 10.0f, 16.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f( 0.0f, 10.0f, 16.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f( 0.0f, 10.0f, 12.0f);
		glTexCoord2f(1.0f, 3.0f); glVertex3f(-7.0f, 10.0f, 12.0f);
	glEnd();

	//Plano 25 (piso 9)
	glBindTexture(GL_TEXTURE_2D, textura[11].texID);

	glBegin(GL_QUADS);
		glNormal3f(0.0f,1.0f,0.0f);
		glTexCoord2f(0.0f, 3.0f); glVertex3f(-8.0f, 12.0f, 8.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f( 0.0f, 12.0f, 8.0f);
		glTexCoord2f(2.0f, 0.0f); glVertex3f( 0.0f, 12.0f, 0.0f);
		glTexCoord2f(2.0f, 3.0f); glVertex3f(-8.0f, 12.0f, 0.0f);
	glEnd();

	//Plano 26 (pared 14)
	glBindTexture(GL_TEXTURE_2D, textura[9].texID);

	glBegin(GL_QUADS);
		glNormal3f(-1.0f,0.0f,0.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-25.0f,  8.0f, 0.0f);
		glTexCoord2f(2.0f, 0.0f); glVertex3f(-25.0f,  8.0f, 8.0f);
		glTexCoord2f(2.0f, 1.0f); glVertex3f(-25.0f, 10.0f, 8.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-25.0f, 10.0f, 0.0f);
	glEnd();

	//Plano 27 (pared 15)
	glBindTexture(GL_TEXTURE_2D, textura[9].texID);

	glBegin(GL_QUADS);
		glNormal3f(-1.0f,0.0f,0.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-15.0f,  8.0f,  8.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-15.0f,  8.0f, 12.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-15.0f, 10.0f, 12.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-15.0f, 10.0f,  8.0f);
	glEnd();

	//Plano 28 (pared 16)
	glBindTexture(GL_TEXTURE_2D, textura[9].texID);

	glBegin(GL_QUADS);
		glNormal3f(-1.0f,0.0f,0.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-7.0f,  8.0f, 12.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-7.0f,  8.0f, 16.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-7.0f, 10.0f, 16.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-7.0f, 10.0f, 12.0f);
	glEnd();

	//Plano 29 (pared 17)
	glBindTexture(GL_TEXTURE_2D, textura[9].texID);

	glBegin(GL_QUADS);
		glNormal3f(-1.0f,0.0f,0.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-8.0f, 10.0f, 0.0f);
		glTexCoord2f(2.0f, 0.0f); glVertex3f(-8.0f, 10.0f, 8.0f);
		glTexCoord2f(2.0f, 1.0f); glVertex3f(-8.0f, 12.0f, 8.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-8.0f, 12.0f, 0.0f);
	glEnd();

	//Plano 30 (pared 18)
	glBindTexture(GL_TEXTURE_2D, textura[10].texID);

	glBegin(GL_QUADS);
		glNormal3f(0.0f,0.0f,1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-7.0f,  8.0f, 16.0f);
		glTexCoord2f(2.0f, 0.0f); glVertex3f( 0.0f,  8.0f, 16.0f);
		glTexCoord2f(2.0f, 1.0f); glVertex3f( 0.0f, 10.0f, 16.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-7.0f, 10.0f, 16.0f);
	glEnd();

	//Plano 31 (pared 19)
	glBindTexture(GL_TEXTURE_2D, textura[10].texID);

	glBegin(GL_QUADS);
		glNormal3f(0.0f,0.0f,1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-15.0f,  8.0f, 12.0f);
		glTexCoord2f(3.0f, 0.0f); glVertex3f(- 7.0f,  8.0f, 12.0f);
		glTexCoord2f(3.0f, 1.0f); glVertex3f(- 7.0f, 10.0f, 12.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-15.0f, 10.0f, 12.0f);
	glEnd();

	//Plano 32 (pared 20)
	glBindTexture(GL_TEXTURE_2D, textura[10].texID);

	glBegin(GL_QUADS);
		glNormal3f(0.0f,0.0f,1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-25.0f,  8.0f, 8.0f);
		glTexCoord2f(4.0f, 0.0f); glVertex3f(-15.0f,  8.0f, 8.0f);
		glTexCoord2f(4.0f, 1.0f); glVertex3f(-15.0f, 10.0f, 8.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-25.0f, 10.0f, 8.0f);
	glEnd();

	//Plano 33 (pared 21)
	glBindTexture(GL_TEXTURE_2D, textura[10].texID);

	glBegin(GL_QUADS);
		glNormal3f(0.0f,0.0f,1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-8.0f, 10.0f, 8.0f);
		glTexCoord2f(2.0f, 0.0f); glVertex3f( 0.0f, 10.0f, 8.0f);
		glTexCoord2f(2.0f, 1.0f); glVertex3f( 0.0f, 12.0f, 8.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-8.0f, 12.0f, 8.0f);
	glEnd();

	//Plano 34 (pared 22)
	glBindTexture(GL_TEXTURE_2D, textura[14].texID);

	glBegin(GL_QUADS);
		glNormal3f(-1.0f,0.0f,0.0f);
		glTexCoord2f(0.6f, 0.0f); glVertex3f(0.0f, -20.0f,  0.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, -20.0f, 30.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f,  17.0f, 30.0f);
		glTexCoord2f(0.6f, 1.0f); glVertex3f(0.0f,  17.0f,  0.0f);
	glEnd();

	//Plano 35 (pared 23)
	glBindTexture(GL_TEXTURE_2D, textura[14].texID);

	glBegin(GL_QUADS);
		glNormal3f(0.0f,0.0f,1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f( 0.0f, -20.0f, 30.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(80.0f, -20.0f, 30.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(80.0f,  17.0f, 30.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f( 0.0f,  17.0f, 30.0f);
	glEnd();

	//Plano 36 (piso 10)
	glBindTexture(GL_TEXTURE_2D, textura[13].texID);

	glBegin(GL_QUADS);
		glNormal3f(0.0f,1.0f,0.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f( 0.0f, 17.0f, 30.0f);
		glTexCoord2f(6.0f, 0.0f); glVertex3f(80.0f, 17.0f, 30.0f);
		glTexCoord2f(6.0f, 2.0f); glVertex3f(80.0f, 17.0f,  0.0f);
		glTexCoord2f(0.0f, 2.0f); glVertex3f( 0.0f, 17.0f,  0.0f);
	glEnd();

	//Plano 37 (pared 24)
	glBindTexture(GL_TEXTURE_2D, textura[15].texID);

	glBegin(GL_QUADS);
		glNormal3f(0.0f,0.0f,1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(22.0f, 17.0f, 8.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(80.0f, 17.0f, 8.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(80.0f, 35.0f, 8.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(22.0f, 35.0f, 8.0f);
	glEnd();

	//Plano 38 (pared 25)
	glBindTexture(GL_TEXTURE_2D, textura[7].texID);

	glBegin(GL_QUADS);
		glNormal3f(-1.0f,0.0f,0.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(22.0f, 17.0f, 0.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(22.0f, 17.0f, 8.0f);
		glTexCoord2f(1.0f, 2.0f); glVertex3f(22.0f, 35.0f, 8.0f);
		glTexCoord2f(0.0f, 2.0f); glVertex3f(22.0f, 35.0f, 0.0f);
	glEnd();

	//Plano 39 (piso 11)
	glBindTexture(GL_TEXTURE_2D, textura[1].texID);

	glBegin(GL_QUADS);
		glNormal3f(0.0f,1.0f,0.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(22.0f, 35.0f, 8.0f);
		glTexCoord2f(2.5f, 0.0f); glVertex3f(80.0f, 35.0f, 8.0f);
		glTexCoord2f(2.5f, 1.0f); glVertex3f(80.0f, 35.0f, 0.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(22.0f, 35.0f, 0.0f);
	glEnd();

	//Plano 40 (pared 26)
	glBindTexture(GL_TEXTURE_2D, textura[7].texID);

	glBegin(GL_QUADS);
		glNormal3f(-1.0f,0.0f,0.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(80.0f, 35.0f, 0.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(80.0f, 35.0f, 8.0f);
		glTexCoord2f(1.0f, 2.0f); glVertex3f(80.0f, 50.0f, 8.0f);
		glTexCoord2f(0.0f, 2.0f); glVertex3f(80.0f, 50.0f, 0.0f);
	glEnd();

	//Plano 41 (pared 27)
	glBindTexture(GL_TEXTURE_2D, textura[8].texID);

	glBegin(GL_QUADS);
		glNormal3f(0.0f,0.0f,1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f( 0.0f, 17.0f, 0.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(80.0f, 17.0f, 0.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(80.0f, 50.0f, 0.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f( 0.0f, 50.0f, 0.0f);
	glEnd();

	//Plano 42 (pared 28)
	glBindTexture(GL_TEXTURE_2D, textura[7].texID);

	glBegin(GL_QUADS);
		glNormal3f(-1.0f,0.0f,0.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, 30.0f, -10.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(0.0f, 30.0f,   0.0f);
		glTexCoord2f(1.0f, 2.0f); glVertex3f(0.0f, 50.0f,   0.0f);
		glTexCoord2f(0.0f, 2.0f); glVertex3f(0.0f, 50.0f, -10.0f);
	glEnd();

	//Plano 43 (pared 29)
	glBindTexture(GL_TEXTURE_2D, textura[16].texID);

	glBegin(GL_QUADS);
		glNormal3f(-1.0f,0.0f,0.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(80.0f, 17.0f,  8.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(80.0f, 17.0f, 30.0f);
		glTexCoord2f(1.0f, 0.7f); glVertex3f(80.0f, 50.0f, 30.0f);
		glTexCoord2f(0.0f, 0.7f); glVertex3f(80.0f, 50.0f,  8.0f);
	glEnd();

	glDisable(GL_TEXTURE_2D);

	//Cajas

	//Caja 1
	glPushMatrix();
		glTranslatef(8.9f, 19.0f, 3.5f);
		DibujaCaja(2);
	glPopMatrix();

	//Caja 2
	glPushMatrix();
		glTranslatef(14.1f, 19.0f, 3.0f);
		DibujaCaja(1);
	glPopMatrix();

	//Caja 3
	glPushMatrix();
		glTranslatef(19.3f, 19.0f, 3.5f);
		DibujaCaja(3);
	glPopMatrix();

	//Caja 4
	glPushMatrix();
		glTranslatef(9.0f, 23.0f, 3.5f);
		DibujaCaja(1);
	glPopMatrix();

	//Caja 5
	glPushMatrix();
		glTranslatef(17.0f, 23.0f, 3.5f);
		glRotatef(-70.0f, 0.0f, 1.0f, 0.0f);
		glRotatef(-90.0f, 0.0f, 0.0f, 1.0f);
		glScalef(0.8f, 1.0f, 1.2f);
		DibujaCaja(1);
	glPopMatrix();

	//Caja 6
	glPushMatrix();
		glTranslatef(14.3f, 27.0f, 3.0f);
		DibujaCaja(3);
	glPopMatrix();

	//Caja 7
	glPushMatrix();
		glTranslatef(19.4f, 27.0f, 3.0f);
		DibujaCaja(3);
	glPopMatrix();

	//Caja 8
	glPushMatrix();
		glTranslatef(19.4f, 31.0f, 3.0f);
		DibujaCaja(2);
	glPopMatrix();

	//Caja 9
	glPushMatrix();
		glTranslatef(67.4f, 19.0f, 11.5f);
		glScalef(1.0f, 1.0f, 0.8f);
		DibujaCaja(2);
	glPopMatrix();

	//Caja 10
	glPushMatrix();
		glTranslatef(72.5f, 19.0f, 11.0f);
		glScalef(1.0f, 1.0f, 0.8f);
		DibujaCaja(1);
	glPopMatrix();

	//Caja 11
	glPushMatrix();
		glTranslatef(77.5f, 19.0f, 11.5f);
		glScalef(1.0f, 1.0f, 0.8f);
		DibujaCaja(3);
	glPopMatrix();

	//Caja 12
	glPushMatrix();
		glTranslatef(72.5f, 23.0f, 11.0f);
		glScalef(1.0f, 1.0f, 0.8f);
		DibujaCaja(1);
	glPopMatrix();

	//Caja 13
	glPushMatrix();
		glTranslatef(77.5f, 23.0f, 11.5f);
		glScalef(1.0f, 1.0f, 0.8f);
		DibujaCaja(1);
	glPopMatrix();

	//Caja 14
	glPushMatrix();
		glTranslatef(77.5f, 27.0f, 11.5f);
		glScalef(1.0f, 1.0f, 0.8f);
		DibujaCaja(3);
	glPopMatrix();

	//Banda transportadora
	glPushMatrix();
		glTranslatef(71.0f, 18.5f, 19.0f);
		DibujaTransportadora();
	glPopMatrix();
}

void DibujaPersonaje()
{
	glPushMatrix();

		//glTranslatef(player1.PosicionObj.x+Xtor, player1.PosicionObj.y+Ytor, player1.PosicionObj.z+Ztor);
		glTranslatef(Xtor, Ytor, Ztor);
		glRotatef(Angt2, 0.0f, 1.0f, 0.0f);
		glRotatef(Angt1, 1.0f, 0.0f, 0.0f);
		
		//Torso
		g_Load3ds.Render3DSFile(&g_3DModel1c, textureModel1c, 1);

		//Cabeza
		glPushMatrix();
			glTranslatef(0.0f, 1.9f ,0.0f);
			glRotatef(Angc2, 0.0f, 1.0f, 0.0f);
			glRotatef(Angc1, 1.0f, 0.0f, 0.0f);
			g_Load3ds.Render3DSFile(&g_3DModel2c, textureModel2c, 1);
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

void AnimacionSalto()
{
	if(player1.contAuxAnim < 40)
	{
		if(play == false)
		{
			Angt1   = KeyFrame2[0].Angt1;
			Angt2   = KeyFrame2[0].Angt2;
			Angc1   = KeyFrame2[0].Angc1;
			Angc2   = KeyFrame2[0].Angc2;
			Angbi1  = KeyFrame2[0].Angbi1;
			Angbi2  = KeyFrame2[0].Angbi2;
			Angbib  = KeyFrame2[0].Angbib;
			Angbd1  = KeyFrame2[0].Angbd1;
			Angbd2  = KeyFrame2[0].Angbd2;
			Angbdb  = KeyFrame2[0].Angbdb;
			Angpizq = KeyFrame2[0].Angpizq;
			Angpder = KeyFrame2[0].Angpder;
			Angpi   = KeyFrame2[0].Angpi;
			Angpd   = KeyFrame2[0].Angpd;
			Xtor    = KeyFrame2[0].Xtor;
			Ytor    = KeyFrame2[0].Ytor;
			Ztor    = KeyFrame2[0].Ztor;

			play=true;
			playIndex=0;
			tipoAnim=2;
		}
		player1.contAuxAnim++;
	}
	else
	{
		player1.kick=false;
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

void IndicadorVidas()
{
	glDisable(GL_DEPTH_TEST);							// Desactiva prueba de profundidad
	glMatrixMode(GL_PROJECTION);						// Selecciona la matriz de proyeccion
	glPushMatrix();										// Guarda la matriz de proyeccion
	glLoadIdentity();									// Limpia la matriz de proyeccion
	glOrtho(0,glWidth,0,glHeight,-1,1);					// Crea una proyección paralela
	glMatrixMode(GL_MODELVIEW);							// Selecciona la matriz de modelo de vista
	glPushMatrix();										// Guarda matriz de modelo de vista
	glLoadIdentity();

	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);

	glAlphaFunc(GL_GREATER, 0.8f);
	glEnable(GL_ALPHA_TEST);

	glBindTexture(GL_TEXTURE_2D, textura[29].texID);

	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex2f(        0.0f,          0.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex2f(glWidth*0.2f,          0.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex2f(glWidth*0.2f, glHeight*0.3f);
		glTexCoord2f(0.0f, 1.0f); glVertex2f(        0.0f, glHeight*0.3f);
	glEnd();

	glDisable(GL_ALPHA_TEST);

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	
	glMatrixMode(GL_PROJECTION);						// Selecciona la matriz de proyeccion
	glPopMatrix();										// Restaura la matriz de proyeccion anterior
	glMatrixMode(GL_MODELVIEW);							// Selecciona la matriz de modelo de vista
	glPopMatrix();										// Restaura la matriz de modelo de vista anterior
	glEnable(GL_DEPTH_TEST);							// Activa prueba de profundidad
}

//Cambios para Fuentes
void DibujaTextos()
{
	glDisable(GL_DEPTH_TEST);							// Desactiva la prueba de profundidad
	glMatrixMode(GL_PROJECTION);						// Selecciona la matriz de proyección
	glPushMatrix();										// Guarda la matriz de proyección
	glLoadIdentity();									// Limpia la matriz de proyección
	glOrtho(0,glWidth,0,glHeight,-1,1);					// Crea una proyección ortogonal
	glMatrixMode(GL_MODELVIEW);							// Selecciona la matriz de modelo de vista
	glPushMatrix();										// Guarda la matriz de modelo de vista
	
	glDisable(GL_LIGHTING);
	glLoadIdentity();

	glColor3f(1.0f,1.0f,1.0f);
	
	glEnable(GL_TEXTURE_2D);

	glAlphaFunc(GL_GREATER, 0.6f);
	glEnable(GL_ALPHA_TEST);

	//Font.glPrint((2.0f/640.0f)*glWidth, glWidth*0.05f,glHeight*0.9f,"Ejemplo texto");
	Font.glPrint((0.8f/640.0f)*glWidth, glWidth*0.85f,glHeight*0.95f,".FPS %d",FPS);
	
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_TEXTURE_2D);

	glColor3f(1.0f,1.0f,1.0f);

	glEnable(GL_LIGHTING);
	glMatrixMode(GL_PROJECTION);						// Selecciona la matriz de proyección
	glPopMatrix();										// Recupera la anterior matriz de proyección
	glMatrixMode(GL_MODELVIEW);							// Selecciona la matriz de modelo de vista
	glPopMatrix();										// Recupera la anterior matriz de modelo de vista
	glEnable(GL_DEPTH_TEST);							// Activa la prueba de profundidad
}

int RenderizaEscena(GLvoid)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
		
	//gluLookAt(-100.0f, 30.0f, 90.0f, -30.0f, 25.0f, 0.0f, 0, 1, 0);
	gluLookAt(player1.PosicionObj.x-10.0f, player1.PosicionObj.y+30.0f, player1.PosicionObj.z+40.0f, 
			  player1.PosicionObj.x, player1.PosicionObj.y+18.0f, player1.PosicionObj.z, 
			  0.0f, 1.0f, 0.0f);

	//AlturaPiso();
					
	ActualizaLuzParam();
	
	if(player1.kick == true)
		AnimacionSalto();

	DibujaEscenario();

	if(player1.visible == true)
	{
		glPushMatrix();
			glTranslatef(player1.PosicionObj.x, player1.PosicionObj.y+altPiso+1.3f, player1.PosicionObj.z);
			glRotatef(player1.AngObj, 0.0f, 1.0f, 0.0f);
			glScalef(player1.escalaX,player1.escalaY,player1.escalaZ);
			DibujaPersonaje();
		glPopMatrix();
	}

	IndicadorVidas();
	DibujaTextos();

	//Cambios para FPS
	CalculateFrameRate();

	return TRUE;
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
		0,											// No Stencil Buffer
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
	BOOL	done=FALSE;								// Variable booleana para salir del ciclo

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
					done=TRUE;						// ESC indica el termino del programa
				}
				else								// De lo contrario, actualiza la pantalla
				{
					RenderizaEscena();				// Dibuja la escena
					if(play)
					{
						if(tipoAnim == 1) //caminar
							animacion(KeyFrame1, maxKF1 , 15);
						else if(tipoAnim == 2) //hurricane kick
							animacion(KeyFrame2, maxKF2 , 3);
					}
					SwapBuffers(hDC);				// Intercambia los Buffers (Double Buffering)
				}

				//if(!ManejaTeclado()) return 0;
				controlFunc.actualizaControl();
				controlEventos();
			}
			
		}
	}

	// Finalización del programa
	DescargaTexturas();
	DescargaModelos();
	//Cambios para Fuentes
	Font.DestroyFont();
	DestruyeVentanaOGL();							// Destruye la ventana
	return (msg.wParam);							// Sale del programa
}

void controlEventos()
{
	if(controlFunc.obtieneEstadoTecla(3) == 1)
	{
		if(player1.kick==false)
		{
			ControlPersonaje2(1);
			
			if(play==false && player1.kick==false) //Para que la asignación de valores siguiente solo se haga una vez y empiece la animación
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

	if(controlFunc.obtieneEstadoTecla(2) == 1)
	{
		if(player1.kick==false)
		{
			ControlPersonaje2(2);
		
			if(play==false && player1.kick==false) //Para que la asignación de valores siguiente solo se haga una vez y empiece la animación
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

	if(controlFunc.obtieneEstadoTecla(2) == 0 && controlFunc.obtieneEstadoTecla(3) == 0
		&& controlFunc.obtieneEstadoTecla(0) == 0 && controlFunc.obtieneEstadoTecla(1) == 0)
	{
		if(tipoAnim == 1)
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

	if(controlFunc.obtieneEstadoTecla(4) == 1)
	{
		if(player1.kick == false) //Para que la asignación de valores siguiente solo se haga una vez y empiece la animación
		{
			//Se le asignan a las variables del personaje los 
			//valores almacenados en el primer keyframe para que
			//inicie desde ahí la animación.
			Angt1   = KeyFrame2[0].Angt1;
			Angt2   = KeyFrame2[0].Angt2;
			Angc1   = KeyFrame2[0].Angc1;
			Angc2   = KeyFrame2[0].Angc2;
			Angbi1  = KeyFrame2[0].Angbi1;
			Angbi2  = KeyFrame2[0].Angbi2;
			Angbib  = KeyFrame2[0].Angbib;
			Angbd1  = KeyFrame2[0].Angbd1;
			Angbd2  = KeyFrame2[0].Angbd2;
			Angbdb  = KeyFrame2[0].Angbdb;
			Angpizq = KeyFrame2[0].Angpizq;
			Angpder = KeyFrame2[0].Angpder;
			Angpi   = KeyFrame2[0].Angpi;
			Angpd   = KeyFrame2[0].Angpd;
			Xtor    = KeyFrame2[0].Xtor;
			Ytor    = KeyFrame2[0].Ytor;
			Ztor    = KeyFrame2[0].Ztor;

			play=true;
			playIndex=0;
			tipoAnim=2;

			player1.kick=true;
			player1.contAuxAnim=0;
		}
	}
}

int ManejaTeclado()
{
	//Al presionar cualquiera de las teclas de dirección se iniciará la animación de caminar
	//Al soltar la tecla presionada se detiene la animación.
	if(GetKeyState(VK_UP) & 0x80) //Si está presionada la tecla UP
	{
		if(player1.kick==false)
		{
			ControlPersonaje2(3);
			
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

	if(GetKeyState(VK_DOWN) & 0x80) //Si está presionada la tecla DOWN
	{
		if(player1.kick==false)
		{
			ControlPersonaje2(4);
			
			if(play==false && player1.kick==false) //Para que la asignación de valores siguiente solo se haga una vez y empiece la animación
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

	if(GetKeyState(VK_LEFT) & 0x80) //Si está presionada la tecla LEFT
	{
		if(player1.kick==false)
		{
			ControlPersonaje2(2);
		
			if(play==false && player1.kick==false) //Para que la asignación de valores siguiente solo se haga una vez y empiece la animación
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

	if(GetKeyState(VK_RIGHT) & 0x80) //Si está presionada la tecla RIGHT
	{
		if(player1.kick==false)
		{
			ControlPersonaje2(1);
			
			if(play==false && player1.kick==false) //Para que la asignación de valores siguiente solo se haga una vez y empiece la animación
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

	if((GetAsyncKeyState('Z')&1) == 1)
	{
		if(player1.kick == false) //Para que la asignación de valores siguiente solo se haga una vez y empiece la animación
		{
			//Se le asignan a las variables del personaje los 
			//valores almacenados en el primer keyframe para que
			//inicie desde ahí la animación.
			Angt1   = KeyFrame2[0].Angt1;
			Angt2   = KeyFrame2[0].Angt2;
			Angc1   = KeyFrame2[0].Angc1;
			Angc2   = KeyFrame2[0].Angc2;
			Angbi1  = KeyFrame2[0].Angbi1;
			Angbi2  = KeyFrame2[0].Angbi2;
			Angbib  = KeyFrame2[0].Angbib;
			Angbd1  = KeyFrame2[0].Angbd1;
			Angbd2  = KeyFrame2[0].Angbd2;
			Angbdb  = KeyFrame2[0].Angbdb;
			Angpizq = KeyFrame2[0].Angpizq;
			Angpder = KeyFrame2[0].Angpder;
			Angpi   = KeyFrame2[0].Angpi;
			Angpd   = KeyFrame2[0].Angpd;
			Xtor    = KeyFrame2[0].Xtor;
			Ytor    = KeyFrame2[0].Ytor;
			Ztor    = KeyFrame2[0].Ztor;

			play=true;
			playIndex=0;
			tipoAnim=2;

			player1.kick=true;
			player1.contAuxAnim=0;
		}
	}

	//Para que al soltar la tecla presionada el personaje no quede en una posición
	//intermedia de la animación se asignan los valores originales a las variables
	if(!(GetKeyState(VK_UP) & 0x80 || GetKeyState(VK_DOWN) & 0x80
	|| GetKeyState(VK_LEFT) & 0x80  || GetKeyState(VK_RIGHT) & 0x80)) //Si no está presionada alguna de esas teclas
	{
		if(tipoAnim == 1)
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

	if(keys[VK_HOME])
	{
		ControlPersonaje(5);
	}

	if(keys[VK_END])
	{
		ControlPersonaje(6);
	}

	if(keys['A'])
	{
		
	}
	if(keys['S'])
	{
		
	}
	

	return TRUE;
}