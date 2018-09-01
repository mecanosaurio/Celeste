import peasy.PeasyCam;
import oscP5.*;
import netP5.*;
  
PeasyCam cam;
OscP5 oscP5;
NetAddress remoteOscClient;

PGraphics pgA, pgB;
JSONArray data;
Cell [] cells = new Cell[170];

int iii;
float ang, siz;
int off_x, off_y, off_z;
int coo_x, coo_y, coo_z;
boolean isUpd;

JSONObject frame;
int index;
String fn;
JSONArray opto;
JSONArray clus; 
int sector;
String label;
int track;
PFont font;


// ---   ---   ----   --- ---   --- ZETUP
void setup(){
  //size(1920, 1080, P3D);
  //background(64, 202, 255);
  fullScreen(P3D);
  frameRate(12);
  noCursor();
  thread("initOSC");
  font = loadFont("Pixeled-20.vlw");
  //textFont(font);
  off_x = width/4;
  off_z = -200;
  off_y = -200;
  iii = 0;
  //- load data
  data = loadJSONArray("./db_12h_mid.json");
  println("[JSONload]: ",data.size(), "entries");
  thread("initCells");
  //- create cam
  blendMode(BLEND);
  pgA = createGraphics(100, height, P3D);
  cam = new PeasyCam(this, 1100);
  isUpd = false;
  sector = 84;
}




// ---   ---   ----   --- ---   --- DROUQ
void draw(){
  background(0);
  //background(64, 202, 255); //(2, 37, 48);
  
  //- * sometimes
  if (frameCount%48==47 ){
    //-- update data
    frame = data.getJSONObject(iii);
    index = frame.getInt("index");
    fn = frame.getString("filename");
    opto = frame.getJSONArray("opticflow");
    for (int j=0; j<opto.size(); j++){
      clus = opto.getJSONArray(j);
      ang = clus.getFloat(4);
      siz = clus.getFloat(5);
      cells[j].setAngle(ang);
      cells[j].setSize(siz);
      //println(ang, siz);
      if(j==sector){
        sendOSC(index, j, ang, siz);
      }
    }  
    /*/-- update the pg
    pgA.beginDraw();
    pgA.background(111, 215, 255, 126);
    pgA.fill(255);
    pgA.textFont(font);
    //pgA.textSize(25);
    pgA.pushMatrix();
    pgA.rotateZ(PI/2);
    pgA.text("[sector]: "+ str(sector) , height/2 - 420 , -35);
    pgA.text("[a]: "+ nf(cells[sector].angle, 3, 2) , height/2 - 70 , -35);
    pgA.text("[s]: "+ nf(cells[sector].size, 3, 2) , height/2 + 250 , -35);
    pgA.popMatrix();
    pgA.endDraw();*/   
    //-- the counts
    println(iii, index, fn);
    iii++;
    if (iii>=data.size()){
      iii=0;
    }
    isUpd = true;
  }
  
  //- * always
  if (isUpd){
    //-- update cells 
    for (int j=0; j<opto.size(); j++){
      cells[j].update();
    }
    //-- then move and draw
    pushMatrix();
    translate(0,0, -300 + sin(frameCount%25500 * TWO_PI/25500) * 600);
    rotateX(frameCount%32300 * TWO_PI/32300);
    for (int j=0; j<opto.size(); j++){
      cells[j].display();
    }
    popMatrix();   
    //-- update the pg
    pgA.beginDraw();
    //pgA.background(64, 202, 255, 126);
    pgA.background(0,64);
    pgA.fill(255);
    pgA.textFont(font);
    pgA.pushMatrix();
    pgA.rotateZ(PI/2);
    pgA.text("[sector]: "+ str(index)+"."+str(sector) , height/2 - 470 , -35);
    pgA.text("[a]: "+ nf(cells[sector].angle, 3, 2) , height/2 - 160 , -35);
    pgA.text("[f]: "+ nf(cells[sector].size, 3, 2) , height/2 + 65 , -35);
    pgA.text("[t]: "+ nf(millis(), 7) , height/2 + 280 , -35);
    pgA.popMatrix();
    pgA.endDraw();
    //-- add th e buff
    image(pgA, -width/2-150, -height/2);
  }
}







void getData(){
  try {
    data = loadJSONArray("./db_12h_640x360_40sz_11mb.json");
    println("[JSONload]: ",data.size(), "entries");
  } catch( Exception e ) {
  }
}

void initOSC(){
  // receiver
  oscP5 = new OscP5(this, 11111);
  // sender
  remoteOscClient = new NetAddress("192.168.1.67", 10001);
  println("[OSC]: ok: ", remoteOscClient); 
}

void oscEvent(OscMessage oscMsg) {
  print("[OSC.<-]: ");
  print(" addrpattern: "+oscMsg.addrPattern());
  println(" typetag: "+oscMsg.typetag());
  if (oscMsg.checkAddrPattern("/sector")==true){
    if (oscMsg.checkTypetag("i")){
      int nSector = oscMsg.get(0).intValue();
      sector = nSector;
      println("\t[OSC]: ", oscMsg.addrPattern(), sector);
    }
  }
  if (oscMsg.checkAddrPattern("/label")==true){
    if (oscMsg.checkTypetag("s")){
      String lab = oscMsg.get(0).stringValue();
      label = lab;
      println("\t[OSC]: ", oscMsg.addrPattern(), label);
    }
  }
  if (oscMsg.checkAddrPattern("/track")==true){
    if (oscMsg.checkTypetag("i")){
      int tra = oscMsg.get(0).intValue();
      track = tra;
      println("\t[OSC]: ", oscMsg.addrPattern(), track);
    }
  }
}

void sendOSC(int id, int se, float an, float sz){
  OscMessage oscMsg = new OscMessage("/cloud");
  oscMsg.add(id);
  oscMsg.add(se);
  oscMsg.add(an);
  oscMsg.add(sz);
  oscP5.send(oscMsg, remoteOscClient); 
  println("[OSC.->]: ", oscMsg.addrPattern(),id,se,an,sz);
}

void initCells(){
  JSONObject firstFrame = data.getJSONObject(0);
  JSONArray firstOpto = firstFrame.getJSONArray("opticflow");
  for (int i=0; i<firstOpto.size(); i++){
    JSONArray clus = firstOpto.getJSONArray(i); 
    cells[i] = new Cell(off_x, 
                      off_y + clus.getInt(1), 
                      off_z  + clus.getInt(0), 
                      clus.getFloat(5), 
                      clus.getFloat(4)
                      );
  }
  println ("complete cells:", firstOpto.size());
}


// This function returns all the files in a directory as an array of Strings  
String[] listFileNames(String dir) {
  File file = new File(dir);
  if (file.isDirectory()) {
    String names[] = file.list();
    return names;
  } else {
    // If it's not a directory
    return null;
  }
}
