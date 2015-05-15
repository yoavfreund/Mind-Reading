import java.util.Random;
import java.lang.Math;

public class HagelExpert implements Expert
{
	int wonlast, wonbefore, playedsame;
	int[] a_mem = new int[8];
	int[] b_mem = new int[8];
	int lastpred, last2pred;
	double preddiff;
	Random generator;
  
	public HagelExpert() 
	{
		generator = new Random();  
		//a_mem[] = new int[8];
		//b_mem[] = new int[8];	
		this.reset();
	}
  
	public void update(int cur)
	{
		int s;
		s = 4*wonlast + 2*wonbefore + playedsame;
		wonbefore = wonlast;
		playedsame = (lastpred == last2pred  ? 1 : 0);
		wonlast = (cur == lastpred ? 1 : 0);
		a_mem[s] += (last2pred == cur ? 1 : -1);
		if (a_mem[s] < -3)
			a_mem[s] = -3;
		if (a_mem[s] > 3)
			a_mem[s] = 3;
		b_mem[s] = ((b_mem[s]&1) << 1) | wonlast;
		
		return;
	}
  
	public int predict()
	{
		int s;
		double p;

		s = 4*wonlast + 2*wonbefore + playedsame;
		if (b_mem[s] == 0)   /* if lost last two times, predict randomly */
			preddiff = 0.5;
		else if (b_mem[s] == 3) /* if won last two times predict what's in a_mem */
			preddiff = (a_mem[s] > 0 ? 0.0 : (a_mem[s] < 0 ? 1.0 : 0.5));
		else /* if won one of last two times, then 3-1 odds */
			/* predict what's in a_mem */
			preddiff = (a_mem[s] > 0 ? 0.25 : (a_mem[s] < 0 ? 0.75 : 0.5));
		p = Math.abs(lastpred - preddiff);

		last2pred = lastpred;
		lastpred = (generator.nextDouble() > p ? 0 : 1); /*returns 1 with probability p*/
                //lastpred = (p >= .5 ? 1 : 0);
		return lastpred; 
	}
  
	public void reset() 
	{
		for (int s = 0; s < 8; s++) 
		{
			a_mem[s] = 0;
			b_mem[s] = 0;
		}
		wonlast = 0;
		wonbefore = 0;
		playedsame = 0;
		lastpred = 0;
	}
}
