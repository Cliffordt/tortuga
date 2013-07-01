/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Daniel Hakim
 * All rights reserved.
 *
 * Author: Daniel Hakim <dhakim@umd.edu>
 * File:  packages/vision/src/GateDetector.cpp
 */


// Library Includes
#include "cv.h"
#include "highgui.h"

// Project Includes
#include "vision/include/main.h"
#include "vision/include/GateDetector.h"
#include "vision/include/OpenCVImage.h"
#include "vision/include/Camera.h"
#include "vision/include/VisionSystem.h"
#include "vision/include/TableColorFilter.h"
#include "vision/include/WhiteBalance.h"
#include "vision/include/GateDetectorKate.h"


using namespace std;
using namespace cv;


namespace ram {
namespace vision {

GateDetector::GateDetector(core::ConfigNode config,
                           core::EventHubPtr eventHub) :
    Detector(eventHub),
    cam(0)
{
    init(config);
}
    
GateDetector::GateDetector(Camera* camera) :
    cam(camera)
{
    init(core::ConfigNode::fromString("{}"));

}

void GateDetector::init(core::ConfigNode)
{
	frame = new OpenCVImage(640,480);
	gateX=0;
	gateY=0;
	found=false;
	//This frame will be a copy of the original rotated 90� counterclockwise.  
	//But only if the camera is on sideways, otherwise we do 640 by 480 like usual.
	//ie 640 by 480 if cameras on right (Or completely upsidedown!... sigh), else 480 by 640.
	gateFrame =cvCreateImage(cvSize(640,480),8,3);
	gateFrameRatios = cvCreateImage(cvGetSize(gateFrame),8,3);
}
    
GateDetector::~GateDetector()
{
	delete frame;
	cvReleaseImage(&gateFrame);
	cvReleaseImage(&gateFrameRatios);
}

double GateDetector::getX()
{
	return gateXNorm;
}

double GateDetector::getY()
{
	return gateYNorm;
}

void GateDetector::show(char* window)
{
	cvShowImage(window,((IplImage*)(gateFrame)));
}

IplImage* GateDetector::getAnalyzedImage()
{
	return (IplImage*)(gateFrame);
}

    
void GateDetector::update()
{
    cam->getImage(frame);
    processImage(frame, 0);
}
    
void GateDetector::processImage(Image* input, Image* output)
{	
//KATE
	Mat img = input->asIplImage();
	//imshow("input image", img);

	//IplImage* tempImage=0;
	img_whitebalance = WhiteBalance(img);
	Mat img_red = gate.gateblob(img_whitebalance); //built in redfilter
	//Mat img_red = gate.hedgeblob(img_whitebalance);  //built in green filter

	//img_gate = gate.rectangle(img_red, img_whitebalance);
	//cvtColor(img_gate,img_gate,CV_BGR2RGB);
	cvtColor(img_whitebalance,img_whitebalance,CV_BGR2RGB);

	input->setData(img_whitebalance.data,false);
	frame->copyFrom(input);
	
	if(output)
	    {
		output->copyFrom(frame);
		//if (m_debug >= 1) {
		//    output->copyFrom(frame);
		//} //endif mdebug==1

	    } //end if output



//------------end Kate


       // output->setData(output_blob2.data,false);
        //tempImage->imageData = (char *)output_blob2.data;


//  These lines are correct only if the camera is on sideways again.
//	rotate90Deg(image,gateFrame);//Rotate image into gateFrame, so that it will be vertical.
//	rotate90Deg(image,gateFrameRatios); 

// Otherwise just copy like this:
	
	//Could just say gateFrame=(IplImage*)(*frame), 
	//but I've heard openCV gets angry if you write pixels 
	//to image when it is getting images from a camera.
	//TODO: Before changing this, check the Camera class to see if it automatically copies anyway.
/*KATE
	cvCopyImage(image,gateFrame);
	cvCopyImage(gateFrame, gateFrameRatios);

	to_ratios(gateFrameRatios);
	found=gateDetect(gateFrameRatios,gateFrame,&gateX,&gateY) ? 1 : 0;
	gateXNorm=gateX;
	gateYNorm=gateY;
	gateXNorm/=image->width;
	gateXNorm/=image->height;
*/
	publishFoundEvent(gate.finalPair);
        if (output)
        {
	    output->copyFrom(frame);///kate a dded
           // OpenCVImage temp(gateFrame, false);
           // output->copyFrom(&temp);
        }
};

void GateDetector::publishFoundEvent(foundLines::parallelLinesPairs finalPairs)
{
    static math::Degree xFOV = VisionSystem::getFrontHorizontalFieldOfView();
    static math::Degree yFOV = VisionSystem::getFrontVerticalFieldOfView();
    static double xPixelWidth = VisionSystem::getFrontHorizontalPixelResolution();
    static double yPixelHeight = VisionSystem::getFrontVerticalPixelResolution();

   GateEventPtr event = GateEventPtr(new GateEvent());
    
    double centerX = 0, centerY = 0;
    Detector::imageToAICoordinates(frame, finalPairs.center.x, finalPairs.center.y,
                                   centerX, centerY);

	//blob.size (or any keypoint.size) returns the diameter of the meaningfull keypooint neighborhood
	//dont get it confused with keypoint.size() which will return the size of the keypoitn vector

   // double blobWidth = blob.getWidth();
   // double fracWidth = blobWidth / xPixelWidth;
   // double range = m_physicalWidthMeters / (2 * std::tan(xFOV.valueRadians() * fracWidth / 2));

    int minX = finalPairs.line1_lower.x-finalPairs.width;
    int maxX = finalPairs.line2_lower.x+finalPairs.width;
    int minY = finalPairs.line1_lower.y-finalPairs.line1_height;
    int maxY = finalPairs.line1_upper.y-finalPairs.line1_height;;

    bool touchingEdge = false;
    if(minX == 0 || minY == 0 || maxX == xPixelWidth || maxY == yPixelHeight)
        touchingEdge = true;

    event->range = finalPairs.width;
    //event->color = color;
    //event->touchingEdge = touchingEdge;
	if (finalPairs.foundleft == 1)
	{
	    event->leftX = finalPairs.line1_lower.x;
	    event->leftY = finalPairs.line1_lower.y;
	    event->haveLeft = true;
	}
	if (finalPairs.foundright == 1)
	{
	    event->rightX = finalPairs.line2_lower.x;
	    event->rightY = finalPairs.line2_lower.y;
	    event->haveRight = true;
	}

    publish(EventType::GATE_FOUND, event);
};



} // namespace vision
} // namespace ram
