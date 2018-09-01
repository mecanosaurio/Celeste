/**
 * Image looper
 *
 */

import toxi.color.*;
import toxi.math.*;
import java.util.Iterator;
PImage img, workImg;
float tolerance=0.13;


int nImgs = 2150;
int chunkSz = 50;
int pivot = 10;
PImage[] imgs = new PImage[chunkSz];
PImage[] imgs_dit = new PImage[chunkSz];
float imgW;
int index_i;
boolean img_loaded;
long t0, t1;

float scale = 1.0;


void setup() {
  //size(1080, 1920);
  fullScreen();
  noCursor();
  frameRate(12);
  img_loaded = false;
  for (int i = 0; i < chunkSz; i++){
    imgs[i] = requestImage("E:/FR/1809_concierto/DATA/12h/a"+nf(i+pivot, 4)+".png");
    imgs_dit[i] = requestImage("E:/FR/1809_concierto/DATA/12h_dit/b-"+nf(i+pivot, 4)+".png");
    //imgs[i] = requestImage("/home/pi/W/DATA/12h/a"+nf(i+pivot, 4)+".png");
    //imgs_dit[i] = requestImage("/home/pi/W/DATA/12h_dit/b-"+nf(i+pivot, 4)+".png");
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
  workImg = imgs_dit[index_i];

  // show
  background(0);
  image(imgs[index_i], 
      width/2 - imgs[index_i].width * scale/2, 
      height/2 - imgs[index_i].height * scale/2 - 600, 
      imgs[index_i].width * scale, 
      imgs[index_i].height * scale);
  
  image(imgs_dit[index_i], 
      width/2 - imgs_dit[index_i].width * scale/2, 
      height/2 - imgs_dit[index_i].height * scale/2 + 400, 
      imgs_dit[index_i].width * scale, 
      imgs_dit[index_i].height * scale);


  // update index
  index_i += 1;
  if (index_i >= chunkSz){
    index_i = 0;
  } else if (index_i < 0){
    index_i = chunkSz-1;
  }
  if (frameCount%500 == 0){
    reloadImgs();
  }
}


void keyPressed(){
  if (key=='f'){
  }
}



void reloadImgs(){
  pivot = int(random(nImgs/chunkSz - 1)* chunkSz) ;
  print ("pivot: ", pivot);
  for (int i = 0; i < chunkSz; i++){
    imgs[i] = requestImage("E:/FR/1809_concierto/DATA/12h/a"+nf(i+pivot, 4)+".png");
    imgs_dit[i] = requestImage("E:/FR/1809_concierto/DATA/12h_dit/b-"+nf(i+pivot, 4)+".png");
    //imgs[i] = requestImage("/home/pi/W/DATA/12h/a"+nf(i+pivot, 4)+".png");
    //imgs_dit[i] = requestImage("/home/pi/W/DATA/12h_dit/b-"+nf(i+pivot, 4)+".png");
 }
  delay(200);
}
