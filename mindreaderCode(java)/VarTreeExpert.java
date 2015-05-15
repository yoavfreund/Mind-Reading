import java.lang.Math;
import java.util.Random;
import java.io.*;

public class VarTreeExpert implements Expert 
{
	double BETA = .55;
	BitList bits;
	DutchNode tree;
        Random generator;
        
	public VarTreeExpert() 
	{
		this.reset();
	}

        public String getID() {
               return "VarTreeExpert"; 
        }

	public void update(int cur) 
	{
		tree.update_wt(bits.current, cur, BETA);
		bits.addbit(cur);
	}
	public int predict()
	{
		double r;
                int out;
		r = tree.get_pred_wt(bits.current)/(tree.sum_wt);
                out = (generator.nextDouble() > r ? 0 : 1);
		//out = (r > .5? 1 : 0);
                System.out.println("pred..."+r);
                return out;
	}

	public void reset()
	{
		tree = new DutchNode();
		bits = new BitList();
                generator = new Random();  
	}

	double F(double r) 
	{
		double cbeta = -1.0;

		if (cbeta < 0.)
			cbeta = (BETA == 1.0 ? 0.5 : (1.+BETA)*Math.log(2./(1.+BETA)) / (2.*(1.-BETA)));
		return (r <= 0.5 - cbeta ? 0. :
			(r >= 0.5 + cbeta ? 1. :
			0.5 * (1. - (1. - 2.*r)/(2.*cbeta))));
	}
}
