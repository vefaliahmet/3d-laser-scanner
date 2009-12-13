// #include <stdafx.h> // Windows
#include <highgui.h>
#include <cv.h>
#include <math.h>
#include <stdio.h>

#define PI 3.14159265
#define SIZE 200	// buffer size
#define MAXFRAMES 60	// maximum # video frames for 360 degrees
#define MAXCOLS 640	// video width
#define MAXROWS 480	//  video heigth

CvPoint		centerPoint1, axisPoint1, axisPoint2, roiPoint1, roiPoint2;
IplImage*	frame, * frame2;
int		drag = 0; // defines if you are dragging the mouse
int		numPoints = 0; // used to count # of profile points found
CvScalar	c1;
const char* message = "";
char buf[SIZE];
CvFont font;
double hScale=0.4;  // font hScale
double vScale=0.4;  // font vScale
int    lineWidth=1; // font lineWidth

void on_mouse_axis(int event, int x, int y, int flags, void* param) {
    /* user press left button */
    if (event == CV_EVENT_LBUTTONDOWN && !drag) {
        axisPoint1 = cvPoint(x, y);
		drag = 1;
    }
    /* user drag the mouse */
    if (event == CV_EVENT_MOUSEMOVE && drag) {
		axisPoint2 = cvPoint(x, y);
		frame2 = (IplImage*) cvClone(frame);
        cvLine(
			frame2,
            axisPoint1,
            axisPoint2,
            CV_RGB(0, 0, 255),
            1, CV_AA, 0
        );
		cvShowImage("3D SCANNER", frame2);
    }

    /* user release left button */
    if (event == CV_EVENT_LBUTTONUP && drag) {
		axisPoint2 = cvPoint(x, y);
		frame2 = (IplImage*) cvClone(frame);
		cvLine(
			frame2,
            axisPoint1,
            axisPoint2,
            CV_RGB(0, 0, 255),
            1, CV_AA, 0
        );
		cvShowImage("3D SCANNER", frame2);
		drag = 0;
    }
     /* user click right button: reset all */
    if (event == CV_EVENT_RBUTTONUP) {
        cvShowImage("3D SCANNER", frame);
		drag = 0;
	}
}

void on_mouse_center(int event, int x, int y, int flags, void* param) {
    /* user press left button */
    if (event == CV_EVENT_LBUTTONDOWN && !drag) {
        centerPoint1 = cvPoint(x, y);
		frame2 = (IplImage*) cvClone(frame);
        cvLine(frame2, centerPoint1, centerPoint1, CV_RGB(0, 0, 255), 3, CV_AA, 0 );
		cvShowImage("3D SCANNER", frame2);
    }
     /* user click right button: reset all */
    if (event == CV_EVENT_RBUTTONUP) {
        cvShowImage("3D SCANNER", frame);
		drag = 0;
	}
}

void on_mouse_roi(int event, int x, int y, int flags, void* param) {
	cvInitFont(&font,CV_FONT_HERSHEY_SIMPLEX|CV_FONT_ITALIC, hScale,vScale,0,lineWidth);

    /* user press left button */
    if (event == CV_EVENT_LBUTTONDOWN && !drag) {
        roiPoint1 = cvPoint(x, y);
		drag = 1;
    }

    /* user drag the mouse */
    if (event == CV_EVENT_MOUSEMOVE && drag) {
		frame2 = (IplImage*) cvClone(frame);
		cvRectangle( frame2, roiPoint1, cvPoint(x, y), CV_RGB(255, 0, 0), 1, 8, 0 );

//		_snprintf_s(buf, SIZE, "X1=%d, X2=%d, Y1=%d, Y2=%d", roiPoint1.x, x, roiPoint1.y, y); //windows
		snprintf(buf, SIZE, "X1=%d, X2=%d, Y1=%d, Y2=%d", roiPoint1.x, x, roiPoint1.y, y);  //linux

		cvPutText(frame2,buf, cvPoint(40,35), &font, cvScalar(0, 255, 0));
		cvShowImage("3D SCANNER", frame2);
    }

	/* user release left button */
    if (event == CV_EVENT_LBUTTONUP && drag) {
		roiPoint2 = cvPoint(x, y);
		frame2 = (IplImage*) cvClone(frame);

		// allow user to draw rectangle from any corner, make sure point 1 is top left corner
		if (roiPoint2.x < roiPoint1.x) {
			int xtemp = roiPoint2.x;
			roiPoint2.x = roiPoint1.x;
			roiPoint1.x = xtemp;
		}
		if (roiPoint2.y < roiPoint1.y) {
			int xtemp = roiPoint2.y;
			roiPoint2.y = roiPoint1.y;
			roiPoint1.y = xtemp;
		}
		// Show Region of Interest
		cvRectangle( frame2, roiPoint1, roiPoint2, CV_RGB(255, 0, 0), 1, 8, 0 );
		cvShowImage("3D SCANNER", frame2);
		drag = 0;
    }
 
    /* user click right button: reset all */
    if (event == CV_EVENT_RBUTTONUP) {
        cvShowImage("3D SCANNER", frame);
		drag = 0;
    }
}

IplImage* rotateFrame(IplImage* frame, double angle) {
		CvMat* rot_mat = cvCreateMat(2,3,CV_32FC1);
		frame2 = (IplImage*) cvClone(frame);
		frame2->origin = frame->origin;
		cvZero( frame2 );
		// change center to center of turn table?
		CvPoint2D32f center = cvPoint2D32f( frame->width/2, frame->height/2 );
		cv2DRotationMatrix( center, -angle, 1, rot_mat );
		cvWarpAffine( frame, frame2, rot_mat );
		return frame2;
}


int main(int argc, char* argv[]) {
	int erodeFactor, camAngle, calFrame;
	if (argc<5) {
		calFrame = 1;
	} else {
//			sscanf_s(argv[4], "%d", &calFrame); // Windows
			sscanf(argv[4], "%d", &calFrame); // Linux
	}
	if (argc<4) {
		camAngle = 15;
	} else {
//			sscanf_s(argv[3], "%d", &camAngle); // Windows
			sscanf(argv[3], "%d", &camAngle); // Linux
	}
	if (argc<3) {
		erodeFactor = 3;
	} else {
//			sscanf_s(argv[2], "%d", &erodeFactor); // Windows
			sscanf(argv[2], "%d", &erodeFactor); // Linux
	}
	if(argc<2){
		printf("Usage: %s video-file-name.avi <Erode Factor> <Camera Angle> <Calibration Frame> \n\7", argv[0]);
		return 1;
	}

	cvNamedWindow( "3D SCANNER", 1);
	CvCapture* capture = cvCreateFileCapture( argv[1] );
	// CvCapture* capture = cvCreateFileCapture( "c.avi" );
	

//	capture calibration frame (default 1)
	cvSetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES, calFrame);
	frame = cvQueryFrame( capture );
	cvSmooth(frame, frame, CV_BLUR, 3);

//	setup video writer
	double fps = cvGetCaptureProperty(
		capture,
		CV_CAP_PROP_FPS
		);
	CvSize imgSize;
	imgSize.width = frame->width;
	imgSize.height = frame->height;
	CvVideoWriter *writer = cvCreateVideoWriter(
		"out.avi",
		//CV_FOURCC('P','I','M','1'), // MPEG-1
		CV_FOURCC('D','I','V','X'), // MPEG-4
		//CV_FOURCC('M','P','4','2'), // For MPEG-4.2
		//CV_FOURCC('D','I','V','3'), // For MPEG-4.3
		//CV_FOURCC('X','V','I','D'), // For Xvid
		//CV_FOURCC('H','2','6','4'), // For H264
		//CV_FOURCC('D','X','5','0'), // For Dvix ver5
		//CV_FOURCC_PROMPT,
//		CV_FOURCC_DEFAULT, // use this default for Windows
		//-1, //A codec code of "-1" will open a codec selection window (in Windows)
		fps,
		imgSize
		);

	// Rotate image
//	frame = rotateFrame(frame,camAngle);

	// Define vertical axis
	frame2 = (IplImage*) cvClone(frame);
	cvInitFont(&font,CV_FONT_HERSHEY_SIMPLEX|CV_FONT_ITALIC, hScale,vScale,0,lineWidth);
	cvSetMouseCallback("3D SCANNER", on_mouse_axis, NULL);
	message = "Draw vertical axis (left-click, drag then release and hit any key)";
	cvPutText(frame2,message, cvPoint(10,15), &font, cvScalar(255,255,255));
	cvShowImage( "3D SCANNER", frame2 );
	cvWaitKey(0);
	double axisAngle = 90.0 - (atan((double)(axisPoint2.y - axisPoint1.y)/(double)(axisPoint2.x - axisPoint1.x)) * 180. / PI);
	frame = rotateFrame(frame,axisAngle);
	cvShowImage( "3D SCANNER", frame );


	// Define region of interest
	frame2 = (IplImage*) cvClone(frame);
	cvInitFont(&font,CV_FONT_HERSHEY_SIMPLEX|CV_FONT_ITALIC, hScale,vScale,0,lineWidth);
	cvSetMouseCallback("3D SCANNER", on_mouse_roi, NULL);
	message = "Create Rectangle of Interest (left-click, drag then release and hit any key)";
	cvPutText(frame2,message, cvPoint(10,15), &font, cvScalar(255,255,255));
	cvShowImage( "3D SCANNER", frame2 );
	cvWaitKey(0);
	cvShowImage( "3D SCANNER", frame );

	// Define center of turn-table (0,0)
	frame2 = (IplImage*) cvClone(frame);
	cvSetMouseCallback("3D SCANNER", on_mouse_center, NULL);
	message = "Left-click on center point of turn-table, release and hit Enter)";
	cvPutText(frame2,message, cvPoint(10,15), &font, cvScalar(255,255,255));
	cvShowImage( "3D SCANNER", frame2 );
	cvWaitKey(0);
	cvShowImage( "3D SCANNER", frame );

   cvSetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES, 1);

	int frameCount = 0;
	int myPoints[MAXFRAMES][MAXCOLS] = {0};
	double highestVal = 0;
	double redVal = 0;
	double brightnessVal = 0;
	while(1) {
//		printf("Current Frame: %d \n", frameCount);
		frame = cvQueryFrame( capture );
		if( !frame ) break;
		cvSmooth(frame, frame, CV_BLUR, 3);
		// Rotate image
		frame = rotateFrame(frame,axisAngle);

		// Draw rotation point
		cvLine(frame, centerPoint1, centerPoint1, CV_RGB(0, 255, 0), 4, CV_AA, 0 );

		// Draw Region of Interest
		cvRectangle(frame, roiPoint1, roiPoint2, CV_RGB(0, 255, 0), 1, 8, 0 );

		// cycle throught all pixels and find highest value in each row
		// *********** convert myPoints array to vector to allow dynamic size ************

		// y is height (Z)
		for(int y=0; y<frame->height; ++y) {
			//next row, reset highest value
			highestVal = 0;
			// x is radius
			for(int x=0; x<frame->width; ++x) {
				c1 = cvGetAt(frame,y,x);
				// if point is inside region of interest
				if (x >= roiPoint1.x && x <= roiPoint2.x && y >= roiPoint1.y && y <= roiPoint2.y) {
 /* DEBUG CODE BELOW */
			/*			frame2 = (IplImage*) cvClone(frame);
						_snprintf_s(buf, SIZE, "b=%f g=%f r=%f\n", c1.val[0], c1.val[1], c1.val[2]); //windows
						message = buf;
						cvPutText(frame2,message, cvPoint(10,15), &font, cvScalar(255,255,255));
						cvLine(frame2, cvPoint(x,y), cvPoint(x,y), CV_RGB(255, 0, 0), 4, CV_AA, 0 );
						cvShowImage( "3D SCANNER", frame2 );
						cvWaitKey(0);*/

					// if redness of point (2x red - blue - green = red)
					redVal = c1.val[2]*2 - c1.val[0] - c1.val[1];

					// brightness of point
					brightnessVal = c1.val[2] + c1.val[0] + c1.val[1];
					
					if ((redVal > 25) && (brightnessVal > highestVal) &&
						(c1.val[2] > 100)) {  // enough red present
					//	((c1.val[0] + c1.val[1] + c1.val[2]) < 600)) { // pixel not too bright
						highestVal = brightnessVal;
						myPoints[frameCount][y] = x;
					}
				}
				
			}
			// display best point found on last row processed
			if (myPoints[frameCount][y] != 0) {
				cvLine(frame, cvPoint(myPoints[frameCount][y],y), cvPoint(myPoints[frameCount][y],y), CV_RGB(0, 0, 255), 2, CV_AA, 0 );
			}
		} 
		cvShowImage( "3D SCANNER", frame );
		cvWriteFrame(writer, frame);


        char myChar = cvWaitKey(33);
        if( myChar == 27 ) break; // ESC to break
	frameCount++;
	}

	for(int frameNum=0; frameNum<frameCount; ++frameNum) {
		for(int z=0; z<MAXROWS; ++z) {
			if (myPoints[frameNum][z] != 0) {
					numPoints++;
			}
		}
	}
	// print ply header (use Meshlab to open PLY file)
	printf("ply\nformat ascii 1.0\nelement vertex %d\nproperty float x\nproperty float y\nproperty float z\nend_header\n", numPoints);
	double dist = 0;
	double x,y, z2;
	for(int frameNum=0; frameNum<frameCount; ++frameNum) {
		for(int z=0; z<MAXROWS; ++z) {
			if (myPoints[frameNum][z] != 0) {
				// dist is distance from profile point to axis of rotation (it should now be vertical so use X)
				//  * PI / 180 TO CONVERT TO RADIANS
				dist = ((double)centerPoint1.x - (double)myPoints[frameNum][z])/sin((double)camAngle * PI / 180.0);
				if ((float)dist > 0) {
					z2 = (double)z - abs(((double)dist * tan((double)(camAngle)* PI / 180.0)));
				}
				else {
					z2 = (double)z + abs(((double)dist * tan((double)(camAngle)* PI / 180.0)));
				}
				x = dist*sin((double)(360.0/(double)frameCount*(double)frameNum) * PI / 180.0);
				y = dist*cos((double)(360.0/(double)frameCount*(double)frameNum) * PI / 180.0);
				printf("%f %f %f\n", x, y, z2);
			}
		}
	}
	cvReleaseCapture( &capture );
	cvReleaseImage( &frame );
	cvReleaseImage( &frame2 );
//	cvReleaseMat( &rot_mat );
	cvReleaseVideoWriter(&writer);
	cvDestroyWindow( "3D SCANNER" );
	return 0;
}
