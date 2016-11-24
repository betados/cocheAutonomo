/**

Recibe la imagen, la analiza y reenvia por socket los datos 
 */

//pkg-config --libs --cflags opencv

//opencv
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>
#include <stdio.h>

//socket
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
int detectAndDisplay( Mat frame );
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

  int x;
  VideoCapture capture( "http://192.168.240.1:8080/?action=stream");
	//VideoCapture capture( "http://192.168.1.51:8080/stream_simple.html");
  Mat frame;

	//////////////////////////////////////////////////////////////////////////////abrir socket////////////////////////
	int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    
    char buffer[20];
    char nuevo[20];


    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
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
do{
  capture.open( "http://192.168.240.1:8080/?action=stream");
	
}while(!capture.isOpened());
//capture.open( "http://192.168.1.51:8080/stream_simple.html");

  //capture.set(CV_CAP_PROP_FRAME_WIDTH,320);
  //capture.set(CV_CAP_PROP_FRAME_HEIGHT,240);

  if( capture.isOpened() )
  {
    for(;;)
    {
      capture >> frame;

      //-- 3. Apply the classifier to the frame
      if( !frame.empty() )
		{ 
			x = detectAndDisplay( frame );
			//if (x!=9999)			  {
			    ////////////////////////////////////////////////////////////////////////////////////
			    //mensaje espurio para reiniciar parseInt() en arduino
			    strcpy(buffer, "espurio ");
			    n = write(sockfd,buffer,strlen(buffer));
			    printf("%s\n", buffer);
			    if (n < 0) 
			      error("ERROR escribiendo el mensaje espureo");
			    bzero(buffer,256);
			


			    //////////////////añade el dato de la x a continuacion de dato/////////////////////////////////////////
			    try///creo que no hace falta esta excepcion, no era aqui donde fallaba 
			      {
				bzero(nuevo,256);
				strcpy(nuevo, "dato    ");
				char cadena [9];
				snprintf(cadena, sizeof(cadena), "%d", x);
				//printf("x: %s\n", cadena);			
				for (int i =0; i<9; i++)
				  {
				    nuevo[5+i] = cadena[i];
				  }
				printf("%s\n", nuevo);
				n = write(sockfd,nuevo,strlen(nuevo));
				if (n < 0) 
				  error("ERROR writing to socket");
			      }
			    catch (exception& e)
			      {
				cout << e.what() << '\n';
			      }
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
  return 0;
}

/**
 * @function detectAndDisplay
 */
int detectAndDisplay( Mat frame )
{
   std::vector<Rect> faces;
   Mat frame_gray;

   cvtColor( frame, frame_gray, COLOR_BGR2GRAY );
   equalizeHist( frame_gray, frame_gray );
   //-- Detect faces
   face_cascade.detectMultiScale( frame_gray, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(80, 80) );

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
   if ( faces.size()>0)    return  faces[0].x + faces[0].width/2;
   else return 9999;
   
    //if (faces.size()>0)*/// enviarYun( faces[0].x + faces[0].width/2);
}


void error(const char *msg)
{
    perror(msg);
    exit(0);
}
