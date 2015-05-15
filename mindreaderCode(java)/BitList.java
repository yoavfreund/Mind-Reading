public class BitList
{
  public BitNode current;

  public BitList()
  {
    current = null;
  }

  void addbit(int b) 
  {
    BitNode temp;
    if(current==null) 
    {
      current = new BitNode(b, null);
    }
    else 
    {
      temp = new BitNode(b,current);
      current = temp;
    }
  }
}
