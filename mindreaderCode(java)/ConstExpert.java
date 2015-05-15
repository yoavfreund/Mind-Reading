import java.util.Random;
/* Predicts with the last input */

public class ConstExpert implements Expert
{
  int last;
  Random generator;
  
  public ConstExpert() {
    generator = new Random();  
    this.reset();
  }
  
  public void update(int cur){
    last = cur;  
  }
  
  public int predict(){
    return last;
  }
  
  public void reset() {
    last = generator.nextInt(2);
  }
}
