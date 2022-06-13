package thread;

public class Turn implements Runnable	{
	
	Rent rent;
	Car car;
	
	public Turn(Rent rent) {
		this.rent = rent; 
	}
	
	@Override
	public void run() {
		
		try {
			car = rent.checkOut(); 
			System.out.println(Thread.currentThread().getName() + "님이 랜트 중 입니다.");
			Thread.sleep(1000);
			rent.returnCar(car); 
			
		} catch (InterruptedException e) {
	
			e.printStackTrace();
		}
	}
}