#include "ofApp.h"

using namespace cv;
using namespace ofxCv;

//--------------------------------------------------------------
void ofApp::setup(){
	ofBackground(0);
	ofSetVerticalSync(true);
	ofSetFrameRate(12);
	h = ofGetHeight();
	w = ofGetWidth();

	// ... ... ... --- --- ... ... ... --- --- ... ... --- gui
	osc_sender.setup(OSC_HOST, OSC_OUT_PORT);

	// ... ... ... --- --- ... ... ... --- --- ... ... --- gui
	gui.setup();
	inputGroup.setup(".-INPUT");
	inputGroup.add(load_new.setup("load_NEW", false));
	inputGroup.add(enable_record.setup("enable_RECORD",false));
	inputGroup.setHeaderBackgroundColor(ofColor::white);
	inputGroup.setBorderColor(ofColor::darkGray);
	inputGroup.setTextColor(ofColor::darkGray);
	inputGroup.add(init_seq.setup("init_LOOP", 51, 0, 100));
	inputGroup.add(len_seq.setup("len_LOOP", 100, 1, 600));
	inputGroup.add(vel_seq.setup("dur_SAMPLE", 83.3, 1, 1000));
	inputGroup.add(en_edges.setup("-->enable_EDGES", false));
	inputGroup.add(edge_thr1.set("thrA_EDGES", 25, 0, 255));
	inputGroup.add(edge_thr2.set("thrB_EDGES", 57, 0, 255));
	gui.add(&inputGroup);

	flowGroup.setup(".-OP.FLOW");
	flowGroup.setHeaderBackgroundColor(ofColor::darkBlue);
	flowGroup.setBorderColor(ofColor::darkBlue);
	flowGroup.setTextColor(ofColor::whiteSmoke);
	//operatorGroup.setBackgroundColor(ofColor::darkBlue);
	flowGroup.add(en_Optoflow.setup("-->enable_OPTOFLOW", true));
	flowGroup.add(lkMaxLevel.set("maxLevel_LK", 2, 0, 6));
	flowGroup.add(lkQualityLevel.set("qLevel_LK", 0.01, 0.001, .5));
	flowGroup.add(lkMinDistance.set("minDist_LK", 4, 1, 50));

	flowGroup.add(draw_colormap.setup("-->enable_COLORMAP", false));
	flowGroup.add(cut_colormap.setup("cut_COLORMAP", 7.0, 0.01, 25.0));
	flowGroup.add(alpha_colormap.setup("alpha_COLORMAP", false));
	flowGroup.add(en_trackOpto.setup("en_OPTOTRACK", false));
	flowGroup.add(en_labelOpto.setup("en_LABEL-OPT", false));
	flowGroup.add(en_dirOpto.setup("en_DIR-COLOR", false));
	gui.add(&flowGroup);

	diffGroup.setup(".-OP.DIFF");
	diffGroup.setHeaderBackgroundColor(ofColor::darkRed);
	diffGroup.setBorderColor(ofColor::darkRed);
	diffGroup.setTextColor(ofColor::whiteSmoke);
	diffGroup.add(en_Simpleblob.setup("-->enable_DIFFBLOB", true));
	diffGroup.add(ampli.setup("amp_DIFF", 3, 1, 32));
	diffGroup.add(diff_thresh.setup("thres_DIFF", 30, 0, 255));
	diffGroup.add(en_track.setup("en_TRACK", true));
	gui.add(&diffGroup);

	outputGroup.setup(".-OUTPUT");
	outputGroup.setHeaderBackgroundColor(ofColor::darkGray);
	outputGroup.setBorderColor(ofColor::darkGray);
	outputGroup.setTextColor(ofColor::whiteSmoke);
	outputGroup.add(val_ofColumn.setup("n_ofCOL", 48, 0, 95));
	outputGroup.add(send_ofColumn.setup("send_ofCOL", false));
	outputGroup.add(send_ofBlobs.setup("send_ofBLOBS", false));
	outputGroup.add(send_sBlobs.setup("send_sBLOBS", true));

	gui.add(&outputGroup);

	gridSz = 10;
	cout << "OK1: " << gridSz << endl;
	// ... ... ... --- --- ... ... ... --- --- ... ... --- files
	load_dir();

	// other inits
	actImg = toCv(imgList[0]);
	cvtColor(actImg, grayImg, CV_RGB2GRAY);
	prevImg = grayImg;

	scale = 0;
	bLoaded = false;
	blackFlag = false;
	showGui = true;
	ind_ff = 0;
	index1 = 1;

	// ... ... ... --- --- ... ... ... --- --- ... ... --- | cFinder |
	cFinder.setMinAreaRadius(10);
	cFinder.setMaxAreaRadius(640 * 480);
	cFinder.setTargetColor(ofColor(255, 255, 255), TRACK_COLOR_RGB);
	cFinder.setFindHoles(true);
	tracker.setPersistence(10);
	tracker.setMaximumDistance(150);

	cFinderOpto.setMinAreaRadius(10);
	cFinderOpto.setMaxAreaRadius(640 * 480);
	cFinderOpto.setTargetColor(ofColor(255, 255, 255), TRACK_COLOR_RGB);
	cFinderOpto.setFindHoles(true);
	trackerOpto.setPersistence(10);
	trackerOpto.setMaximumDistance(150);
	cout << "OK2: setup" << endl;

	// empty the queue
	theQueue.resize(50);
	for (int i = 0; i < 50; i++) {
		theQueue[i] = -1;
	}

	// ... ... ... --- --- ... ... ... --- --- ... ... --- | rec |
	img_recorder.setPrefix(ofToDataPath("rec/ce_img_"));
	img_recorder.setFormat("png");
}

//--------------------------------------------------------------
void ofApp::update(){
	t = ofGetElapsedTimeMillis();
	tt = ofGetElapsedTimef();

	std::stringstream strm;
	strm << "CE_player // fps: " << ofGetFrameRate();
	ofSetWindowTitle(strm.str());

	// check state
	if (load_new == true) {
		load_dir();
		load_new = false;
	}

	// ... ... ... --- --- sequence
	int ini = init_seq *(nImgs / 100.0);
	//if (((ini + ind_ff)<(nImgs - len_seq)) && ((ini + ind_ff)<(ini + len_seq))) {
	if ( ((ini + ind_ff)<(ini + len_seq)) ) {
		t0 = ofGetElapsedTimeMillis();
		if ((t0 - t1) > vel_seq) {
			//index_01++;
			ind_ff++;
			index1 = ini + ind_ff;
			blackFlag = true;
			t1 = ofGetElapsedTimeMillis();
		}
	}
	else {
		ind_ff = 0;
	}

	// ... ... ... --- --- operate new frame
	if (blackFlag) {

		actImg = toCv(imgList[index1]);
		cvtColor(actImg, grayImg, CV_RGB2GRAY);
		//nImg = imgList[index1];
		
		if (en_edges == true) {
			Canny(grayImg, edge, edge_thr1, edge_thr2);
			gray.update();
			edge.update();
		}
		if (en_Optoflow) {				// ----- ------ OPTO FLOW
			// ... ... ... process
			LK.setWindowSize(15);
			LK.setQualityLevel(lkQualityLevel);
			LK.setMinDistance(lkMinDistance);
			LK.setMaxLevel(lkMaxLevel);
			LK.setFeaturesToTrack(feats);
			if (en_edges == true) { LK.calcOpticalFlow(edge); }
			else { LK.calcOpticalFlow(imgList[index1]); }
			new_feats = LK.getMotion();

			// ... ... ... draw fbo
			fbo_B.begin();
			ofClear(0, 0);
			ofSetColor(0, 255, 0, 200);
			// draw vectorfields
			LK.draw(0, 0);
			for (int i = 0; i < new_feats.size(); i++) {
				if (new_feats[i].length() <= 15) {
					scale = ofMap(new_feats[i].length(), 0, 15, 0, 255);
					ofSetColor(255, 255, 0, scale+50);
					//ofDrawRectangle(ofVec2f(feats[i].x, feats[i].y) + new_feats[i], 2, 2);
					ofLine(ofVec2f(feats[i].x, feats[i].y), ofVec2f(feats[i].x, feats[i].y) + new_feats[i]);
				}
				else {
					scale = (255);
					ofSetColor(255, 100, 0);
					//ofDrawRectangle(ofVec2f(feats[i].x, feats[i].y) + new_feats[i], 2, 2);
					float sca = 15/new_feats[i].length();
					ofLine(ofVec2f(feats[i].x, feats[i].y), ofVec2f(feats[i].x, feats[i].y) + sca*new_feats[i]);
				}
				ofSetColor(new_feats[i].length()>cut_colormap ? 255 : 0, scale<255 ? scale : 255);
				ofDrawRectangle(ofVec2f(feats[i].x, feats[i].y), 2, 2);
			}

			// OSC - send_COL
			if (send_ofColumn == true) {
				ofxOscMessage m;
				m.setAddress("/ce_player/ofColumns/qMov");
				int nrows = int(ih / gridSz);
				m.addIntArg(nrows);
				for (int iy = 0; iy < nrows; iy += 1) {
					int gridIndex;
					gridIndex = val_ofColumn*nrows + iy;
					m.addIntArg(int(new_feats[gridIndex].length()*100));
				}
				osc_sender.sendMessage(m);

				ofSetLineWidth(5);
				ofSetColor(0, 0, 255, 150);
				ofLine(val_ofColumn*gridSz, 0, val_ofColumn*gridSz, h);
				ofSetLineWidth(1);
				ofSetColor(0, 0, 255, 220);
				ofLine(val_ofColumn*gridSz, 0, val_ofColumn*gridSz, h);
			}
			fbo_B.end();

			buffer.begin();
			ofClear(0, 0);
			// draw colorMap
			for (int i = 0; i < new_feats.size(); i++) {
				scale = ofMap(new_feats[i].length(), 0, 15, 0, 255);
				if ( alpha_colormap == true ) {
					transpa = scale < 255 ? scale : 255;
				} else {
					transpa = 255;
				}
				if ( (en_dirOpto==true) && (new_feats[i].length()>5) ) {
					float angle = atan2(new_feats[i].y, new_feats[i].x);
					// colors
					if ((new_feats[i].x == 0) && (new_feats[i].y == 0)) {
						dirColor = ofColor(0,0,0);
					}
					else if ((angle > -PI/4) && (angle < PI/4)) { // ESTE (rojo)
						dirColor = ofColor(255, 0 , 0);
					}
					else if ((angle >= PI/4) && (angle <= 3*PI/4)) { // SUR (amarillo)
						dirColor = ofColor(255, 255, 0);
					}
					else if ((angle > 3*PI/4) || (angle < -3*PI/4)) { // OESTE (negro)
						dirColor = ofColor(0, 255, 0);
					}
					else if ((angle <= -PI/4) && (angle >= -3*PI/4)) { // NORTE (blanco)
						dirColor = ofColor(0, 0, 255);
					}
					else {
						dirColor = ofColor(0, 0, 0);
					}
				} else {
					dirColor = ofColor(new_feats[i].length() > cut_colormap ? 255 : 0);				
				}
				ofSetColor(dirColor, transpa);
				ofDrawRectangle(ofVec2f(feats[i].x, feats[i].y), gridSz, gridSz);
			}
			buffer.end();

			if (draw_colormap == true) {
				fbo_B.begin();
				buffer.draw(0, 0);
				fbo_B.end();
			}

			if (en_trackOpto==true)		{// ----- ------ OPTO TRACK
				// update image from colormap buffer
				ofPixels pix;
				buffer.readToPixels(pix);
				nImg.setFromPixels(pix);
				nImg.update();
				cFinderOpto.findContours(nImg);
				trackerOpto.track(cFinderOpto.getBoundingRects());
				// track and draw
				fbo_B.begin();
				ofNoFill();
				int n = cFinderOpto.size();
				for (int i = 0; i < n; i++) {
					// rect and shapes
					ofSetLineWidth(2);
					ofSetColor(ofColor::blueSteel, 205);
					cv::Rect r = cFinderOpto.getBoundingRect(i);
					ofDrawRectangle(r.x, r.y, r.width, r.height);
					ofSetLineWidth(4);
					nColor = ofColor(255, 127 + (i % 128));
					ofSetColor(nColor);
					ofPolyline shape = cFinderOpto.getPolyline(i);
					shape.draw();
					// labels
					if (en_labelOpto == true) {
						ofPushMatrix();
						ofTranslate(r.x, r.y + r.height + 20);
						int label = cFinderOpto.getLabel(i);
						float ar = cFinderOpto.getContourArea(i);
						ofVec2f velocity = toOf(cFinderOpto.getVelocity(i));

						std::stringstream st;
						int ag = trackerOpto.getAge(label);
						st << "[n]: " << ofToString(label) << endl;//"\t[t]: " << ag << endl;
						st << "[x]: " << r.x << "\t[y]: " << r.y << endl;
						st << "[v.x]: " << velocity.x << "\t[v.y]: " << velocity.y << endl;
						st << "[$]: " << ar << "\t[amp]: " << ofRandom(0, 1.0) << endl;

						ofSetColor(ofColor::whiteSmoke);
						ofSetLineWidth(1);
						ofDrawBitmapString(st.str(), 0, 0);
						ofPopMatrix();
					}
				}
				// then update paths
				/*dont
				vector<Stuff>& followers = trackerOpto.getFollowers();
				for (int i = 0; i < followers.size(); i++) {
					followers[i].draw();
				}*/
				fbo_B.end();

				// OSC - ofBlob
				if (send_ofBlobs == true) {
					ofxOscMessage m;
					m.setAddress("/ce_player/ofBlobs/area");
					m.addIntArg(int(n));
					for (int i = 0; i < n; i++) {
						m.addIntArg(int(cFinderOpto.getContourArea(i)));
					}
					osc_sender.sendMessage(m);
				}
			}
		} else if (en_Simpleblob==true) {	// ----- ------ SIMPLE BLOBS
			absdiff(grayImg, prevImg, diffImg);
			blur(diffImg, diffImg, 11);
			diffImg = ampli*diffImg;
			threshold(diffImg, diff_thresh);

			toOf(diffImg, nImg);
			nImg.update();

			// update image
			//prevImg = diffImg;
			prevImg = grayImg.clone();

			if (en_track == true) {
				//cFinder.setThreshold(contour_thresh);
				cFinder.findContours(nImg);
				tracker.track(cFinder.getBoundingRects());

				fbo_C.begin();
				ofClear(0, 0);
				// draw base
				nImg.draw(0, 0);
				// then track
				ofNoFill();
				int n = cFinder.size();
				for (int i = 0; i < n; i++) {
					// rect and shapes
					ofSetLineWidth(2);
					ofSetColor(ofColor::blueSteel, 205);
					cv::Rect r = cFinder.getBoundingRect(i);
					ofDrawRectangle(r.x, r.y, r.width, r.height);
					ofSetLineWidth(4);
					//nColor = ofColor::fromHsb((i)%255,255,255, 200);
					nColor = ofColor(255, 127+(i%128));
					ofSetColor(nColor);
					ofPolyline shape = cFinder.getPolyline(i);
					shape.draw();
					// labels
					ofPushMatrix();
					ofTranslate(r.x, (r.y + r.height + 20)<ih?(r.y + r.height + 20): (r.y - r.height));
					int label = cFinder.getLabel(i);
					float ar = cFinder.getContourArea(i);
					ofVec2f velocity = toOf(cFinder.getVelocity(i));
					
					std::stringstream st;
					int ag = trackerOpto.getAge(label);
					st << "x/: " << r.x << "\ty/: " << r.y << endl;
					st << "vx/: " << velocity.x << "\tvy/: " << velocity.y << endl;
					st << "ar/: " << ar << "\tsz/: " << ofRandom(0,1.0) << endl;
					st << "n/: " << ofToString(label) << endl;//"\t[t]: " << ofToString(ag) << endl;

					ofSetColor(ofColor::whiteSmoke);
					ofSetLineWidth(1);
					//ofDrawBitmapString(st.str(), 0, 0);
					ofPopMatrix();
				}
				// update paths
				/*/ dont
				vector<Stuff>& followers = tracker.getFollowers();
				for (int i = 0; i < followers.size(); i++) {
					followers[i].draw();
				}*/
				fbo_C.end();
				
				// OSC semd sBlobs
				if (send_sBlobs == true) {
					ofxOscMessage m;
					m.setAddress("/ce_player/sBlobs/area");
					m.addIntArg(int(n));
					for (int i = 0; i < n; i++) {
						m.addIntArg(int(cFinder.getContourArea(i)));
					}
					osc_sender.sendMessage(m);
				}

				// the partiture
				vector<Stuff>& more_followers = tracker.getFollowers();
				//-cout << "[THOSE ALIVE]: ";
				for (int i = 0; i < more_followers.size(); i++) {
					// if is undead
					auxLabel = more_followers[i].getLabel();
					//-cout << auxLabel << "/";
					// check if exists
					isHere = false;
					for (int j = 0; j < theQueue.size(); j++) {
						if (auxLabel == theQueue[j]) {
							isHere = true;
							break;
						}
					}
					// if not exists insert
					if (!isHere) {
						for (int j = 0; j < theQueue.size(); j++) {
							if (theQueue[j] == -1) {
								theQueue[j] = auxLabel;
								break;
							}
						}
					}
				}
				for (int j = 0; j < theQueue.size(); j++) {
					isHere = false;
					for (int i = 0; i < more_followers.size(); i++) {
						auxLabel = more_followers[i].getLabel();
						/*
						int vertexSize = more_followers[i].all.size();
						ofVec2f pastPoint = more_followers[i].all[vertexSize>1?(vertexSize-1):0];
						ofVec2f curPoint = more_followers[i].cur;
						*/
						if (auxLabel == theQueue[j]) {
							isHere = true;
							break;
						}
					}
					if (!isHere) {
						theQueue[j] = -1;
					}
				}
				//-cout << endl;
				//ofNoFill();
				fbo_D.begin();
				//ofClear(0, 0);
				ofSetLineWidth(2);
				pX = index1 * 5;
				for (int j = 0; j < theQueue.size(); j++) {
					pY = 50 + j * 10;
					ofSetColor(0);  // color
					ofDrawRectangle(pX, pY, 5, 10);
					if (theQueue[j] > 0) {
						ofSetColor(255);
						ofDrawRectangle(pX, pY, 5, 10);
					}
				}
				ofSetColor(0);  // black rect
				ofDrawRectangle(pX + 5, 20, 10, ih);

				ofSetLineWidth(1);
				ofSetColor(0 , 0, 255, 100);
				ofDrawLine(pX + 5, 50, pX + 5, ih);
				fbo_D.end();
				/*
				cout << "[QUEUE]: ";
				for (int j = 0; j < theQueue.size(); j++) {
					cout << theQueue[j] << "/";
				}
				cout << endl;
				*/
			}
		}
		blackFlag = false;
	} // end blackFlag
	// draw the other fBO
	fbo_A.begin();
	ofClear(0, 0);
	//if (en_edges == true) { edge.draw(0, 0); }
	//else { imgList[index1].draw(0, 0); }
	imgList[index1].draw(0, 0);
	fbo_A.end();

}

//--------------------------------------------------------------
void ofApp::draw(){
	/* normal version
	ofSetColor(255, 255);
	fbo_A.draw(0, 0);
	ofSetColor(255, 255);
	fbo_B.draw(w/2, 0);
	ofSetColor(255, 255);
	fbo_C.draw(w / 2, h/2);
	ofSetColor(255, 255);
	fbo_D.draw(0, h/2);

	// r + sh
	if (enable_record==true) {
		tImg.grabScreen(0, h/2+20, w/2, h/2);
		img_recorder.addFrame(tImg);
	}*/

	// mod render version
	ofSetColor(255, 255);
	fbo_A.draw(0, 0);
	ofSetColor(255, 255);
	fbo_B.draw(0, 0);
	// r + sh
	if (enable_record == true) {
		tImg.grabScreen(0, 0, 1280, 720);
		img_recorder.addFrame(tImg);
	}

	// normal gui
	if (showGui) {
		ofDrawBitmapStringHighlight("[" + ofToString(index1) + "]", 10, ih - 20);
		gui.draw();
	}
}





//--------------------------------------------------------------
void ofApp::load_dir() {
	// ... ... ... --- --- ... ... ... --- --- ... ... --- files
	ofFileDialogResult result = ofSystemLoadDialog("Selecciona un nuevo archivo");
	if (result.bSuccess) {
		string filepath = result.getPath();
		string path = filepath.substr(0, filepath.rfind("\\"));
		while (path.find("\\") != string::npos) path.replace(path.find("\\"), 1, "/");
		cout << "[filepath]: " << filepath << endl;
		cout << "[path]: " << path << endl;
		nImgs = dir.listDir(path);
		cout << "[loading]: \tw/ " << nImgs << "files: " << endl;
		imgList.resize(nImgs);
		for (int i = 0; i < nImgs - 1; i++) {
			string fn = dir.getPath(i);
			imgList[i].load(fn);
			cout << "\t." << i << ".";
		}; cout << endl;
		bLoaded = true;
	}
	if (nImgs > 0) {
		iw = imgList[0].getWidth();
		ih = imgList[0].getHeight();
	}
	else {
		iw = 0;
		ih = 0;
	}

	// features 
	feats.resize(0); // mesh
	for (int ix = 0; ix <= iw; ix += gridSz) {
		for (int iy = 0; iy <= ih; iy += gridSz) {
			feats.push_back(cv::Point2f(ix, iy));
			//LK.setFeaturesToTrack(feats);
		}
	}

	// fbos
	fbo_A.allocate(iw, ih, GL_RGBA);
	fbo_B.allocate(iw, ih, GL_RGBA);
	fbo_C.allocate(iw, ih, GL_RGBA);
	buffer.allocate(iw, ih, GL_RGBA);
	fbo_D.allocate(iw, ih, GL_RGBA);
	nImg.allocate(iw, ih, OF_IMAGE_COLOR_ALPHA);
	optoMap.allocate(iw, ih, OF_IMAGE_COLOR_ALPHA);

	actImg = toCv(imgList[0]);
	cvtColor(actImg, grayImg, CV_RGB2GRAY);
	prevImg = grayImg;
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	switch (key) {
		case 'f':
			ofToggleFullscreen();
			break;
		case 'h':
			showGui = !showGui;
			break;
		case 't':
			if (img_recorder.isThreadRunning()) {
				img_recorder.stopThread();
			}
			else {
				img_recorder.startThread(false, true);
			}
			break;
		case 'r':
			enable_record = !enable_record;
			break;
		}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	if (key == 's') {
		tImg.grabScreen(0, 0, w, h);
		tImg.saveImage("imgs/frame_" + ofToString(t) + ".png");
	}
}

//--------------------------------------------------------------
void ofApp::exit(){ 
	img_recorder.waitForThread();
}


//--------------------------------------------------------------
//--------------------------------------------------------------
const float dyingTime = 1;

void Stuff::setup(const cv::Rect& track) {
	//color = ofColor(255, 0, 0, 220);
	color = ofColor::fromHsb(ofRandom(255), 255, 255, 200);
	cur = toOf(track).getCenter();
	smooth = cur;
}

void Stuff::update(const cv::Rect& track) {
	cur = toOf(track).getCenter();
	smooth.interpolate(cur, .5);
	all.addVertex(smooth);
}

void Stuff::kill() {
	float curTime = ofGetElapsedTimef();
	if (startedDying == 0) {
		startedDying = curTime;
	}
	else if (curTime - startedDying > dyingTime) {
		dead = true;
	}
}

void Stuff::draw() {
	ofPushStyle();
	float size = 16;
	ofSetColor(color);
	ofSetLineWidth(1);
	if (startedDying) {
		ofSetColor(ofColor::red);
		size = ofMap(ofGetElapsedTimef() - startedDying, 0, dyingTime, size, 0, true);
	}
	ofNoFill();
	ofDrawCircle(cur, size);
	ofSetColor(color);
	ofSetLineWidth(3);
	all.draw();
	ofSetColor(255, 212);
	ofDrawBitmapString(ofToString(label), cur);
	ofPopStyle();
}