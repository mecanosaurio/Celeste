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
int chunkSz = 100;
int pivot = 10;
int count = 0;

PImage[] imgs = new PImage[chunkSz];
float imgW;
int index_i;
boolean img_loaded;
long t0, t1;




void setup() {
  size(640, 360);
  //fullScreen();
  noCursor();
  frameRate(6);
  img_loaded = false;
  for (int i = 0; i < chunkSz; i++){
    imgs[i] = requestImage("E:/FR/1804_concierto/DATA/12h/a"+nf(i+pivot, 4)+".png");
    //imgs[i] = requestImage("/home/pi/W/DATA/12h/a"+nf(i+pivot, 4)+".png");
  }
  // wait to show the first img 
  index_i = 0;
  while (!img_loaded){
    if ((imgs[0].width != 0) && (imgs[0].width != -1)){
      image(imgs[0], 
          width/2 - imgs[0].width * 1.5/2, 
          height/2 - imgs[0].height * 1.5/2,
          imgs[0].width * 1.5, 
          imgs[0].height * 1.5);
      img_loaded = true;
    } else {
      delay(100);
    }
  }
  count = 0;
}




void draw(){
  img = imgs[index_i];
  workImg = new PImage(img.width,img.height, ARGB);
  Histogram hist=Histogram.newFromARGBArray(img.pixels, img.pixels.length/10, tolerance, true);
  TColor col=TColor.BLACK.copy();
  for(int i=0; i<img.pixels.length; i++) {
    col.setARGB(img.pixels[i]);
    TColor closest=col;
    float minD=1;
    for(HistEntry e : hist) {
      float d=col.distanceToRGB(e.getColor());
      if (d<minD) {
        minD=d;
        closest=e.getColor();
      }
    }
    workImg.pixels[i]=closest.toARGB();
  }
  workImg.updatePixels();
  //save
  
  
  // show
  background(0);
  /*
  image(imgs[index_i], 
      width/2 - imgs[index_i].width * 1.5/2, 
      height/2 - imgs[index_i].height * 1.5/2 - 500, 
      imgs[index_i].width * 1.5, 
      imgs[index_i].height * 1.5);
  */
  image(workImg, 0,0, imgs[index_i].width, imgs[index_i].height);
  saveFrame("E:/FR/1804_concierto/DATA/12h_dit/b-####.png");

  // update index
  index_i += 1;
  if (index_i >= chunkSz){
    count += chunkSz;
    index_i = 0;
    reloadImgs();
  } else if (index_i < 0){
    index_i = chunkSz-1;
  }
  //if (frameCount%200 == 0){
  //  reloadImgs();
  //}
}


void keyPressed(){
  if (key=='f'){
  }
}



void reloadImgs(){
  //pivot = int(random(nImgs/chunkSz - 1)* chunkSz) ;
  pivot = count;
  print ("pivot: ", pivot);
  for (int i = 0; i < chunkSz; i++){
    imgs[i] = requestImage("E:/FR/1804_concierto/DATA/12h/a"+nf(i+pivot, 4)+".png");
    //imgs[i] = requestImage("/home/pi/W/DATA/12h/a"+nf(i+pivot, 4)+".png");
 }
  delay(200);
}
