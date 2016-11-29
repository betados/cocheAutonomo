/**
Recibe la imagen, la analiza y reenvia por socket los datos 
primer parametro: ip servidor
segundo: puerto de la comunicacion socket
tercer: ancho resolucion captura
 */

//pkg-config --libs --cflags opencv

//opencv
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>
#include <stdio.h>

//socket?dummy=param.mjpeg
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 


using namespace std;
using namespace cv;

/** Function Headers */
int detectAndDisplay(  Mat frame, int *jx, int *jTamano );
void error(const char *msg);


/** Global variables */


//-- Note, either copy these two files from opencv/data/haarscascades to your current folder, or change these locations
String face_cascade_name = "haar/lbpcascade_frontalface.xml";
//String eyes_cascade_name = "haarcascade_eye_tree_eyeglasses.xml";
CascadeClassifier face_cascade;
CascadeClassifier eyes_cascade;
string window_name = "Capture - Face detection";
RNG rng(12345);

/**
 * @function main
 */
int main( int argc, char *argv[] )
{

  int x=0;
  int pruebaX=0,pruebaTamano=0;
  int *jx = &pruebaX; int *jTamano=&pruebaTamano;
  String ipVideo = "http://";
  //String ipVideo = "http://192.168.1.55:8080/?action=stream?dummy=param.jpeg";
   int resolucion  = atoi(argv[3]);
  //String resolucion = "ancho ";
  ipVideo.append( argv[1]);//////////////////////
  //resolucion.append( argv[3]);
  cout<<ipVideo<<endl;
  ipVideo.append(":8080/?action=stream?dummy=param.mjpeg");///////////////
  cout<<ipVideo<<endl;
  VideoCapture capture(ipVideo);
  
 
	//VideoCapture capture( "http://192.168.1.51:8080/stream_simple.html");
  Mat frame;

	//////////////////////////////////////////////////////////////////////////////abrir socket////////////////////////
	int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    
    char limpia[20];
    strcpy(limpia, "limpia ");
    char equis[20];
    char cadena [9];
    char tamano[20];


    if (argc < 4) {
       fprintf(stderr,"usage %s ip_hostname puerto resolucion\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


  //-- 1. Load the cascades
  if( !face_cascade.load( face_cascade_name ) ){ printf("--(!)Error loading\n"); return -1; };
  //if( !eyes_cascade.load( eyes_cascade_name ) ){ printf("--(!)Error loading\n"); return -1; };

  //-- 2. Read the video stream
  int  contador=0;
   capture.open( ipVideo);
 if (resolucion == 320) {
    capture.set(CV_CAP_PROP_FRAME_WIDTH,320);
    capture.set(CV_CAP_PROP_FRAME_HEIGHT,240);
    cout<<"RESOLUCION"<<endl;
  }

   
   /*do{
     capture.open( ipVideo);
     //capture.set(CV_CAP_PROP_FRAME_WIDTH,320);
     //capture.set(CV_CAP_PROP_FRAME_HEIGHT,240);
     contador++;
     cout<<contador<<endl;
     if (contador>999){
     cout<<"Esto no arranca"<<endl;
     break;
  }
  	
  }while(!capture.isOpened());*/

   
//capture.open( "http://192.168.1.51:8080/stream_simple.html");

  //capture.set(CV_CAP_PROP_FRAME_WIDTH,320);
  //capture.set(CV_CAP_PROP_FRAME_HEIGHT,240);

  if( capture.isOpened() )
  {
    for(;;)
    {

      capture.read(frame);
      resize(frame, frame, Size(320, 240), 0, 0, INTER_CUBIC);
	//capture >> frame;

      //-- 3. Apply the classifier to the frame
      if( !frame.empty() )
		{
		  //cvtColor( frame, frame, COLOR_BGR2GRAY );
		  x = detectAndDisplay( frame,jx ,jTamano  );
		  if (x==9999) *jTamano=9999;
			//if (x!=9999)			  {
			    ////////////////////////////////////////////////////////////////////////////////////
			    //mensaje espurio para reiniciar parseInt() en arduino
			    
			    n = write(sockfd,limpia,strlen(limpia));
			    printf("%s\n", limpia);
			    if (n < 0) 
			      error("ERROR escribiendo el mensaje espureo");
			    //bzero(limpia,256);
			    //cout<<*jx<<" , "<<*jTamano<<endl;
			    //printf("%d , %d\n",*jx, *jTamano);
			


			    //////////////////añade el dato de la x a continuacion de dato/////////////////////////////////////////
			    try///creo que no hace falta esta excepcion, no era aqui donde fallaba 
			      {
				bzero(equis,256);
				strcpy(equis, "equis    ");				
				snprintf(cadena, sizeof(cadena), "%d", x);//creo que transforma x en una cadena
				//printf("x: %s\n", cadena);			
				for (int i =0; i<9; i++)
				  {
				    equis[6+i] = cadena[i];
				  }
				printf("%s\n", equis);
				n = write(sockfd,equis,strlen(equis));
				if (n < 0) 
				  error("ERROR writing to socket");
			      }
			    catch (exception& e)
			      {
				cout << e.what() << '\n';
			      }

			    n = write(sockfd,limpia,strlen(limpia));
			    printf("%s\n", limpia);
			    if (n < 0)  error("ERROR escribiendo el mensaje espureo");

			    bzero(tamano,256);
			    strcpy(tamano, "tamano    ");				
			    snprintf(cadena, sizeof(cadena), "%d", *jTamano);//creo que transforma x en una cadena
			    //printf("x: %s\n", cadena);			
			    for (int i =0; i<9; i++)
			      {
				tamano[7+i] = cadena[i];
			      }
			    printf("%s\n", tamano);
			    n = write(sockfd,tamano,strlen(tamano));
			    if (n < 0) 
			      error("ERROR writing to socket");

			    char ancho [9];
			    if ( resolucion==320) strcpy(ancho, "ancho 320");
			    else strcpy(ancho, "ancho 640");
			     n = write(sockfd,ancho,strlen(ancho));
			    if (n < 0) 
			      error("ERROR writing to socket");

			    
			    // }
			//////////////////////////////////////////////////////////////////////////////////////////
    
		}
      else
       { printf(" --(!) No captured frame -- Break!"); break; }

      int c = waitKey(1);
      if( (char)c == 'c'|| c == 27 ) { break; }

    }
	
  }
  else printf("mierda pa ti\n");
	

////cierra socket
    close(sockfd);
    //cvReleaseCapture();
  return 0;
}

/**
 * @function detectAndDisplay
 */
int detectAndDisplay( Mat frame, int *jx, int *jTamano )
{
   std::vector<Rect> faces;
   Mat frame_gray;

   cvtColor( frame, frame_gray, COLOR_BGR2GRAY );
   equalizeHist( frame_gray, frame_gray );
   //-- Detect faces
   face_cascade.detectMultiScale( frame_gray, faces, 1.1, 5, 0|CV_HAAR_SCALE_IMAGE, Size(40, 40) );

   for( size_t i = 0; i < faces.size(); i++ )
    {
      Point center( faces[i].x + faces[i].width/2, faces[i].y + faces[i].height/2 );
      ellipse( frame, center, Size( faces[i].width/2, faces[i].height/2), 0, 0, 360, Scalar( 255, 0, 255 ), 2, 8, 0 );

	

      //Mat faceROI = frame_gray( faces[i] );
      //std::vector<Rect> eyes;

/*
      //-- In each face, detect eyes
      eyes_cascade.detectMultiScale( faceROI, eyes, 1.1, 2, 0 |CV_HAAR_SCALE_IMAGE, Size(30, 30) );
      for( size_t j = 0; j < eyes.size(); j++ )
       {
         Point eye_center( faces[i].x + eyes[j].x + eyes[j].width/2, faces[i].y + eyes[j].y + eyes[j].height/2 );
         int radius = cvRound( (eyes[j].width + eyes[j].height)*0.25 );
         circle( frame, eye_center, radius, Scalar( 255, 0, 0 ), 3, 8, 0 );
       }
*/
    }
	
   //-- Show what you got
   imshow( window_name, frame );
   if ( faces.size()>0){
     
     //int xProvisional= faces[0].x + faces[0].width/2;
     *jx= faces[0].x + faces[0].width/2; //xProvisional;
     *jTamano = faces[0].width/2;
     return  faces[0].x + faces[0].width/2;
   }
   else return 9999;
   
    //if (faces.size()>0)*/// enviarYun( faces[0].x + faces[0].width/2);
}


void error(const char *msg)
{
    perror(msg);
    exit(0);
}
