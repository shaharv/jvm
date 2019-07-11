package test;

import java.util.Date;
import java.text.DateFormat;
import java.text.SimpleDateFormat;

class Main
{
	static public void main(String[] args) throws InterruptedException
	{
		System.out.println("------------------------------------------------------------");
		System.out.println("Date test: started");
		System.out.println("------------------------------------------------------------");

		DateFormat dateFormat = new SimpleDateFormat("yyyy/MM/dd HH:mm:ss");
		Date date = new Date();

		System.out.println("------------------------------------------------------------");
		System.out.println("The time is " + dateFormat.format(date));
		System.out.println("------------------------------------------------------------");

		Thread.sleep(2000);

		System.out.println("------------------------------------------------------------");
		System.out.println("End of test.");
		System.out.println("------------------------------------------------------------");
	}
}
