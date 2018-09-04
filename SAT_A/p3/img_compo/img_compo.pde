/**
 * Image looper
 *
 */

PImage img, workImg;
float tolerance=0.13;


int nImgs = 2150;
int chunkSz = 5;
int pivot = 10;
PImage[] imgs = new PImage[chunkSz];
PImage[] imgs_dit = new PImage[chunkSz];
PImage[] imgs_flow = new PImage[chunkSz];
float imgW;
int index_i;
boolean img_loaded;
long t0, t1;

float scale = 0.8;


void setup() {
  size(1080, 1920);
  //fullScreen();
  background(0);
  colorMode(RGB);
  //blendMode(BLEND);
  //tint(255,2);
  noCursor();
  frameRate(12);
  img_loaded = false;
  for (int i = 0; i < chunkSz; i++){
    imgs[i] = requestImage("E:/FR/1809_concierto/DATA/NOI_0901_640x360/clouds_"+nf(i+pivot, 4)+".png");
    imgs_dit[i] = requestImage("E:/FR/1809_concierto/DATA/NOI_0901_640x360_DITH/clouds_"+nf(i+pivot, 4)+".png");
    imgs_flow[i] = requestImage("E:/FR/1809_concierto/DATA/NOI_0901_640x360_FLOW/clouds_"+nf(i+pivot, 4)+".png");
    //imgs[i] = requestImage("/home/pi/W/DATA/NOI_0901_640x360/clouds_"+nf(i+pivot, 4)+".png");
    //imgs_dit[i] = requestImage("/home/pi/W/DATA/NOI_0901_640x360_DITH/clouds_"+nf(i+pivot, 4)+".png");
    //imgs_flow[i] = requestImage("/home/pi/W/DATA/NOI_0901_640x360_FLOW/clouds_"+nf(i+pivot, 4)+".png");
  }
  // wait to show the first img 
  index_i = 0;
  while (!img_loaded){
    if ((imgs[0].width != 0) && (imgs[0].width != -1)){
      image(imgs[0], 
          width/2 - imgs[0].width * scale/2, 
          height/2 - imgs[0].height * scale/2,
          imgs[0].width * scale, 
          imgs[0].height * scale);
      img_loaded = true;
    } else {
      delay(100);
    }
  }
}




void draw(){
  img = imgs[index_i];
  float imgW = img.width;
  float imgH = img.height;

  // show
  //background(0);
  pushMatrix();
  translate(width/2, height/2);
  tint(255);  
  scale(1,1);
  image(imgs_dit[index_i], 0, 0, imgW*scale, imgH*scale);
  scale(-1,1);
  image(imgs_dit[index_i], 0, 0, imgW*scale, imgH*scale);
  scale(1,-1);
  image(imgs_dit[index_i], 0, 0, imgW*scale, imgH*scale);
  scale(-1,1);
  image(imgs_dit[index_i], 0, 0, imgW*scale, imgH*scale);
  popMatrix();

  
  
  // this is the sup
  tint(255, 32);
  pushMatrix();
  translate(width/2 , height/2 );
  scale(1,1);
  image(imgs[index_i], -imgW*scale, -2*imgH * scale - 40, imgW * scale, imgH * scale);
  scale(-1,1);
  image(imgs[index_i], -imgW*scale, -2*imgH * scale - 40, imgW * scale, imgH * scale);
  scale(1,-1);
  image(imgs[index_i], -imgW*scale, -2*imgH * scale - 40, imgW * scale, imgH * scale);
  scale(-1,1);
  image(imgs[index_i], -imgW*scale, -2*imgH * scale - 40, imgW * scale, imgH * scale);
  popMatrix();


  // update index
  index_i += 1;
  if (index_i >= chunkSz){
    index_i = 0;
  } else if (index_i < 0){
    index_i = chunkSz-1;
  }
  if (frameCount%25 == 0){
    reloadImgs();
  }
}


void keyPressed(){
  if (key=='f'){
  }
  if (key=='s'){
    saveFrame("cloud_###.png");
  }
}



void reloadImgs(){
  pivot = int(random(nImgs/chunkSz - 1)* chunkSz) ;
  print ("pivot: ", pivot);
  for (int i = 0; i < chunkSz; i++){
    imgs[i] = requestImage("E:/FR/1809_concierto/DATA/NOI_0901_640x360/clouds_"+nf(i+pivot, 4)+".png");
    imgs_dit[i] = requestImage("E:/FR/1809_concierto/DATA/NOI_0901_640x360_DITH/clouds_"+nf(i+pivot, 4)+".png");
    imgs_flow[i] = requestImage("E:/FR/1809_concierto/DATA/NOI_0901_640x360_FLOW/clouds_"+nf(i+pivot, 4)+".png");
    //imgs[i] = requestImage("/home/pi/W/DATA/NOI_0901_640x360/clouds_"+nf(i+pivot, 4)+".png");
    //imgs_dit[i] = requestImage("/home/pi/W/DATA/NOI_0901_640x360_DITH/clouds_"+nf(i+pivot, 4)+".png");
    //imgs_flow[i] = requestImage("/home/pi/W/DATA/NOI_0901_640x360_FLOW/clouds_"+nf(i+pivot, 4)+".png");
 }
  delay(200);
}
