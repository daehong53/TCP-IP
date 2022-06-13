package thread;

import java.util.concurrent.Semaphore;

public class Rent {
	
	private final int MAX_PERMIT = 3; 
	private Car[] cars = {new Car("현대"), new Car("기아"), new Car("포드")};
	private boolean[] beingTurn = new boolean[MAX_PERMIT];
	private Semaphore s = new Semaphore(MAX_PERMIT,true); 


	public Car checkOut() throws InterruptedException {
		s.acquire(); 
		return getAvailableCar();
	}

	public Car getAvailableCar() {
		Car car= null;
		
		for(int i =0; i<MAX_PERMIT;i++) {
			if(!beingTurn[i]) {
				beingTurn[i] = true; 
				car = cars[i]; 
				System.out.println(Thread.currentThread().getName() + "님 랜트 완료 : " +car.getName());
				break;
			}
		}
		return car; 
	}

	
	public void returnCar(Car car) {
		
        if(getAvailable(car)) { 
			s.release(); 
		}
	}
 
	public boolean getAvailable(Car car) {
			
		for(int i=0; i<MAX_PERMIT; i++) {
			if(car == cars[i]) { 
				beingTurn[i] = false;
				System.out.println(Thread.currentThread().getName()+ "님 반납완료 : " + car.getName());
				break;
			}
		}
		return true; 
	}
}