import java.util.Random;

public class ShannonExpert implements Expert{
  int[][][] inputs;
  int last_1,last_2;
  Random generator;
  
  public ShannonExpert() {
    inputs = new int[2][2][2];
    last_1 = 0;
    last_2 = 0;
    generator = new Random(); 
    this.reset();
  }
  
  public void update(int cur) {
   if (inputs[last_2][last_1][0] == cur) {
      inputs[last_2][last_1][1] = 1;
      inputs[last_2][last_1][0] = cur;
    }
    else {
      inputs[last_2][last_1][1] = 0;
      inputs[last_2][last_1][0] = cur;
    }

    last_2 = last_1;
    last_1 = cur; 
  }
  
  public int predict() {
   int pred = 0;
   if (inputs[last_2][last_1][1] == 1) {
      pred = inputs[last_2][last_1][0];
    } 
    else {
      pred = generator.nextInt(2);
    }
    return pred; 
  }
  
  public void reset() {
     for (int i=0; i<2; i++) {
      for (int j=0; j<2; j++) {
        for (int k=0; k<2; k++) {
          inputs[i][j][k]=0;
        }
      }
    }

    last_1 = generator.nextInt(2);
    last_2 = generator.nextInt(2);
  }


}
