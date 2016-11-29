/**

Recibe la imagen, la analiza y reenvia por socket los datos 
primer parametro: ip servidor
segundo: puerto de la comunicacion socket
tercer: ancho resolucion captura
 */

//pkg-config --libs --cflags opencv

//opencv
//#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>
#include <stdio.h>
#include <sstream>

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
//int detectAndDisplay(  Mat frame, int *jx, int *jTamano );
void error(const char *msg);


void drawAxis(Mat&, Point, Point, Scalar, const float);
int getOrientation(const vector<Point> &, Mat&);
void MyPolygon( Mat );


/** Global variables */


string window_name = "Capture - Face detection";

/**
 * @function main
 */
int main( int argc, char *argv[] )
{

  int x=0;
  //int pruebaX=0,pruebaTamano=0;
  //int *jx = &pruebaX; int *jTamano=&pruebaTamano;
  String ipVideo = "http://";
  ipVideo.append( argv[1]);
  cout<<ipVideo<<endl;
  ipVideo.append(":8080/?action=stream?dummy=param.mjpeg");///////////////
  cout<<ipVideo<<endl;
  VideoCapture capture(ipVideo);

   int resolucion  = atoi(argv[3]);
  
 
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

int vecinos=361;
			int C=11;

  //-- 2. Read the video stream
  
   capture.open( ipVideo);
	/*
 if (resolucion == 320) {
    capture.set(CV_CAP_PROP_FRAME_WIDTH,320);
    capture.set(CV_CAP_PROP_FRAME_HEIGHT,240);
    cout<<"RESOLUCION"<<endl;
  }*/


  if( capture.isOpened() )
  {
    for(;;)
    {

		capture.read(frame);
		MyPolygon( frame);
      //if  (resolucion == 320) resize(frame, frame, Size(320, 240), 0, 0, INTER_CUBIC);
	//capture >> frame;

      //-- 3. Apply the classifier to the frame
      if( !frame.empty() )
		{
		  //cvtColor( frame, frame, COLOR_BGR2GRAY );
		  //x = detectAndDisplay( frame,jx ,jTamano  );
		  //if (x==9999) *jTamano=9999;
			
			
			
			
			//////////////////////////////////////contorno y direccion///////////////////////////////////////////////
			
			cvtColor(frame, frame, COLOR_BGR2GRAY);
			// Convert image to binary
			Mat bw;
			//threshold(frame, bw, 50, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
			
	adaptiveThreshold(frame, bw, 	 255, 
					   ADAPTIVE_THRESH_MEAN_C
					 //ADAPTIVE_THRESH_GAUSSIAN_C
					  , THRESH_BINARY,vecinos, C); //funciona mejor con papel marron, claro. Pero tambien se confunde con el parquet
			// Find all the contours in the thresholded image
			int l = waitKey(1);
      		if( (char)l == 'q' ) vecinos+=2;
      		if( (char)l == 'a' ) vecinos-=2;
      		if( (char)l == 'w' ) C++;
      		if( (char)l == 's' ) C--;
			
			//cout<<"vecinos: "<<vecinos<<endl;
			//cout<<"C: "<<C<<endl;
			
			
			vector<Vec4i> hierarchy;
			vector<vector<Point> > contours;
			findContours(bw, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
			
			double area/*[9999]*/;	
			double perimetro/*[9999]*/;	
			float relacion[9999];	
			int indiceRelacionMejor=1;
			for (size_t i = 0; i < contours.size(); ++i){
				
				area = contourArea(contours[i]);
				
				
				// Ignore contours that are too small or too large
				if (area < 32e2 || 1e5 < area) continue;
				
				perimetro= arcLength(contours[i],1/*cerrado*/);
				
				relacion[i] = perimetro/area;
				
				drawContours(frame, contours, static_cast<int>(i), Scalar(255, 255, 255), 2, 8, hierarchy, 0);
				//x = getOrientation(contours[i], frame);
				
				
				//convertir a string
				string Result;          // string which will contain the result
				ostringstream convert;   // stream used for the conversion
				convert << relacion[i];      // insert the textual representation of 'Number' in the characters in the stream
				Result = convert.str(); // set 'Result' to the contents of the stream
				//putText( frame, Result, Point(x,400), FONT_HERSHEY_SIMPLEX, 0.5, Scalar( 0, 0, 0 ), 1, 8, false );
				
				//convert.flush();
				//Result.clear();
				convert << " ";
				convert << x;      // insert the textual representation of 'Number' in the characters in the stream
				Result = convert.str(); // set 'Result' to the contents of the stream
				putText( frame, Result, Point(x,415), FONT_HERSHEY_SIMPLEX, 0.5, Scalar( 0, 0, 0 ), 1, 8, false );
				
				if (relacion[i]>relacion[indiceRelacionMejor]) indiceRelacionMejor=i;
				//if ((relacion[i] < 0.05 || 0.5 < relacion[i]) && ((relacion[i]>relacion[indiceRelacionMejor]) || (relacion[indiceRelacionMejor] < 0.05 || 0.5 < relacion[indiceRelacionMejor]) )) indiceRelacionMejor=i;
				
			}
			//cout<<indiceRelacionMejor<<endl;
			x = getOrientation(contours[indiceRelacionMejor], frame);
			//cout<< x <<endl;
			imshow("imagen", frame);
			//imshow("BW", bw);
				
			///////////////////////////////////////////////////////////////////////////////////////////////////////
			
			
			
			
			
			//if (x!=9999)			  {
			    ////////////////////////////////////////////////////////////////////////////////////
			    //mensaje espurio para reiniciar parseInt() en arduino
			    
			    n = write(sockfd,limpia,strlen(limpia));
			    //printf("%s\n", limpia);
			    if (n < 0) 
			      error("ERROR escribiendo el mensaje espureo");
			


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
				//printf("%s\n", equis);
				n = write(sockfd,equis,strlen(equis));
				if (n < 0) 
				  error("ERROR writing to socket");
			      }
			    catch (exception& e)
			      {
					cout << e.what() << '\n';
			      }

			    n = write(sockfd,limpia,strlen(limpia));
			    //printf("%s\n", limpia);
			    if (n < 0)  error("ERROR escribiendo el mensaje espureo");

			/*
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
			      error("ERROR writing to socket");*/

			    char ancho [9];
			    if ( frame.cols==320) strcpy(ancho, "ancho 320");
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
   

    	
   //-- Show what you got
   imshow( window_name, frame );

     return  faces[0].x + faces[0].width/2;

   
    //if (faces.size()>0)*/// enviarYun( faces[0].x + faces[0].width/2);
}

void drawAxis(Mat& img, Point p, Point q, Scalar colour, const float scale = 0.2)
{
    double angle;
    double hypotenuse;
    angle = atan2( (double) p.y - q.y, (double) p.x - q.x ); // angle in radians
    hypotenuse = sqrt( (double) (p.y - q.y) * (p.y - q.y) + (p.x - q.x) * (p.x - q.x));
//    double degrees = angle * 180 / CV_PI; // convert radians to degrees (0-180 range)
//    cout << "Degrees: " << abs(degrees - 180) << endl; // angle in 0-360 degrees range
    // Here we lengthen the arrow by a factor of scale
    q.x = (int) (p.x - scale * hypotenuse * cos(angle));
    q.y = (int) (p.y - scale * hypotenuse * sin(angle));
    line(img, p, q, colour, 1, CV_AA);
    // create the arrow hooks
    p.x = (int) (q.x + 9 * cos(angle + CV_PI / 4));
    p.y = (int) (q.y + 9 * sin(angle + CV_PI / 4));
    line(img, p, q, colour, 1, CV_AA);
    p.x = (int) (q.x + 9 * cos(angle - CV_PI / 4));
    p.y = (int) (q.y + 9 * sin(angle - CV_PI / 4));
    line(img, p, q, colour, 1, CV_AA);
}
int getOrientation(const vector<Point> &pts, Mat &img)
{
    //Construct a buffer used by the pca analysis
    int sz = static_cast<int>(pts.size());
    Mat data_pts = Mat(sz, 2, CV_64FC1);
    for (int i = 0; i < data_pts.rows; ++i)
    {
        data_pts.at<double>(i, 0) = pts[i].x;
        data_pts.at<double>(i, 1) = pts[i].y;
    }
    //Perform PCA analysis
    PCA pca_analysis(data_pts, Mat(), CV_PCA_DATA_AS_ROW);
    //Store the center of the object
    Point cntr = Point(static_cast<int>(pca_analysis.mean.at<double>(0, 0)),
                      static_cast<int>(pca_analysis.mean.at<double>(0, 1)));
	//cout<<cntr.x<<endl;
    //Store the eigenvalues and eigenvectors
    vector<Point2d> eigen_vecs(2);
    vector<double> eigen_val(2);
    for (int i = 0; i < 2; ++i)
    {
        eigen_vecs[i] = Point2d(pca_analysis.eigenvectors.at<double>(i, 0),
                                pca_analysis.eigenvectors.at<double>(i, 1));
        eigen_val[i] = pca_analysis.eigenvalues.at<double>(0, i);
    }
    // Draw the principal components
    circle(img, cntr, 3, Scalar(255, 0, 255), 2);
    Point p1 = cntr + 0.02 * Point(static_cast<int>(eigen_vecs[0].x * eigen_val[0]), static_cast<int>(eigen_vecs[0].y * eigen_val[0]));
    Point p2 = cntr - 0.02 * Point(static_cast<int>(eigen_vecs[1].x * eigen_val[1]), static_cast<int>(eigen_vecs[1].y * eigen_val[1]));
    drawAxis(img, cntr, p1, Scalar(0, 255, 0), 1);
    drawAxis(img, cntr, p2, Scalar(255, 255, 0), 5);
    double angle = atan2(eigen_vecs[0].y, eigen_vecs[0].x); // orientation in radians
    //return angle;
	return cntr.x;
}
void MyPolygon( Mat img )
{
	int color=90;
  int lineType = 8;
int grosor = 110	;

  /** Create some points */
  Point rook_points[1][4];
  rook_points[0][0] = Point(0, img.rows-grosor );
  rook_points[0][1] = Point( img.cols-1,img.rows-grosor);
  rook_points[0][2] = Point( img.cols-1, img.rows-1 );
  rook_points[0][3] = Point( 0, img.rows-1);
	
  const Point* ppt[1] = { rook_points[0] };
  int npt[] = { 4 };

  fillPoly( img,
            ppt,
            npt,
            1,
            Scalar( color, color, color ),
            lineType );
///////////////////////////////////////////////////////////////////////////////////////	////////////
 grosor=3;
  rook_points[0][0] = Point(0, 0 );
  rook_points[0][1] = Point(grosor,0);
  rook_points[0][2] = Point( grosor, img.rows-1 );
  rook_points[0][3] = Point( 0, img.rows-1);
	
  //ppt[0] =  rook_points[0] ;

  fillPoly( img,
            ppt,
            npt,
            1,
            Scalar( color, color, color ),
            lineType );
///////////////////////////////////////////////////////////////////////////////////////	////////////

  rook_points[0][0] = Point(0, 0 );
  rook_points[0][1] = Point(0,grosor);
  rook_points[0][2] = Point( img.cols-1,grosor );
  rook_points[0][3] = Point( img.cols-1, 0);
	
  //ppt[0] =  rook_points[0] ;

  fillPoly( img,
            ppt,
            npt,
            1,
            Scalar( color, color, color ),
            lineType );

///////////////////////////////////////////////////////////////////////////////////////	////////////

  rook_points[0][0] = Point(img.cols-1, 0 );
  rook_points[0][1] = Point(img.cols-1,img.rows-1);
  rook_points[0][2] = Point(img.cols-1-grosor,img.rows-1 );
  rook_points[0][3] = Point(img.cols-1-grosor, 0);
	
  //ppt[0] =  rook_points[0] ;

  fillPoly( img,
            ppt,
            npt,
            1,
            Scalar( color, color, color ),
            lineType );
}

void error(const char *msg)
{
    perror(msg);
    exit(0);
}
