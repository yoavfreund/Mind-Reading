import java.io.*;

public class DutchNode {

  DutchNode[] son;
  double[] wt;
  double end_wt;
  double sum_wt;

  public DutchNode() 
  {
    son = new DutchNode[2];
    wt = new double[2];
    son[0]	= null;
    son[1]	= null;
    wt[0]	= 1.;
    wt[1]	= 1.;
    end_wt	= 1.;
    sum_wt	= 1.;
  }

  void update_wt(BitNode ex, int y, double beta)
  {
    wt[1-y] *= beta;

    if (ex == null)
      end_wt = 0.5*(1.+beta);
    else
      son[ex.bit].update_wt(ex.next, y, beta);

    sum_wt = 0.25*(wt[0] + wt[1]) + 0.5 * sonwt(0) * sonwt(1) * end_wt;
  }


  double sonwt(int s) 
  {
    double son_sum_wt;
    if (son[s] == null)	
      son_sum_wt = 1;
    else
      son_sum_wt = son[s].sum_wt;
    return son_sum_wt;
  }

  double get_pred_wt(BitNode ex)
  {

    if (ex == null)
      return 0.25 * (wt[1] + sonwt(0) * sonwt(1));
    else 
    {
      int b = ex.bit;
      if (son[b] == null)
        son[b] = new DutchNode();

      return 0.25*wt[1]+0.5*sonwt(1-b)*son[b].get_pred_wt(ex.next)*end_wt;
    }
  }

}
