package thread;

public class Main {

	public static void main(String[] args) {
		
		Rent rent = new Rent();
		Thread[] turn = new Thread[10];

		
		for(int i = 0; i < turn.length; i++) {
			
			turn[i] = new Thread(new Turn(rent)); 
			turn[i].setName("회원" + (i+1)); 
			turn[i].start(); 
			
		}
		
		try {
			for(int i = 0; i<turn.length; i++) {
				turn[i].join(); 
			}
		}catch(InterruptedException e) {}
	}
}