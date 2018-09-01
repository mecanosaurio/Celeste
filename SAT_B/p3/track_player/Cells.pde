class Cell{
  float x, y, z;
  float size;
  float angle;
  float nAn, nSz;
  color colorFill, colorStroke;
  
  Cell(float iniX, float iniY, float iniZ, float iniSz, float iniAn){
    x = iniX;
    y = iniY;
    z = iniZ;
    size = iniSz;
    angle = iniAn;
    nSz = iniSz;
    nAn = iniAn;
    colorFill = color(255, 64);
    colorStroke = color(255, 128);
  }
    
  void setAngle(float na){
    nAn = na;
  }

  void setSize(float ns){
    nSz = ns;
  }

  void setColors(color ncf, color ncs){
    colorFill = ncf;
    colorStroke = ncs;
  }
  
  void update(){
    float rateSz = 0.3;
    float rateAn = 0.15;
    size = lerp(size, nSz, rateSz);
    angle = lerp(angle, nAn, rateAn);
  }
  
  void display(){
    // draw cube, 
    noFill();
    strokeWeight(1);

    pushMatrix();
    translate(x, y, z);
    int kkk = int(map(abs(angle), 0, 180, 0, 3));
    //print(kkk,"_ ");
    for (int k=0; k < kkk; k++){
      stroke(255, map(int(abs(angle))%60, 0, 60, 0, 64));
      //stroke(255);
      pushMatrix();
      translate(-40*k, 0, 0);
      box(40);    
      translate(2*40*k, 0, 0);
      box(40);    
      popMatrix();
    }
    popMatrix();
    
    strokeWeight(1);
    color cos = color(255);
    if (angle>0 && angle<=90) cos = color(200, 216, 222, map(size, 2, 30, 0, 124));
    else if (angle>90 && angle<=180) cos = color(207, 207, 237, map(size, 2, 30, 0, 124));
    else if (angle>-180 && angle<=-90) cos = color(111, 215, 255, map(size, 2, 30, 0, 124));
    else if (angle>-90 && angle<=0) cos=color(52, 237, 255, map(size, 2, 30, 0, 124));
    stroke(cos);
    //fill(cos);
    fill(255, constrain(map(abs(angle)%45, 0, 45, 0, 151), 0, 151));

    pushMatrix();
    strokeWeight(2);
    translate(x, y, z);
    box(constrain(size*2, 0, 40));
    popMatrix();
  }
}
