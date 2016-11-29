//opencv
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
//#include "opencv2/videoio.hpp"

#include <iostream>
#include <stdio.h>

using namespace std;
using namespace cv;

int main( int argc, char *argv[] )
{
	
	//String ipVideo = "http://";
	//ipVideo.append( argv[1]);
	//ipVideo.append(":8080/?action=stream");
String ipVideo = "http://192.168.1.55:8080/?action=stream?dummy=param.mjpeg";
//String ipVideo = "http://c-cam.uchicago.edu/mjpg/video.mjpg";

	cout<<ipVideo<<endl;
	VideoCapture capture(ipVideo);
	Mat frame;
	int c;
	capture.open( ipVideo) ;

	if( capture.isOpened() )
		{
			do{

			  capture.read(frame);
				imshow( "ola k ase", frame );
				c = waitKey(1);
			}while ((char)c != 'c');
		}
	else cout<<"Mierda pa ti"<<endl;
	
	return 0;
}

