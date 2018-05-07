/*
[Celestial] Concierto de los Angeles
20180501 // Player2
-------------------------------------------------
	___-___---__--
	--_____---____
	-----_--------
	__________---_
	. 
*/



#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "ofxCv.h"
#include "ofxOpenCv.h"
#include "ofxImageSequenceRecorder.h"
#include "ofxGui.h"
#include "ofxXmlSettings.h"

//#include "ofxFBOTexture.h"

#define OSC_HOST "localhost"
#define OSC_IN_PORT 11111
#define OSC_OUT_PORT 10001

using namespace cv;
using namespace ofxCv;

class Stuff : public ofxCv::RectFollower {
protected:
	ofColor color;

public:
	Stuff()
		:startedDying(0) {
	}
	//--------------------------------------------------------------
	void Stuff::setup(const cv::Rect& track);
	void Stuff::update(const cv::Rect& track);
	void Stuff::kill();
	void Stuff::draw();

	float startedDying;
	ofPolyline all;
	ofVec2f cur, smooth;
};


class ofApp : public ofBaseApp{

public:
	void setup();
	void update();
	void draw();

	void load_dir();

	void keyPressed(int key);
	void keyReleased(int key);
	void exit();

	ofVideoGrabber camera;
	cv::Mat grabberGray;
	ofxCv::FlowPyrLK flow;
	ofVec2f p1;
	ofRectangle rect;
	vector<cv::Point2f> feats;

	// ------ ------ ------ ------ ------ ------ | gui |
	bool showGui;
	ofxPanel gui;

	ofxGuiGroup inputGroup;
	ofxGuiGroup flowGroup, diffGroup;
	ofxGuiGroup outputGroup;

	ofxToggle load_new;
	ofxToggle enable_record;
	ofxFloatSlider init_seq, len_seq, vel_seq;
	ofxToggle en_edges;

	ofxToggle en_Optoflow;
	ofParameter<float> lkQualityLevel;
	ofParameter<int> lkMaxLevel, lkMinDistance;
	ofParameter<float> edge_thr1, edge_thr2;
	ofxToggle draw_colormap;
	ofxFloatSlider cut_colormap;
	ofxToggle alpha_colormap;
	ofxToggle en_trackOpto;
	ofxToggle en_labelOpto;
	ofxToggle en_dirOpto;

	ofxToggle en_Simpleblob;
	ofxFloatSlider ampli, diff_thresh, contour_thresh;
	ofxToggle en_track;

	ofxIntSlider val_ofColumn;
	ofxToggle send_ofColumn;
	ofxToggle send_ofBlobs;
	ofxToggle send_sBlobs;

	// ------ ------ ------ ------ ------ ------ | osc |
	ofxOscReceiver osc_recver;
	ofxOscSender osc_sender;

	// ------ ------ ------ ------ ------ ------ | rec stuff |
	ofxImageSequenceRecorder img_recorder;
	bool isRecording;

	// ------ ------ ------ ------ ------ ------ | imgs |
	ofDirectory dir;
	bool bLoaded;
	
	int nImgs;
	int ih, iw;
	int gridSz, scale;
	bool blackFlag;
	int ind_ff;
	int t0, t1, index1;
	vector<ofImage> imgList;
	vector<cv::Mat> matList;
	cv::Mat tempMat;
	ofImage tempImg;
	ofFbo fbo_A, fbo_B, fbo_C, fbo_D;
	ofPixels pixs;
	ofColor cColor, nColor;
	ofImage optoMap;
	ofFbo buffer;

	ofxCv::FlowPyrLK LK;
	ofxCv::FlowFarneback FB;
	ofxCv::Flow* curFlow;
	vector<ofPoint> old_feats;
	vector<ofVec2f> new_feats;
	ofColor dirColor;
	int transpa;

	// --------------------- NRML
	int w, h, t;
	float tt;

	// --------------------- BLOB
	ofImage gray, edge, sobel;
	cv::Mat prevImg, diffImg, actImg, grayImg;
	ofImage nImg, tImg;

	ofxCv::ContourFinder cFinder;
	ofxCv::RectTrackerFollower<Stuff> tracker;
	ofxCv::ContourFinder cFinderOpto;
	ofxCv::RectTrackerFollower<Stuff> trackerOpto;

	vector<int> theQueue;
	int auxLabel;
	bool isHere;
	int pX, pY;
};



