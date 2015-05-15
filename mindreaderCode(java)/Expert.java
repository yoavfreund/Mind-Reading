public interface Expert
{
    /* update the state of the expert given current sequence element. (selected by user) */
	public void update(int cur);
    /* Produce the prediction for the next bit (0 or 1) */
	public int predict();
    /* reset expert to original state */
	public void reset();
        public String getID();
}
