import java.io.File;

int tileWidth = 128;
int chunkWidth = tileWidth + 64;

float quality = 7.5;

PImage orig;
PImage thumb;

PrintWriter luaOutput;
//PrintWriter hOutput;

String fname;

String typedText = "";

PFont font;

int qgood = 0;

void setup() {
  size(400, 400);
  font = createFont("Helvetica", 18);
  luaOutput = createWriter((dataPath("")+"/silt/assets.lua"));
  //hOutput = createWriter("dims.h");
  selectInput("Select an image", "slice");
}



void slice(File selection) {
  if (selection == null) {
    println("Window was closed or the user hit cancel.");
  } 
  else {
    fname = selection.getName();
    println("User selected " + fname);
    orig = loadImage(selection.getAbsolutePath());
    thumb = loadImage(selection.getAbsolutePath());
    thumb.resize(300,0);
  }
}

void draw() {
  background(153);
  fill(0, 0, 255);
  textFont(font, 18);
  if (thumb == null) {
  } 

  else {
    image(thumb, 50, 50);
    text("Select Quality (0-10):"+typedText+(frameCount/10 % 2 == 0 ? "_" : ""), 50, thumb.height + 70);
    if (typedText.length() > 0) {
      if (Float.isNaN(float(typedText))) {
        text("Not a valid quality factor!", 50 , thumb.height + 100);
        qgood = 0;
      }
      else {
        quality = constrain(float(typedText), 0, 10);
        qgood = 1;
      }
    }
  }
}

void keyReleased() {
  if (key != CODED) {
    switch(key) {
    case BACKSPACE:
      typedText = typedText.substring(0, max(0, typedText.length()-1));
      break;
    case TAB:
      //typedText += "    ";
      break;
    case ENTER:
    case RETURN:
      // comment out the following two lines to disable line-breaks
      //typedText += "\n";
      //break;
      if (qgood == 1) {
        int pixlim = 800000;
        int pixnum = orig.width * orig.height;
        float newW = orig.width / sqrt(pixnum/pixlim);
        float newH = orig.height / sqrt(pixnum/pixlim);

        println(pixnum + " " + newW + "  " + newH + " " + newW*newH);

        if (pixnum > pixlim) {
          orig.resize(int(newW), int(newH));
        } 

        orig.resize((orig.width - orig.width % 8), (orig.height - orig.height % 8));

        orig.save((dataPath("")+"/silt/giga.png"));


        /*    int cols = orig.width / chunkWidth;
         int rows = orig.height / chunkWidth;
         
         int idx;
         int idy;
         
         if ((orig.width - cols * chunkWidth) > (orig.height - rows*chunkWidth)) {
         orig.resize(0, (rows)*chunkWidth);
         }
         else {
         orig.resize((cols)*chunkWidth, 0);
         }
         
         for (int r = 1; r <= rows; r = r + 1) {
         for (int c = 1; c <= cols; c = c + 1) {
         idx = (c-1) * chunkWidth;
         idy = (r-1) * chunkWidth;
         crop = orig.get(idx, idy, tileWidth, tileWidth);
         image(crop, idx, idy);
         crop.save("chunks/"+fname.substring(0,fname.indexOf("."))+"r"+r+"c"+c+".png");
         }
         }
         
         */
        luaOutput.println("-- Metadata");
        luaOutput.println("IconAssets = group{quality=9.95}");
        luaOutput.println("Icon = image{\"icon.png\"}");
        luaOutput.println(" ");
        /*   
         luaOutput.println("function frames(fmt, count1, count2)");
         luaOutput.println("  t = {}");
         luaOutput.println("  for i = 1, count1 do");
         luaOutput.println("    for j = 1, count2 do");
         luaOutput.println("      t[1+#t] = string.format(fmt, i, j)");
         luaOutput.println("    end");
         luaOutput.println("  end");
         luaOutput.println("  return t");
         luaOutput.println("end");
         luaOutput.println(" ");
         */

        luaOutput.println("-- Animation: \"giga\"");
        luaOutput.println("gigaGroup = group{quality="+quality+"}");
        luaOutput.println("giga = image{\"giga.png\"}");
        luaOutput.flush();
        luaOutput.close();

        //   hOutput.println("#define NUM_ROWS "+rows);
        //   hOutput.println("#define NUM_COLS "+cols);
        //   hOutput.flush();
        //   hOutput.close();    

        String cmdPath = (dataPath("")+"/SDK/gviewer.command");
        
        //cmdPath = dataPath("") + params;
        
        println(cmdPath);

        open(cmdPath);
        exit();
      }



    case ESC:
    case DELETE:
      break;
    default:
      typedText += key;
    }
  }
}


//    noLoop();



//  }
//}

