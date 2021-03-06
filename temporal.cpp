#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <opencv2/opencv.hpp>

#include <iostream>

using namespace cv;
using namespace std;

Mat dst;

void andeir();
void drawAxis(Mat&, Point, Point, Scalar, const float);
double getOrientation(const vector<Point> &, Mat&);
void MyPolygon( Mat );

int main(int argc, char** argv)
{

	VideoCapture capture(0);
 capture.open( 0);

  if( capture.isOpened() )
  {
	  cout<<"captura abierta"<<endl;
    for(;;)
    {
		Mat frame, frameGray,frameNegro;
		Mat  detected_edges;
		capture.read(frame);
		
		 MyPolygon( frame);

		
		dst.create( frame.size(), frame.type() );
		frameNegro.create( frame.size(), frame.type() );

  /// Convert the image to grayscale
  		


      	//-- 3. Apply the classifier to the frame
		if( !frame.empty() )
			{

					
				
				
	//////////////////////////////////////contorno y direccion///////////////////////////////////////////////
			
			cvtColor(frame, frame, COLOR_BGR2GRAY);
			// Convert image to binary
			Mat bw;
			threshold(frame, bw, 50, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
			// Find all the contours in the thresholded image
			vector<Vec4i> hierarchy;
			vector<vector<Point> > contours;
			findContours(bw, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
			
			double area[9999];	
			int indiceAreaMayor=0;
			for (size_t i = 0; i < contours.size(); ++i){
				
				area[i] = contourArea(contours[i]);
				// Ignore contours that are too small or too large
				if (area[i] < 32e2 || 1e5 < area[i]) continue;
				drawContours(frame, contours, static_cast<int>(i), Scalar(0, 0, 255), 2, 8, hierarchy, 0);
				
				if (i>0){
					if (area[i]>area[indiceAreaMayor]) indiceAreaMayor=i;
				}
			}
			//cout<<indiceAreaMayor<<endl;
			getOrientation(contours[indiceAreaMayor], frame);
				
			
				
				 imshow("source", frame);
			}
		int c = waitKey(1);
		if( (char)c == 'c'|| c == 27 ) { break; }
		//imshow( "lineas", frame );
	}
  }
  else cout<<"mierda pa ti"<<endl;
	return(0);
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
double getOrientation(const vector<Point> &pts, Mat &img)
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
	cout<<cntr.x<<endl;
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
    return angle;
}
void MyPolygon( Mat img )
{
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
            Scalar( 150, 150, 150 ),
            lineType );
 }