#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>

using namespace cv;
using namespace std;

Mat dst;

void andeir()
{
	Point pt1(dst.cols/2,dst.rows), pt2(0,0);

	
	for (int fila=dst.rows; fila>0; fila--){
		int acumulado=0,contador=0;
		for (int columna=0;columna<dst.cols;columna++){
			if (dst.at<bool>(fila,columna) > 100){
				acumulado+=columna;
				contador++;
				//cout<<columna<<endl;
			}
		}
		
		if (contador > 0){
			pt2.x=acumulado/contador;
			pt2.y=fila;
			cout<<pt2<<endl;
		}
		line( dst, pt1,  pt2, CV_RGB(255, 255, 255), /* thickness=*/1	,  /*lineType=*/8,  /*shift=*/0);
		pt1=pt2;
	}	

	
	//imshow("pintao", *marco);
	
}

int main(int argc, char** argv)
{

	String ipVideo = "http://";
		  //String ipVideo = "http://192.168.1.55:8080/?action=stream?dummy=param.jpeg";
	ipVideo.append( argv[1]);//////////////////////
		  //resolucion.append( argv[3]);
	cout<<ipVideo<<endl;
	ipVideo.append(":8080/?action=stream");///////////////
	cout<<ipVideo<<endl;
	VideoCapture capture(0);
	
//const char* filename = argc >= 2 ? argv[1] : "pic1.jpg";
 capture.open( 0);

  if( capture.isOpened() )
  {
    for(;;)
    {
		Mat frame, frameGray;
		Mat  detected_edges;
		capture.read(frame);

		
		dst.create( frame.size(), frame.type() );

  /// Convert the image to grayscale
  		
		 //cvtColor(dst, cdst, CV_GRAY2BGR);
		//if  (resolucion == 320) resize(frame, frame, Size(320, 240), 0, 0, INTER_CUBIC);
		//capture >> frame;

      	//-- 3. Apply the classifier to the frame
		if( !frame.empty() )
			{

				cvtColor( frame, frameGray, CV_BGR2GRAY );
				blur( frameGray, detected_edges, Size(3,3) );
			 	Canny(frameGray, dst, 200, 600, 3);
				#if 0
				  vector<Vec2f> lines;
				  HoughLines(dst, lines, 1, CV_PI/180, 100, 0, 0 );

				  for( size_t i = 0; i < lines.size(); i++ )
				  {
					 float rho = lines[i][0], theta = lines[i][1];
					 Point pt1, pt2;
					 double a = cos(theta), b = sin(theta);
					 double x0 = a*rho, y0 = b*rho;
					 pt1.x = cvRound(x0 + 1000*(-b));
					 pt1.y = cvRound(y0 + 1000*(a));
					 pt2.x = cvRound(x0 - 1000*(-b));
					 pt2.y = cvRound(y0 - 1000*(a));
					 line( cdst, pt1, pt2, Scalar(0,0,255), 3, CV_AA);
				  }
				 #else
/*				  vector<Vec4i> lines;

					
				  HoughLinesP(dst, lines, 1, CV_PI/180, 50, 50, 10 );
				  for( size_t i = 0; i < lines.size(); i++ )
				  {
					Vec4i l = lines[i];
					line( cdst, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 3, CV_AA);
				  }
*/

		//GaussianBlur( frameGray, framGray, Size(9, 9), 2, 2 );


				 #endif
//////////convertir a binario//////////////////////
				/* 0: Binary
			     1: Binary Inverted
 			    2: Threshold Truncated
  				   3: Threshold to Zero
 			    4: Threshold to Zero Inverted
   */
  //threshold( dst, dst, 130, 255,0);

				andeir();
				 imshow("source", frame);
				 imshow("detected lines", dst);
			}
		int c = waitKey(1);
		if( (char)c == 'c'|| c == 27 ) { break; }
		//imshow( "lineas", frame );
	}
  }
  else cout<<"mierda pa ti"<<endl;
	return(0);
}
