import processing.core.*; import java.net.Socket; import java.net.InetAddress; import java.io.IOException; import java.io.OutputStreamWriter; import java.io.BufferedWriter; import java.applet.*; import java.awt.*; import java.awt.image.*; import java.awt.event.*; import java.io.*; import java.net.*; import java.text.*; import java.util.*; import java.util.zip.*; import javax.sound.midi.*; import javax.sound.midi.spi.*; import javax.sound.sampled.*; import javax.sound.sampled.spi.*; import java.util.regex.*; import javax.xml.parsers.*; import javax.xml.transform.*; import javax.xml.transform.dom.*; import javax.xml.transform.sax.*; import javax.xml.transform.stream.*; import org.xml.sax.*; import org.xml.sax.ext.*; import org.xml.sax.helpers.*; 





    //Find where information/Data is being stored
    //Output Predictions Array to command line
    //Draw Predictions Array to screen when "r" pressed

    //Outp user sequence to screen, when "r" pressed

// Shannon's Mind Reading Machine - A Race
//
// Built using tools from http://processing.org

public class mindreader extends PApplet {

  int i,j,k;
  int[][][] inputs = new int[2][2][2];
  int last_1,last_2,current;
  int predict;
  int endOfGame = 0;
  int comwins = 0, humwins=0;
  int initsetup = 1; //before the game starts, we need the people to click once.
  boolean showpred = false; //whether or not to show the computer's prediction
  boolean cheating = false; //at some point in the game, did the user look at the predictions?
  boolean showstats= false; //show the stats or not
  int[][] sequence = new int[200][2];
  int curgameindex = 0;
  Expert expert;
  String error = "";  //debug message to display on top of applet
  String username = "";
  String[] stats;
  PFont font1;
  PFont font2;  
  float starttime = 0, totalTime = 0;
  Random generator;
  public void setup()
  {
    size(600, 750);
    //framerate(5);
    ellipseMode(CENTER);
    generator = new Random();
    font1 = loadFont("CourierNewPSMT-14.vlw");   
    font2 = loadFont("AgencyFB-Bold-24.vlw"); 
    //expert = new HagelExpert();
    //expert = new ShannonExpert();
    //expert = new ConstExpert();
    //expert = new VarTreeExpert();

    expert = new VarTreeExpert();
    stats = new String[22]; //////////////////////////notice hardcoded number here
    reset();
    noLoop();
  }

  public void draw()
  {
    drawbgstuff();
    drawplayers();
    if(comwins>=100 || humwins>=100) {
      drawwinner();
    }
    if(showpred) {
      drawpredictions();
    }
    if(showstats) {
      drawstats();
    }
    drawsequence();
    noLoop();
  }

  public void drawplayers() {
    int cx=0, cy=0, hx=0, hy=0;

    cx = (int)(300+285*cos(comwins*TWO_PI/100.00f)); //was 50 for 2 laps
    cy = (int)(200+165*sin(comwins*TWO_PI/100.00f));
    hx = (int)(300+285*cos(humwins*TWO_PI/100.00f));
    hy = (int)(200+165*sin(humwins*TWO_PI/100.00f));  
    noStroke();
    fill(200,0,0);
    ellipse(cx,cy,20,20);
    fill(0,0,200);
    ellipse(hx,hy,20,20);
  }

  public void drawwinner() {
    textFont(font2, 24);
    fill(0);
    if (comwins>=100) {
      text("Computer Wins!", 420,430);
      text("Your Score: "+(humwins-comwins), 420,455);
    } 
    else {
      text("You Win!", 420,430);
      text("Your Score: "+(humwins-comwins), 420,455);
    }

    text(" (press 'r' to try again)", 200,430);
  }

  public void drawbgstuff() {
    textFont(font2, 24);

    background(255);

    stroke(100,100,10);
    strokeWeight(3);
    fill(90);
    ellipse(300, 200, 600, 360);
    noStroke();
    fill(0,200,0);
    ellipse(300, 200, 540, 300);

    stroke(255);
    strokeWeight(1);
    line(570,200,600,200);

    noStroke();
    fill(200,0,0);
    ellipse(200, 450, 10, 10);
    text("Computer", 207,460);

    fill(0,0,200);
    ellipse(320, 450, 10, 10);
    text("You", 327,460);

    if(initsetup == 1) {
      fill(0);
      text("Your pseudonym please:",200,160);
      text("Press Enter to start!",200,220);
      fill(200);
      stroke(3);
      rect(200,165,182,30);

      // if(millis()%500<250){  // Only fill cursor half the time
      //   noFill();
      // }
      // else{
      fill(255);
      stroke(0);
      // }
      float rPos;
      // Store the cursor rectangle's position
      rPos = textWidth(username)+210;
      rect(rPos+1, 170, 10, 20);

      fill(0,0,200);
      pushMatrix();
      translate(rPos,190); //rPos,190
      char k;
      for(int i=username.length()-1;i>=0;i--){
        k = username.charAt(i);
        translate(-textWidth(k),0); //-textWid...
        text(k,0,0);
      }
      popMatrix();

    }

    if(cheating) {
      textFont(font1, 14); 
      fill(90);
      text("(you've been peeking! this score won't count!)", 0, 720);
    }


  }

  public void drawpredictions() {
    textFont(font1, 14); 
    fill(90);
    text("Next Prediction: "+str(predict), 0, 700);
    
  }

  public void drawsequence() {
    textFont(font1, 14); 
    fill(90);

    String preds = "Predict:";
    String ins =   "Inputs :";

    for(int i=0; i<curgameindex && i<=67; i++) {
      preds += str(sequence[i][0])+"";
      ins   += str(sequence[i][1])+"";
    }
    text(""+ins,   0, 510); 
    text(""+preds, 0, 525); 


    if(curgameindex>=67) {  
      preds = "Predict:";
      ins =   "Inputs :";
      for(int i=67; i<curgameindex && i<=133; i++) {
        preds += str(sequence[i][0])+"";
        ins   += str(sequence[i][1])+"";
      }
      text(""+ins,   0, 545); 
      text(""+preds, 0, 560); 
    }
   
    if(curgameindex>=134) {  
      preds = "Predict:";
      ins =   "Inputs :";
      for(int i=134; i<curgameindex && i<=200; i++) {
        preds += str(sequence[i][0])+"";
        ins   += str(sequence[i][1])+"";
      }
      text(""+ins,   0, 580); 
      text(""+preds, 0, 595); 
    }

    fill(150);
    text(error, 0, 10);

  }

  public void drawstats() {
    //stats: lines 0-9 are top ten individual scores.
    //10-19 are top ten fraction of wins
    //20 is "comp won x% of the y games played"
    //21 is "ave.score: z"
    textFont(font2, 24);
    fill(0);
    text("High Scores", 160, 100);
    textFont(font2, 14);
    text("Score & Name", 180, 118);
    for(int i=0; i<10; i++) {
      text(""+(i+1)+":", 160, 118+(i+1)*17);
      text(""+stats[i], 180, 118+(i+1)*17);
    } 

    textFont(font2, 24);
    text("!TEST! Highh % of Wins", 310, 100);
    textFont(font2, 14);
    text("Percent (Total # Played)", 330, 118);
    for(int i=0; i<10; i++) {
      text(""+(i+1)+":", 310, 118+(i+1)*17);
      text(""+stats[i+10], 330, 118+(i+1)*17);
    } 

    textFont(font2,24);
    ////text(stats[21], 130, 315); //comp won x% of y games
    //text(stats[20],225,342);  // average score is z
  }

  public void getstats() {
    try {
      Socket sock = new Socket(/*server name here*/);

      //Send Handshake/Stats request
      BufferedWriter dataout;
      String ipMessage = "stats\n";
      dataout = new BufferedWriter (new OutputStreamWriter (sock.getOutputStream ()));
      dataout.write (ipMessage, 0, ipMessage.length ());
      dataout.flush ();

      //Read in stats
      BufferedReader datain = new BufferedReader (new InputStreamReader(sock.getInputStream ()));
    for(int i=0; i<22; i++) { ////////////////////notice the hardcoded number here
     stats[i] = datain.readLine();
      }    
      sock.close ();
      error = "";
    } 
    catch(Exception e) {
      e.printStackTrace();
      error = "submit error:"+e.toString();      
      //error = "Server issues...";
    }

  }


  public void sendData() {  

    String preds = "";
    String ins =   "";
    for(int i=0; i<curgameindex && i<=200; i++) {
      preds += str(sequence[i][0])+"";
      ins   += str(sequence[i][1])+"";

      }

      restartWriteFile();

    String ipMessage = "howdy\n"+username+"\n"+expert.getID()+"\n"+(humwins-comwins)+"\n"+totalTime+"\nP:"+preds+"\nI:"+ins+"\n";

    try {
      Socket sock = new Socket(/*server name here*/);
      BufferedWriter dataout;
      dataout = new BufferedWriter (new OutputStreamWriter (sock.getOutputStream ()));

      dataout.write (ipMessage, 0, ipMessage.length ());
      dataout.flush ();
      endOfGame = 1;
      sock.close ();
      error = "";
    } 
    catch(Exception e) {
      e.printStackTrace();
      //error = "submit error:"+e.toString();      
      error = "Server undergoing some updating...check back in a few!";
    }
  }

  public void process() {
    if (!(comwins>=100 || humwins>=100)) { //stop counting if the game is over.
      if (predict == current) { //comp got it right
        comwins++;
      } 
      else { //we fooled the comp
        humwins++;
      }

      if(comwins==100 || humwins==100) { //if someone just won, tell the server
        totalTime = millis() - starttime;
        //error="TIME="+totalTime;
        if(!cheating) {
          sendData();
        }
        getstats();
        showstats = true;
      }

      sequence[curgameindex][0] = predict; //update historical data
      sequence[curgameindex][1] = current;
      curgameindex++;

      expert.update(current);

      predict = expert.predict();
      println("C=" +comwins+ "\tY="+humwins);
    }
    draw();
  }


  public void restartWriteFile()
  {
    String preds = "";
    String ins =   "";
    for(int i=0; i<curgameindex ; i++) 
    {
      preds += str(sequence[i][0])+"";
      ins   += str(sequence[i][1])+"";
    } 
     try
    {
      endOfGame = 1;
      //Output to file at same time as outputting to screen
      //File outFileName = new File("test.txt");
      
      FileWriter outFileName = new FileWriter("test.txt", true);
      PrintWriter pw = new PrintWriter(outFileName, true);
      
      pw.println ("PREDICTIONS: " + preds); 
      pw.println ("USER INPUT: " + ins); 
      pw.flush();
      pw.close();
      
      /*FileOutputStream  outputStream = new FileOutputStream(outFileName);
      BufferedOutputStream bufferedOutFile = new BufferedOutputStream(outputStream);
      BufferedWriter outFile = new BufferedWriter(new OutputStreamWriter(bufferedOutFile)); 
    
      outFile.write ("PREDICTIONS: " + preds + "\n"); 
      outFile.write ("USER INPUT: " + ins + "\n"); 
      outFile.newLine();
      outFile.flush();
      outFile.close();*/
    }
    catch (IOException e)
    {
    }
  }


  public void keyPressed() 
  {
    if (initsetup == 0) {
      switch(key){
      case '0':
        current = 0;
        process();
        break;
      case '1': 
        current = 1;
        process();
        break;
      case 'm':
        current = (generator.nextDouble()  < .5f ? 0 : 1);
        cheating = true;
        process();
        break;
      case 'r':
        if (endOfGame == 0)
        {
          restartWriteFile();
        }
        
        //Restart game so reset flag
        endOfGame = 0;
        reset();
        break;
      case 's':
        showpred = !showpred;   
        cheating = true;
        break;
      case 'h':
        getstats();
        showstats = !showstats;
        break;
      }
    } 
    else { //initsetup==1!!!
      char k;
      k = (char)key;
      switch(k){
      case 8:
        if(username.length()>0){
          username = username.substring(0,username.length()-1);
        }
        break;
      case 10:
        //Pressed Enter - Game STARTS!!
        initsetup = 0;
        starttime = millis();
        break;
      case 13:  // Avoid special keys
      case 65535:
      case 127:
      case 27:
        break;
      default:
        if(textWidth(username+k)+10 < 150){
          username=username+k; //k+username
        }
        break;
      }
    }
    loop();
  }

  public void reset() {

    //When R is pressed, reset is called by mindreader
    //System.out.println("TESTER");
    initsetup=1;
    humwins = 0;
    comwins = 0; 
    curgameindex = 0;
    showpred = false;
    cheating = false;
    showstats = false;
    sequence = new int[200][2];

    expert.reset();
    predict = expert.predict();
    error="";
    draw();
  }

}

