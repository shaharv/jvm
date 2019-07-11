package test;

class Main
{
	static public void main(String[] args) throws InterruptedException
	{
		for (int i = 3; i >= 1; i--)
		{
			System.out.println("Test #" + i);
			Thread.sleep(1000);
		}
	}
}
