package test;

import java.util.Date;
import java.text.DateFormat;
import java.text.SimpleDateFormat;

class Main
{
	static public void main(String[] args) throws InterruptedException
	{
		DateFormat dateFormat = new SimpleDateFormat("yyyy/MM/dd HH:mm:ss");
		Date date = new Date();
		System.out.println(dateFormat.format(date)); //2016/11/16 12:08:43

		for (int i = 0; i < 4; i++)
		{
			System.out.print(".");
			Thread.sleep(500);
		}

		System.out.println("End of test.");
	}
}
